#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "FA_Handling.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "Robot_Handling.h"
#include "Equip_Handling.h"
#include "sch_prm_FBTPM.h"
#include "sch_util.h"
#include "sch_transfer.h"

#include "Errorcode.h"

#include "INF_default.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_sub.h"
#include "INF_sch_CommonUser.h"

#include "INF_sch_CommonUser.h"

//------------------------------------------------------------------------------------------
enum {
	RUN_ALGORITHM_0_DEFAULT = 0 ,
	RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 = 4 ,
	RUN_ALGORITHM_6_INLIGN_PM2_V1 = 6 ,
};
//------------------------------------------------------------------------------------------
int Address_TR_SRC_STATION = -2;
int Address_TR_SRC_SLOT = -2;
int Address_TR_SRC_SLOT2 = -2;
int Address_TR_TRG_STATION = -2;
int Address_TR_TRG_SLOT = -2;
int Address_TR_TRG_SLOT2 = -2;
int Address_TR_FINGER = -2;
//=============================================================================================
int FM_AL_WAFER_SOURCE() {
	return _i_SCH_IO_GET_MODULE_SOURCE( F_AL , 1 );
}
//=============================================================================================
int FM_IC_WAFER_SOURCE() {
	return _i_SCH_IO_GET_MODULE_SOURCE( F_IC , 1 );
}
//=============================================================================================
int FM_IC_WAFER_SLOT() {
	return _i_SCH_IO_GET_MODULE_STATUS( F_IC , 1 );
}
//=============================================================================================
int FM_AL_WAFER_SLOT() {
	return _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 );
}
//=============================================================================================
BOOL FM_AL_HAS_A_WAFER( int CHECKING_SIDE ) {
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_AL , FALSE , -1 ) )	return FALSE;
	if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , 1 ) > 0 ) return TRUE;
	return FALSE;
}
//=============================================================================================
BOOL FM_IC_HAS_A_WAFER( int CHECKING_SIDE , int *slot ) {
	int i;
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( F_IC , FALSE , -1 ) )	return FALSE;
	if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , 1 ) > 0 ) {
		*slot = 1;
		return TRUE;
	}
	for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE(F_IC) ; i++ ) {
		if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , i + 1 ) ) {
			*slot = i + 1;
			return TRUE;
		}
	}
	return FALSE;
}
//=============================================================================================
int ROBOT_TR_FM_PICK_FINGER( int fms , int Station ) {
	if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
		if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) == 0 ) return TA_STATION;
	}
	else if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
		if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) == 0 ) return TB_STATION;
	}
	else {
		if      ( WAFER_STATUS_IN_FM( fms , TA_STATION ) == 0 && WAFER_STATUS_IN_FM( fms , TB_STATION ) != 0 ) {
			return TA_STATION;
		}
		else if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) != 0 && WAFER_STATUS_IN_FM( fms , TB_STATION ) == 0 ) {
			return TB_STATION;
		}
		else if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) != 0 && WAFER_STATUS_IN_FM( fms , TB_STATION ) != 0 ) {
			return -1;
		}
		else {
			return 2;
		}
	}
	return -1;
}
//=============================================================================================
int ROBOT_TR_FM_GET_FINGER_FOR_USE( int fms , int arm ) {
	switch( arm ) {
		case 0 :
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
					return -1;
				}
				else {
					if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) == 0 ) return TA_STATION;
					return -1;
				}
				break;
		case 1 :
				if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
					return -1;
				}
				else {
					if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) == 0 ) return TB_STATION;
					return -1;
				}
				break;
		case 2 :
				if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
					if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) == 0 ) {
						if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) == 0 ) {
							return 2;
						}
					}
				}
				break;
	}
	return -1;
}
//=============================================================================================
int ROBOT_TR_FM_PUT_FINGER_FOR_USE( int fms , int arm ) {
	switch( arm ) {
		case 0 :
				if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
					return -1;
				}
				else {
					if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) != 0 ) return TA_STATION;
					return -1;
				}
				break;
		case 1 :
				if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
					return -1;
				}
				else {
					if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) != 0 ) return TB_STATION;
					return -1;
				}
				break;
		case 2 :
				if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TA_STATION ) && _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , TB_STATION ) ) {
					if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) != 0 ) {
						if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) != 0 ) {
							return 2;
						}
					}
				}
				break;
	}
	return -1;
}
//=============================================================================================
int Transfer_ROBOT_GET_FINGER_FREE_COUNT( int TMATM ) {
	//
	if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
		//
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
				if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) return 1;
			}
			else                                         return 0;
		}
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
				if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) return 1;
			}
			else                                         return 0;
		}
		else if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
				if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) {
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) {
							return 2;
						}
					}
					return 1;
				}
				else {
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) {
							return 1;
						}
					}
					return 0;
				}
			}
			else {
				if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
					if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) {
						return 1;
					}
				}
				return 0;
			}
		}
	}
	else {
		//
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) return 1;
			else                                         return 0;
		}
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) return 1;
			else                                         return 0;
		}
		else if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if      ( ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) && ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) ) return 2;
			else if ( ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) || ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) ) return 1;
			else                                                                                               return 0;
		}
	}
	return 0;
}
//=============================================================================================
int Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( int TMATM , int Station ) {
	BOOL same;
	if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
				if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) return TA_STATION;
			}
		}
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
				if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) return TB_STATION;
			}
		}
		else if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
				if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) {
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) {
							return( ROBOT_Get_FAST_FINGER_AT_STATION( TMATM , TA_STATION , TB_STATION , Station , ROBOT_GET_ROTATE_POSITION( TMATM ) , ROBOT_GET_MOVE_POSITION( TMATM ) , &same ) );
						}
					}
					return TA_STATION;
				}
				else {
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) {
							return TB_STATION;
						}
					}
					return -1;
				}
			}
			else {
				if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
					if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) {
						return TB_STATION;
					}
				}
				return -1;
			}
		}
	}
	else {
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) return TA_STATION;
		}
		else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) return TB_STATION;
		}
		else if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
			if      ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 && WAFER_STATUS_IN_TM( TMATM , TB_STATION ) != 0 ) {
				return TA_STATION;
			}
			else if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) != 0 && WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
				return TB_STATION;
			}
			else if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) != 0 && WAFER_STATUS_IN_TM( TMATM , TB_STATION ) != 0 ) {
				return -1;
			}
			else {
				return( ROBOT_Get_FAST_FINGER_AT_STATION( TMATM , TA_STATION , TB_STATION , Station , ROBOT_GET_ROTATE_POSITION( TMATM ) , ROBOT_GET_MOVE_POSITION( TMATM ) , &same ) );
			}
		}
	}
	return -1;
}
//=============================================================================================
int Transfer_ROBOT_GET_TR_FINGER_FOR_USE( int TMATM , int arm ) {
	switch( arm ) {
		case 0 :
				if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					return -1;
				}
				else {
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
							if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) return TA_STATION;
						}
					}
					else {
						if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) return TA_STATION;
					}
					return -1;
				}
				break;
		case 1 :
				if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					return -1;
				}
				else {
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
							if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) return TB_STATION;
						}
					}
					else {
						if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) return TB_STATION;
					}
					return -1;
				}
				break;
		case 2 :
				if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
							if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) return TA_STATION;
						}
					}
					else {
						if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) return TA_STATION;
					}
				}
				else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
							if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) return TB_STATION;
						}
					}
					else {
						if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) return TB_STATION;
					}
				}
				else if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) {
							if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) == 0 ) return TA_STATION;
						}
						if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) {
							if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) == 0 ) return TB_STATION;
						}
					}
					else {
						if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) == 0 ) return TA_STATION;
						if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) == 0 ) return TB_STATION;
					}
				}
				break;
	}
	return -1;
}
//=============================================================================================
int Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( int TMATM , int arm ) {
	switch( arm ) {
		case 0 :
				if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					return -1;
				}
				else {
					if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) != 0 ) return TA_STATION;
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) != 0 ) return TA_STATION;
					}
					return -1;
				}
				break;
		case 1 :
				if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					return -1;
				}
				else {
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) != 0 ) return TB_STATION;
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) != 0 ) return TB_STATION;
					}
					return -1;
				}
				break;
		case 2 :
				if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) != 0 ) return TA_STATION;
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) != 0 ) return TA_STATION;
					}
				}
				else if ( !_i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) != 0 ) return TB_STATION;
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) != 0 ) return TB_STATION;
					}
				}
				else if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 0 ) && _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , 1 ) ) {
					if ( WAFER_STATUS_IN_TM( TMATM , TA_STATION ) != 0 ) return TA_STATION;
					if ( WAFER_STATUS_IN_TM( TMATM , TB_STATION ) != 0 ) return TB_STATION;
					if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
						if ( WAFER_STATUS_IN_TM2( TMATM , TA_STATION ) != 0 ) return TA_STATION;
						if ( WAFER_STATUS_IN_TM2( TMATM , TB_STATION ) != 0 ) return TB_STATION;
					}
				}
				break;
	}
	return -1;
}
//=============================================================================================
BOOL Transfer_ROBOT_TR_FINGER_HAS_WAFER( int TMATM , int STATION ) {
	if ( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) {
		if ( WAFER_STATUS_IN_TM( TMATM , STATION ) != 0 ) return TRUE;
		if ( WAFER_STATUS_IN_TM2( TMATM , STATION ) != 0 ) return TRUE;
		return FALSE;
	}
	else {
		if ( WAFER_STATUS_IN_TM( TMATM , STATION ) != 0 ) return TRUE;
		else                                      return FALSE;
	}
}
//=============================================================================================
int Transfer_RUN_ALG_STYLE_GET() {
	switch( SYSTEM_RUN_ALG_STYLE_GET() ) {
	case RUN_ALGORITHM_0_DEFAULT			:	return RUN_ALGORITHM_0_DEFAULT;
	case RUN_ALGORITHM_4_INLIGN_EXPRESS_V1	:	return RUN_ALGORITHM_4_INLIGN_EXPRESS_V1;
	case RUN_ALGORITHM_6_INLIGN_PM2_V1		:	return RUN_ALGORITHM_6_INLIGN_PM2_V1;
	default									:	return _SCH_COMMON_TRANSFER_OPTION( 0 , -1 );
	}
}
//=============================================================================================
int Transfer_IO_Reading( char *ioname , int *ioaddr ) {
	int CommStatus;
	if ( *ioaddr == -2 ) *ioaddr = _FIND_FROM_STRING( _K_D_IO , ioname );
	if ( *ioaddr <   0 ) return 0;
	return _dREAD_DIGITAL( *ioaddr , &CommStatus );
}
//=============================================================================================
int Transfer_FM_ARM_ALL( int fms ) {
	if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) return FALSE;
	if ( !_i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 1 ) ) return FALSE;
	return TRUE;
}
//=============================================================================================
BOOL Transfer_Enable_InterlockFinger_Check_for_Move( int tms , int finger , int pickch , int placech , int *rfinger , BOOL checkcond ) { // 2003.10.17
	if ( finger == 2 ) {
		*rfinger = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( tms , pickch );
		switch( _i_SCH_PRM_GET_MODULE_TRANSFER_CONTROL_MODE() ) { // 2003.11.05
		case 3 :
		case 4 :
		case 5 :
		case 11 : // 2005.10.10
		case 12 : // 2005.10.10
		case 13 : // 2005.10.10
		case 14 : // 2005.10.10
		case 15 : // 2005.10.10
			return TRUE;
			break;
		}
		if ( checkcond ) {
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tms , pickch , *rfinger , -1 ) ) {
				if ( *rfinger == TA_STATION ) {
					if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TB_STATION ) ) return FALSE;
					if ( WAFER_STATUS_IN_TM( tms , TB_STATION ) > 0 ) return FALSE;
					*rfinger = TB_STATION;
				}
				else {
					if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TA_STATION ) ) return FALSE;
					if ( WAFER_STATUS_IN_TM( tms , TA_STATION ) > 0 ) return FALSE;
					*rfinger = TA_STATION;
				}
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tms , pickch , *rfinger , -1 ) ) return FALSE;
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tms , placech , *rfinger , pickch ) ) return FALSE;
			}
			else {
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tms , placech , *rfinger , pickch ) ) {
					if ( *rfinger == TA_STATION ) {
						if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TB_STATION ) ) return FALSE;
						if ( WAFER_STATUS_IN_TM( tms , TB_STATION ) > 0 ) return FALSE;
						*rfinger = TB_STATION;
					}
					else {
						if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TA_STATION ) ) return FALSE;
						if ( WAFER_STATUS_IN_TM( tms , TA_STATION ) > 0 ) return FALSE;
						*rfinger = TA_STATION;
					}
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tms , pickch , *rfinger , -1 ) ) return FALSE;
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tms , placech , *rfinger , pickch ) ) return FALSE;
				}
			}
		}
	}
	else {
		*rfinger = finger;
		switch( _i_SCH_PRM_GET_MODULE_TRANSFER_CONTROL_MODE() ) { // 2003.11.05
		case 3 :
		case 4 :
		case 5 :
		case 11 : // 2005.10.10
		case 12 : // 2005.10.10
		case 13 : // 2005.10.10
		case 14 : // 2005.10.10
		case 15 : // 2005.10.10
			return TRUE;
			break;
		}
		if ( checkcond ) {
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tms , pickch , *rfinger , -1 ) ) return FALSE;
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tms , placech , *rfinger , pickch ) ) return FALSE;
		}
	}
	if ( ( *rfinger != TA_STATION ) && ( *rfinger != TB_STATION ) ) return FALSE; // 2006.02.23
	return TRUE;
}

BOOL Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( int *Finger , int picktms , int placetms , int pickch , int placech ) { // 2004.08.19
//	if ( *Finger == 2 ) { // 2004.09.02
	if ( *Finger >= 2 ) { // 2004.09.02
		if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , picktms , pickch , TA_STATION , -1 ) ) {
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , picktms , pickch , TB_STATION , -1 ) ) {
				return FALSE;
			}
			else {
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , placetms , placech , TB_STATION , pickch ) ) {
					return FALSE;
				}
				else {
					*Finger = TB_STATION;
					return TRUE;
				}
			}
		}
		else {
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , placetms , placech , TA_STATION , pickch ) ) {
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , picktms , pickch , TB_STATION , -1 ) ) {
					return FALSE;
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , placetms , placech , TB_STATION , pickch ) ) {
						return FALSE;
					}
					else {
						*Finger = TB_STATION;
						return TRUE;
					}
				}
			}
			else {
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , picktms , pickch , TB_STATION , -1 ) ) {
					*Finger = TA_STATION;
					return TRUE;
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , placetms , placech , TB_STATION , pickch ) ) {
						*Finger = TA_STATION;
						return TRUE;
					}
					else {
						*Finger = 2;
						return TRUE;
					}
				}
			}
		}
	}
	else if ( *Finger == TA_STATION ) {
		if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , picktms , pickch , TA_STATION , -1 ) ) {
			return FALSE;
		}
		else {
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , placetms , placech , TA_STATION , pickch ) ) {
				return FALSE;
			}
			else {
				return TRUE;
			}
		}
	}
	else if ( *Finger == TB_STATION ) {
		if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , picktms , pickch , TB_STATION , -1 ) ) {
			return FALSE;
		}
		else {
			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , placetms , placech , TB_STATION , pickch ) ) {
				return FALSE;
			}
			else {
				return TRUE;
			}
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Transfer Part for LL Type
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Transfer_User_Running_LL_Code( int Mode , int tmatm , int Finger , int Src_Station , int Src_Slot , int Trg_Station , int Trg_Slot ) {
	int CLSYES , DMSKIP , SDMSKIPCOUNT , SDMSKIP[MAX_SDUMMY_DEPTH] , DMYES , CLSFIND;
	int i , j , f , s , c , d , sl , sc , pmalch , tx , lx , sres;
	if ( ( Mode == TR_PICK ) || ( Mode == TR_MOVE ) ) {
		//--------------------------------------------------------
		Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
		//--------------------------------------------------------
		sc = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , 1 ) + CM1;
		//--------------------------------------------------------
		if ( EQUIP_PICK_FROM_CHAMBER_TT( tmatm , TR_CHECKING_SIDE , Src_Station , Finger , Src_Slot , 1 , FALSE , sc , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
		//
		if ( Src_Station < PM1 ) { // 2004.10.11
			if ( ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 3 ) && _SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) ) {
				while( TRUE ) {
					i = EQUIP_ALIGNING_STATUS_FOR_TM_MAL( TR_CHECKING_SIDE );
					if ( i == SYS_SUCCESS ) break;
					if ( i != SYS_RUNNING ) return SYS_ABORTED;
					Sleep(250);
				}
				if ( _i_SCH_EQ_PICK_FROM_TM_MAL( TR_CHECKING_SIDE , Finger , Src_Slot , Src_Station , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
			}
			else {
				//============================================================================================
				// 2005.12.16
				//============================================================================================
				if ( !_SCH_MODULE_GET_USE_ENABLE( AL , FALSE , -1 ) ) {
					if ( _i_SCH_PRM_GET_CLSOUT_AL_TO_PM() != 0 ) {
						pmalch = PM1 + _i_SCH_PRM_GET_CLSOUT_AL_TO_PM() - 1;
						//
						if ( !Transfer_Enable_Check( pmalch ) ) return SYS_ABORTED;
						if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , pmalch , Finger , Src_Slot , 1 , 0 , sc , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
						//
						if ( !_i_SCH_EQ_SPAWN_PROCESS( 0 , -1 , pmalch , sc , Src_Slot , Finger , "?" , 13 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) return SYS_ABORTED;
						//
						if ( EQUIP_WAIT_PROCESS( 0 , pmalch ) != SYS_SUCCESS ) return SYS_ABORTED;
						//
						if ( ( Mode == TR_MOVE ) && ( pmalch == Trg_Station ) ) return SYS_SUCCESS;
						//
						if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , pmalch , Finger , Src_Slot , 1 , FALSE , sc , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
					}
				}
				//============================================================================================
			}
		}
		//
	}
	if ( ( Mode == TR_PLACE ) || ( Mode == TR_MOVE ) ) {
		//--------------------------------------------------------
		sc = WAFER_SOURCE_IN_TM( tmatm , Finger ) + CM1;
		//--------------------------------------------------------
		if ( Trg_Station < PM1 ) {
			_i_SCH_IO_SET_MODULE_STATUS( Trg_Station , Trg_Slot , CWM_ABSENT );
			//============================================================================================
			// 2005.12.16
			//============================================================================================
			if ( !_SCH_MODULE_GET_USE_ENABLE( IC , FALSE , -1 ) ) {
				if ( _i_SCH_PRM_GET_CLSOUT_IC_TO_PM() != 0 ) {
					pmalch = PM1 + _i_SCH_PRM_GET_CLSOUT_IC_TO_PM() - 1;
					//
					if ( !Transfer_Enable_Check( pmalch ) ) return SYS_ABORTED;
					if ( ( Mode != TR_MOVE ) || ( pmalch != Src_Station ) ) {
						if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , pmalch , Finger , Trg_Slot , 1 , 0 , sc , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
						//
						if ( !_i_SCH_EQ_SPAWN_PROCESS( 0 , -1 , pmalch , sc , Src_Slot , Finger , "?" , 13 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) return SYS_ABORTED;
						//
						if ( EQUIP_WAIT_PROCESS( 0 , pmalch ) != SYS_SUCCESS ) return SYS_ABORTED;
						//
						if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , pmalch , Finger , Trg_Slot , 1 , FALSE , sc , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
					}
				}
			}
			//============================================================================================
		}
		if ( EQUIP_PLACE_TO_CHAMBER_TT( tmatm , TR_CHECKING_SIDE , Trg_Station , Finger , Trg_Slot , 1 , 0 , sc , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
		//--------------------------------------------------------
		Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
		//--------------------------------------------------------
	}
	if ( Mode == TR_CLEAR ) {
		//============================================================================================================
		// 2005.01.06
		//============================================================================================================
		if ( !Transfer_Enable_Check_until_Process_Finished() ) {
			return SYS_ABORTED;
		}
		//============================================================================================================
		DMSKIP = 0;
		if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() != DM_NOT_USE ) && ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() > 0 ) ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() ) > 0 ) {
				if ( _i_SCH_IO_GET_MODULE_SOURCE( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() , 1 ) == MAX_CASSETTE_SIDE ) { // 2002.05.10
					if ( Transfer_Enable_Check( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) ) {
						DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
					}
				}
			}
		}
		//------------------------------------------------------------------------------------------
		//-- 2002.05.10
		//------------------------------------------------------------------------------------------
		SDMSKIPCOUNT = 0;
		for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
			if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) != 0 ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) , _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d ) ) > 0 ) {
					if ( _i_SCH_IO_GET_MODULE_SOURCE( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) , 1 ) > MAX_CASSETTE_SIDE ) {
						if ( Transfer_Enable_Check( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) ) ) {
							SDMSKIP[SDMSKIPCOUNT] = _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d );
							SDMSKIPCOUNT++;
						}
					}
				}
			}
		}
		//
		DMYES = 0;
		for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
			if ( Transfer_Enable_Check( i ) ) {
				for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
					if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == i ) break;
				}
				if ( d == MAX_SDUMMY_DEPTH ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_SOURCE( i , 1 ) == MAX_CASSETTE_SIDE ) { // 2002.05.10
							if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) )  return SYS_ABORTED;
							if ( DMYES != 0 ) return SYS_ABORTED;
							DMYES++;
						}
						if ( _i_SCH_IO_GET_MODULE_SOURCE( i , 1 ) > MAX_CASSETTE_SIDE ) {
							for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
								if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) - 1 + PM1 ) ) break;
							}
							if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
						}
					}
				}
			}
		}
		//===================================================================================================
		// 2006.02.23
		//===================================================================================================
		while ( TRUE ) {
			lx = 0;
			for ( tx = 0 ; tx < MAX_TM_CHAMBER_DEPTH ; tx++ ) {
				//==========================================================================
				if ( !Transfer_Enable_Check( tx + TM ) ) continue; // 2006.02.23
				//==========================================================================
				if ( tx == 0 ) { // 2006.02.23
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
						if ( WAFER_SOURCE_IN_TM( 0,TA_STATION ) == MAX_CASSETTE_SIDE ) { // 2002.05.10
							if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) )  return SYS_ABORTED;
							if ( DMYES  != 0 ) return SYS_ABORTED;
							DMYES++;
						}
						if ( WAFER_SOURCE_IN_TM( 0,TA_STATION ) > MAX_CASSETTE_SIDE ) { // 2002.05.10
							for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
								if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == ( _i_SCH_IO_GET_MODULE_STATUS( 0,TA_STATION ) - 1 + PM1 ) ) break;
							}
							if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
						}
					}
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
						if ( WAFER_SOURCE_IN_TM( 0,TB_STATION ) == MAX_CASSETTE_SIDE ) { // 2002.05.10
							if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) )  return SYS_ABORTED;
							if ( DMYES  != 0 ) return SYS_ABORTED;
							DMYES++;
						}
						if ( WAFER_SOURCE_IN_TM( 0,TB_STATION ) > MAX_CASSETTE_SIDE ) { // 2002.05.10
							for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
								if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == ( _i_SCH_IO_GET_MODULE_STATUS( 0,TB_STATION ) - 1 + PM1 ) ) break;
							}
							if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
						}
					}
				}
				//
//				if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM ) ) { // 2006.02.23
				if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM + tx ) ) { // 2006.02.23
//					if ( EQUIP_CLEAR_CHAMBER( TM , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
					if ( EQUIP_CLEAR_CHAMBER( TM + tx , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
				}
				//
				CLSYES = -1;
				for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
					if ( DMSKIP != i ) {
						for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
							if ( SDMSKIP[d] == i ) break;
						}
						if ( d == SDMSKIPCOUNT ) {
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PLACE ) ) { // 2006.02.23
								if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
									CLSYES = i;
								}
							}
						}
					}
				}
				//
				if ( tx == 0 ) { // 2006.02.23
					if ( ( Transfer_Enable_Check( IC ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( IC , 1 ) > 0 ) ) {
						//=======================================
						lx++; // 2006.02.23
						//=======================================
			//			if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , IC ); // 2003.10.17
			//			else               f = Finger; // 2003.10.17
						s = _i_SCH_IO_GET_MODULE_STATUS( IC , 1 );
						c = _i_SCH_IO_GET_MODULE_SOURCE( IC , 1 ) + CM1;
						//--------------------------------------------------------
						if ( !Transfer_Enable_InterlockFinger_Check_for_Move( 0 , Finger , IC , c , &f , ( c < PM1 ) ) ) return SYS_ERROR; // 2003.10.17
						//--------------------------------------------------------
						if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , IC , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
						_i_SCH_IO_MTL_SAVE();
						if ( c < PM1 ) {
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
							//--------------------------------------------------------
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( c , s );
							//--------------------------------------------------------
						}
						else if ( c == PM1 ) {
							DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
							//--------------------------------------------------------
							if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , f , -1 ) ) return SYS_ERROR; // 2003.10.17
							//--------------------------------------------------------
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
							//--------------------------------------------------------
						}
						else if ( c > PM1 ) { // 2002.05.10
							for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
								if ( SDMSKIP[d] == s - 1 + PM1 ) break;
							}
							if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
							for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
								if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
							}
							if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
							sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
							SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
							SDMSKIPCOUNT++;
							//--------------------------------------------------------
							if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , f , -1 ) ) return SYS_ERROR; // 2003.10.17
							//--------------------------------------------------------
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
							//--------------------------------------------------------
						}
						_i_SCH_IO_MTL_SAVE();
					}
					if ( ( Transfer_Enable_Check( AL ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( AL , 1 ) > 0 ) ) {
						//=======================================
						lx++; // 2006.02.23
						//=======================================
			//			if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , AL ); // 2003.10.17
			//			else               f = Finger; // 2003.10.17
						s = _i_SCH_IO_GET_MODULE_STATUS( AL , 1 );
						c = _i_SCH_IO_GET_MODULE_SOURCE( AL , 1 ) + CM1;
						//--------------------------------------------------------
						if ( !Transfer_Enable_InterlockFinger_Check_for_Move( 0 , Finger , AL , c , &f , ( c < PM1 ) ) ) return SYS_ERROR; // 2003.10.17
						//--------------------------------------------------------
						if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , AL , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
						_i_SCH_IO_MTL_SAVE();
						if ( c < PM1 ) {
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
							//--------------------------------------------------------
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( c , s );
							//--------------------------------------------------------
						}
						else if ( c == PM1 ) {
							DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
							//--------------------------------------------------------
							if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , f , -1 ) ) return SYS_ERROR; // 2003.10.17
							//--------------------------------------------------------
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
							//--------------------------------------------------------
						}
						else if ( c > PM1 ) { // 2002.05.10
							for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
								if ( SDMSKIP[d] == s - 1 + PM1 ) break;
							}
							if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
							for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
								if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
							}
							if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
							sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
							SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
							SDMSKIPCOUNT++;
							//--------------------------------------------------------
							if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , f , -1 ) ) return SYS_ERROR; // 2003.10.17
							//--------------------------------------------------------
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
							//--------------------------------------------------------
						}
						_i_SCH_IO_MTL_SAVE();
					}
				}
				//
				if ( CLSYES >= 0 ) {
					CLSFIND = FALSE;
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( tx,TA_STATION ) && ( WAFER_STATUS_IN_TM( tx , TA_STATION ) <= 0 ) ) {
						if ( Finger != 1 ) {
							CLSFIND = TRUE;
						}
					}
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( tx,TB_STATION ) && ( WAFER_STATUS_IN_TM( tx , TB_STATION ) <= 0 ) ) {
						if ( Finger != 0 ) {
							CLSFIND = TRUE;
						}
					}
					if ( CLSFIND ) {
						for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
							if ( DMSKIP != i ) {
								sres = FALSE; // 2006.03.27
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PICK ) ) { // 2006.02.23
									if ( tx == 0 ) {
										sres = TRUE;
									}
									else {
										if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , i , G_PICK ) ) sres = TRUE;
									}
								}
								if ( sres ) { // 2006.03.27
									if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) ) {
										//=======================================
										lx++; // 2006.02.23
										//=======================================
										//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i ); // 2003.10.17
										//else               f = Finger; // 2003.10.17
										s = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
										c = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 ) + CM1;
										//
										if ( tx == 0 ) {
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Move( tx , Finger , i , c , &f , ( c < PM1 ) ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
												if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
											}
											if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											_i_SCH_IO_MTL_SAVE();
											if ( c < PM1 ) {
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												Transfer_Cass_Info_Operation_After_Place( c , s );
												//--------------------------------------------------------
											}
											else if ( c == PM1 ) {
												DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , f , -1 ) ) return SYS_ERROR; // 2003.10.17
												//--------------------------------------------------------
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
												//--------------------------------------------------------
											}
											else if ( c > PM1 ) { // 2002.05.10
												for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
													if ( SDMSKIP[d] == s - 1 + PM1 ) break;
												}
												if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
												for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
													if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
												}
												if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
												sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
												SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
												SDMSKIPCOUNT++;
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , f , -1 ) ) return SYS_ERROR; // 2003.10.17
												//--------------------------------------------------------
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
												//--------------------------------------------------------
											}
										}
										else { // 2006.02.23
											for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
												if ( i != j ) {
													if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
														if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
															if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
																//--------------------------------------------------------
																if ( !Transfer_Enable_InterlockFinger_Check_for_Move( tx , Finger , i , j , &f , TRUE ) ) continue;
																//--------------------------------------------------------
																if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
																	if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
																}
																if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
																_i_SCH_IO_MTL_SAVE();
																//--------------------------------------------------------
																if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
																if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
																//--------------------------------------------------------
																break;
															}
														}
													}
												}
											}
										}
										_i_SCH_IO_MTL_SAVE();
									}
								}
							}
						}
					}
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( tx,TA_STATION ) && ( WAFER_STATUS_IN_TM( tx , TA_STATION ) > 0 ) ) {
						CLSFIND = -1;
						if ( Finger != 1 ) {
							if ( _SCH_CASSETTE_LAST_RESULT_GET( WAFER_SOURCE_IN_TM( tx , TA_STATION ) + CM1 , WAFER_STATUS_IN_TM( tx , TA_STATION ) ) == -2 ) {
								//=======================================
								lx++; // 2006.02.23
								//=======================================
								s = WAFER_STATUS_IN_TM( tx , TA_STATION );
								c = WAFER_SOURCE_IN_TM( tx , TA_STATION ) + CM1;
								if ( tx == 0 ) { // 2006.02.23
									if ( c < PM1 ) {
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
										case 5 :
										case 6 :
											if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
												return SYS_ERROR;
											}
											break;
										}
										//--------------------------------------------------------
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										//--------------------------------------------------------
										Transfer_Cass_Info_Operation_After_Place( c , s );
										//--------------------------------------------------------
									}
									else if ( c == PM1 ) {
										DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										//--------------------------------------------------------
										Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
										//--------------------------------------------------------
									}
									else if ( c > PM1 ) { // 2002.05.10
										for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
											if ( SDMSKIP[d] == s - 1 + PM1 ) break;
										}
										if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
										for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
											if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
										}
										if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
										sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
										SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
										SDMSKIPCOUNT++;
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										//--------------------------------------------------------
										Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
										//--------------------------------------------------------
									}
								}
								else { // 2006.02.23
									for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
												if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
													//--------------------------------------------------------
													if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , TA_STATION , -1 ) ) continue;
													//--------------------------------------------------------
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													break;
												}
											}
										}
									}
								}
								_i_SCH_IO_MTL_SAVE();
							}
							else {
								for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
									if ( DMSKIP != i ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PLACE ) ) { // 2006.02.23
											if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
												CLSFIND = i;
											}
										}
									}
								}
								if ( CLSFIND >= 0 ) {
									//=======================================
									lx++; // 2006.02.23
									//=======================================
									s = WAFER_STATUS_IN_TM( tx , TA_STATION );
									c = WAFER_SOURCE_IN_TM( tx , TA_STATION ) + CM1;
									//-----------------------------------------------------
									if ( tx == 0 ) { // 2006.02.23
										if ( c < PM1 ) { // 2003.10.17
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
											case 5 :
											case 6 :
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												break;
											}
											//--------------------------------------------------------
										}
									}
									if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , CLSFIND , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									_i_SCH_IO_MTL_SAVE();
									if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) return SYS_ABORTED;
									}
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
										if ( EQUIP_CLEAR_CHAMBER( CLSFIND , TA_STATION , s ) == SYS_ABORTED ) return SYS_ABORTED;
									}
									//==================================================================================================
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , CLSFIND , G_PICK ) ) { // 2006.02.23
										sres = FALSE; // 2006.03.27
										if ( tx == 0 ) {
											sres = TRUE;
										}
										else {
											if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , CLSFIND , G_PICK ) ) sres = TRUE;
										}
										if ( sres ) { // 2006.03.27
											if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , CLSFIND , TA_STATION , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											_i_SCH_IO_MTL_SAVE();
											if ( tx == 0 ) {
												if ( c < PM1 ) {
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													Transfer_Cass_Info_Operation_After_Place( c , s );
													//--------------------------------------------------------
												}
												else if ( c == PM1 ) {
													DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
													//--------------------------------------------------------
													if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
													//--------------------------------------------------------
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
													//--------------------------------------------------------
												}
												else if ( c > PM1 ) { // 2002.05.10
													for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
														if ( SDMSKIP[d] == s - 1 + PM1 ) break;
													}
													if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
													for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
														if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
													}
													if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
													sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
													SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
													SDMSKIPCOUNT++;
													//--------------------------------------------------------
													if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
													//--------------------------------------------------------
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
													//--------------------------------------------------------
												}
											}
											else { // 2006.02.23
												for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
													if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
														if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
															if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
																//--------------------------------------------------------
																if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , TA_STATION , -1 ) ) continue;
																//--------------------------------------------------------
																if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
																if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
																//--------------------------------------------------------
																break;
															}
														}
													}
												}
											}
										}
									}
									_i_SCH_IO_MTL_SAVE();
								}
								else {
//									return SYS_ABORTED; // 2006.03.27
									if ( tx == 0 ) return SYS_ABORTED; // 2006.03.27
								}
							}
						}
					}
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( tx,TB_STATION ) && ( WAFER_STATUS_IN_TM( tx , TB_STATION ) > 0 ) ) {
						CLSFIND = -1;
						if ( Finger != 0 ) {
							if ( _SCH_CASSETTE_LAST_RESULT_GET( WAFER_SOURCE_IN_TM( tx , TB_STATION ) + CM1 , WAFER_STATUS_IN_TM( tx , TB_STATION ) ) == -2 ) {
								//=======================================
								lx++; // 2006.02.23
								//=======================================
								s = WAFER_STATUS_IN_TM( tx , TB_STATION );
								c = WAFER_SOURCE_IN_TM( tx , TB_STATION ) + CM1;
								if ( tx == 0 ) { // 2006.02.23
									if ( c < PM1 ) {
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
										case 5 :
										case 6 :
											if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
												return SYS_ERROR;
											}
											break;
										}
										//--------------------------------------------------------
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										//--------------------------------------------------------
										Transfer_Cass_Info_Operation_After_Place( c , s );
										//--------------------------------------------------------
									}
									else if ( c == PM1 ) {
										DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										//--------------------------------------------------------
										Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
										//--------------------------------------------------------
									}
									else if ( c > PM1 ) { // 2002.05.10
										for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
											if ( SDMSKIP[d] == s - 1 + PM1 ) break;
										}
										if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
										for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
											if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
										}
										if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
										sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
										SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
										SDMSKIPCOUNT++;
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										//--------------------------------------------------------
										Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
										//--------------------------------------------------------
									}
								}
								else { // 2006.02.23
									for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
												if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
													//--------------------------------------------------------
													if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , TB_STATION , -1 ) ) continue;
													//--------------------------------------------------------
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													break;
												}
											}
										}
									}
								}
								_i_SCH_IO_MTL_SAVE();
							}
							else {
								for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
									if ( DMSKIP != i ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PLACE ) ) { // 2006.02.23
											if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
												CLSFIND = i;
											}
										}
									}
								}
								if ( CLSFIND >= 0 ) {
									//=======================================
									lx++; // 2006.02.23
									//=======================================
									s = WAFER_STATUS_IN_TM( tx , TB_STATION );
									c = WAFER_SOURCE_IN_TM( tx , TB_STATION ) + CM1;
									//-----------------------------------------------------
									if ( tx == 0 ) { // 2006.02.23
										if ( c < PM1 ) { // 2003.10.17
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
											case 5 :
											case 6 :
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												break;
											}
											//--------------------------------------------------------
										}
									}
									if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , CLSFIND , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									_i_SCH_IO_MTL_SAVE();
									if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) return SYS_ABORTED;
									}
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
										if ( EQUIP_CLEAR_CHAMBER( CLSFIND , TB_STATION , s ) == SYS_ABORTED ) return SYS_ABORTED;
									}
									//==================================================================================================
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , CLSFIND , G_PICK ) ) { // 2006.02.23
										sres = FALSE; // 2006.03.27
										if ( tx == 0 ) {
											sres = TRUE;
										}
										else {
											if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , CLSFIND , G_PICK ) ) sres = TRUE;
										}
										if ( sres ) { // 2006.03.27
											if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , CLSFIND , TB_STATION , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											_i_SCH_IO_MTL_SAVE();
											if ( tx == 0 ) {
												if ( c < PM1 ) {
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													Transfer_Cass_Info_Operation_After_Place( c , s );
													//--------------------------------------------------------
												}
												else if ( c == PM1 ) {
													DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
													//--------------------------------------------------------
													if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
													//--------------------------------------------------------
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
													//--------------------------------------------------------
												}
												else if ( c > PM1 ) { // 2002.05.10
													for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
														if ( SDMSKIP[d] == s - 1 + PM1 ) break;
													}
													if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
													for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
														if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
													}
													if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
													sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
													SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
													SDMSKIPCOUNT++;
													//--------------------------------------------------------
													if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
													//--------------------------------------------------------
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
													if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
													//--------------------------------------------------------
													Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
													//--------------------------------------------------------
												}
											}
											else { // 2006.02.23
												for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
													if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
														if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
															if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
																//--------------------------------------------------------
																if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , TB_STATION , -1 ) ) continue;
																//--------------------------------------------------------
																if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
																if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
																//--------------------------------------------------------
																break;
															}
														}
													}
												}
											}
										}
									}
									_i_SCH_IO_MTL_SAVE();
								}
								else {
//									return SYS_ABORTED; // 2006.03.27
									if ( tx == 0 ) return SYS_ABORTED; // 2006.03.27
								}
							}
						}
					}
					for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
						if ( DMSKIP != i ) {
							sres = FALSE; // 2006.03.27
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PICK ) ) { // 2006.02.23
								if ( tx == 0 ) {
									sres = TRUE;
								}
								else {
									if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , i , G_PICK ) ) sres = TRUE;
								}
							}
							if ( sres ) { // 2006.03.27
			//					if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) ) { // 2003.10.23
								if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) != 2 ) && ( ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) || ( _i_SCH_PRM_GET_MRES_SUCCESS_SCENARIO( i ) == MRES_SUCCESS_LIKE_NOTRUN ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) ) { // 2003.10.23
									//=======================================
									lx++; // 2006.02.23
									//=======================================
									//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i ); // 2003.10.17
									//else               f = Finger; // 2003.10.17
									s = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
									c = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 ) + CM1;
									//--------------------------------------------------------
									if ( tx == 0 ) { // 2006.02.23
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Move( 0 , Finger , i , c , &f , ( c < PM1 ) ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( c < PM1 ) {
											//--------------------------------------------------------
											switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
											case 5 :
											case 6 :
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												break;
											}
										}
										//--------------------------------------------------------
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
											if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
										}
										if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										_i_SCH_IO_MTL_SAVE();
										if ( c < PM1 ) {
											//--------------------------------------------------------
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											//--------------------------------------------------------
											Transfer_Cass_Info_Operation_After_Place( c , s );
											//--------------------------------------------------------
										}
										else if ( c == PM1 ) {
											DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , f , -1 ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											//--------------------------------------------------------
											Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
											//--------------------------------------------------------
										}
										else if ( c > PM1 ) { // 2002.05.10
											for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
												if ( SDMSKIP[d] == s - 1 + PM1 ) break;
											}
											if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
											for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
												if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
											}
											if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
											sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
											SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
											SDMSKIPCOUNT++;
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , f , -1 ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											//--------------------------------------------------------
											Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
											//--------------------------------------------------------
										}
									}
									else { // 2006.02.23
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( i != j ) {
												if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
													if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
														if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
															//--------------------------------------------------------
															if ( !Transfer_Enable_InterlockFinger_Check_for_Move( tx , Finger , i , j , &f , TRUE ) ) continue;
															//--------------------------------------------------------
															if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
																if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
															}
															if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
															_i_SCH_IO_MTL_SAVE();
															//--------------------------------------------------------
															if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
															if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
															//--------------------------------------------------------
															break;
														}
													}
												}
											}
										}
									}
									_i_SCH_IO_MTL_SAVE();
								}
							}
						}
					}
					for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
						if ( DMSKIP != i ) {
							sres = FALSE; // 2006.03.27
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PICK ) ) { // 2006.02.23
								if ( tx == 0 ) {
									sres = TRUE;
								}
								else {
									if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , i , G_PICK ) ) sres = TRUE;
								}
							}
							if ( sres ) { // 2006.03.27
								if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) ) {
									//=======================================
									lx++; // 2006.02.23
									//=======================================
									//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i ); // 2003.10.17
									//else               f = Finger; // 2003.10.17
									s = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
									c = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 ) + CM1;
									//--------------------------------------------------------
									if ( tx == 0 ) {
										//--------------------------------------------------------
										if ( !Transfer_Enable_InterlockFinger_Check_for_Move( 0 , Finger , i , c , &f , ( c < PM1 ) ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( c < PM1 ) { // 2004.01.30
											switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
											case 5 :
											case 6 :
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												break;
											}
										}
									}
									else {
										if ( !Transfer_Enable_InterlockFinger_Check_for_Move( tx , Finger , i , -1 , &f , TRUE ) ) continue;
									}
									//--------------------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
										if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
									}
									if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									_i_SCH_IO_MTL_SAVE();
									CLSFIND = -1;
									for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
										if ( DMSKIP != j ) {
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PLACE ) ) { // 2006.02.23
												if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 1 ) ) {
													CLSFIND = j;
												}
											}
										}
									}
									if ( CLSFIND >= 0 ) {
										s = WAFER_STATUS_IN_TM( tx , f );
										c = WAFER_SOURCE_IN_TM( tx , f ) + CM1;
										if ( tx == 0 ) {
											if ( c < PM1 ) { // 2003.10.17
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , f , -1 ) ) return SYS_ERROR; // 2003.10.17
												//--------------------------------------------------------
											}
										}
										if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , CLSFIND , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										_i_SCH_IO_MTL_SAVE();
										if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) return SYS_ABORTED;
										}
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
											if ( EQUIP_CLEAR_CHAMBER( CLSFIND , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
										}
										if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , CLSFIND , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
										_i_SCH_IO_MTL_SAVE();
										if ( tx == 0 ) {
											if ( c < PM1 ) {
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												Transfer_Cass_Info_Operation_After_Place( c , s );
												//--------------------------------------------------------
											}
											else if ( c == PM1 ) {
												DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , f , -1 ) ) return SYS_ERROR; // 2003.10.17
												//--------------------------------------------------------
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
												//--------------------------------------------------------
											}
											else if ( c > PM1 ) { // 2002.05.10
												for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
													if ( SDMSKIP[d] == s - 1 + PM1 ) break;
												}
												if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
												for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
													if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
												}
												if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
												sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
												SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
												SDMSKIPCOUNT++;
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , f , -1 ) ) return SYS_ERROR; // 2003.10.17
												//--------------------------------------------------------
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
												//--------------------------------------------------------
											}
										}
										else { // 2006.02.23
											for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
												if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
													if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
														if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
															//--------------------------------------------------------
															if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , f , -1 ) ) continue;
															//--------------------------------------------------------
															if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
															if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
															//--------------------------------------------------------
															break;
														}
													}
												}
											}
										}
										_i_SCH_IO_MTL_SAVE();
									}
									else {
//										return SYS_ABORTED; // 2006.03.27
										if ( tx == 0 ) return SYS_ABORTED; // 2006.03.27
									}
								}
							}
						}
					}
				}
				else {
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( tx,TA_STATION ) && ( WAFER_STATUS_IN_TM( tx , TA_STATION ) > 0 ) ) {
						if ( Finger != 1 ) {
							//=======================================
							lx++; // 2006.02.23
							//=======================================
							s = WAFER_STATUS_IN_TM( tx , TA_STATION );
							c = WAFER_SOURCE_IN_TM( tx , TA_STATION ) + CM1;
							if ( tx == 0 ) {
								if ( c < PM1 ) {
									//--------------------------------------------------------
									if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
									//--------------------------------------------------------
									switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
									case 5 :
									case 6 :
										if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
											return SYS_ERROR;
										}
										break;
									}
									//--------------------------------------------------------
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
									if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( c , s );
									//--------------------------------------------------------
								}
								else if ( c == PM1 ) {
									DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
									//--------------------------------------------------------
									if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
									//--------------------------------------------------------
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
									if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
									//--------------------------------------------------------
								}
								else if ( c > PM1 ) { // 2002.05.10
									for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
										if ( SDMSKIP[d] == s - 1 + PM1 ) break;
									}
									if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
									for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
										if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
									}
									if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
									sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
									SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
									SDMSKIPCOUNT++;
									//--------------------------------------------------------
									if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , TA_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
									//--------------------------------------------------------
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
									if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
									//--------------------------------------------------------
								}
							}
							else { // 2006.02.23
								for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , TA_STATION , -1 ) ) continue;
												//--------------------------------------------------------
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												break;
											}
										}
									}
								}
							}
							_i_SCH_IO_MTL_SAVE();
						}
					}
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( tx,TB_STATION ) && ( WAFER_STATUS_IN_TM( tx , TB_STATION ) > 0 ) ) {
						if ( Finger != 0 ) {
							//=======================================
							lx++; // 2006.02.23
							//=======================================
							s = WAFER_STATUS_IN_TM( tx , TB_STATION );
							c = WAFER_SOURCE_IN_TM( tx , TB_STATION ) + CM1;
							if ( tx == 0 ) {
								if ( c < PM1 ) {
									//--------------------------------------------------------
									if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , c , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
									//--------------------------------------------------------
									switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
									case 5 :
									case 6 :
										if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
											return SYS_ERROR;
										}
										break;
									}
									//--------------------------------------------------------
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
									if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( c , s );
									//--------------------------------------------------------
								}
								else if ( c == PM1 ) {
									DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
									//--------------------------------------------------------
									if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
									//--------------------------------------------------------
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
									if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
									//--------------------------------------------------------
								}
								else if ( c > PM1 ) { // 2002.05.10
									for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
										if ( SDMSKIP[d] == s - 1 + PM1 ) break;
									}
									if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
									for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
										if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
									}
									if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
									sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
									SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
									SDMSKIPCOUNT++;
									//--------------------------------------------------------
									if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , TB_STATION , -1 ) ) return SYS_ERROR; // 2003.10.17
									//--------------------------------------------------------
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
									if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
									//--------------------------------------------------------
								}
							}
							else { // 2006.02.23
								for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
									if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
										if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
												//--------------------------------------------------------
												if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , TB_STATION , -1 ) ) continue;
												//--------------------------------------------------------
												if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
												if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
												//--------------------------------------------------------
												break;
											}
										}
									}
								}
							}
							_i_SCH_IO_MTL_SAVE();
						}
					}
					for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
						if ( DMSKIP != i ) {
							sres = FALSE; // 2006.03.27
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , i , G_PICK ) ) { // 2006.02.23
								if ( tx == 0 ) {
									sres = TRUE;
								}
								else {
									if ( !_SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , i , G_PICK ) ) sres = TRUE;
								}
							}
							if ( sres ) { // 2006.03.27
								if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) != 2 ) && ( _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) ) {
									//=======================================
									lx++; // 2006.02.23
									//=======================================
									//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i ); // 2003.10.17
									//else               f = Finger; // 2003.10.17
									s = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
									c = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 ) + CM1;
									//--------------------------------------------------------
									if ( tx == 0 ) {
										if ( !Transfer_Enable_InterlockFinger_Check_for_Move( 0 , Finger , i , c , &f , ( c < PM1 ) ) ) return SYS_ERROR; // 2003.10.17
										//--------------------------------------------------------
										if ( c < PM1 ) {
											//--------------------------------------------------------
											switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( c ) ) { // 2004.01.30
											case 5 :
											case 6 :
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TA_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( WAFER_STATUS_IN_TM( 0 , TB_STATION ) > 0 ) ) {
													return SYS_ERROR;
												}
												break;
											}
											//--------------------------------------------------------
										}
									}
									else {
										if ( !Transfer_Enable_InterlockFinger_Check_for_Move( tx , Finger , i , -1 , &f , TRUE ) ) continue;
									}
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
										if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) return SYS_ABORTED;
									}
									if ( EQUIP_PICK_FROM_CHAMBER_TT( tx , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
									_i_SCH_IO_MTL_SAVE();
									if ( tx == 0 ) {
										if ( c < PM1 ) {
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( c ) ) ) return SYS_ABORTED;
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											//--------------------------------------------------------
											Transfer_Cass_Info_Operation_After_Place( c , s );
											//--------------------------------------------------------
										}
										else if ( c == PM1 ) {
											DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , DMSKIP , f , -1 ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) return SYS_ABORTED;
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , DMSKIP , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											//--------------------------------------------------------
											Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
											//--------------------------------------------------------
										}
										else if ( c > PM1 ) { // 2002.05.10
											for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
												if ( SDMSKIP[d] == s - 1 + PM1 ) break;
											}
											if ( d != SDMSKIPCOUNT ) return SYS_ABORTED;
											for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
												if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == s - 1 + PM1 ) break;
											}
											if ( d == MAX_SDUMMY_DEPTH ) return SYS_ABORTED;
											sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
											SDMSKIP[SDMSKIPCOUNT] = s - 1 + PM1;
											SDMSKIPCOUNT++;
											//--------------------------------------------------------
											if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , 0 , s - 1 + PM1 , f , -1 ) ) return SYS_ERROR; // 2003.10.17
											//--------------------------------------------------------
											if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( s - 1 + PM1 ) ) ) return SYS_ABORTED;
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , s - 1 + PM1 , f , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
											//--------------------------------------------------------
											Transfer_Cass_Info_Operation_After_Place( s - 1 + PM1 , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
											//--------------------------------------------------------
										}
									}
									else { // 2006.02.23
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx - 1 , j , G_PICK ) ) {
												if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tx , j , G_PLACE ) ) {
													if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( j , 1 ) <= 0 ) ) {
														//--------------------------------------------------------
														if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tx , j , f , -1 ) ) continue;
														//--------------------------------------------------------
														if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( j ) ) ) return SYS_ABORTED;
														if ( EQUIP_PLACE_TO_CHAMBER_TT( tx , TR_CHECKING_SIDE , j , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
														//--------------------------------------------------------
														break;
													}
												}
											}
										}
									}
									_i_SCH_IO_MTL_SAVE();
								}
							}
						}
					}
				}
			}
			//=====================================================================================
			// 2006.02.26
			//=====================================================================================
			if ( lx == 0 ) break;
			//=====================================================================================
		}
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( Transfer_Enable_Check( i + CM1 ) && _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i + CM1 ) ) {
				if ( EQUIP_CLEAR_CHAMBER( i + CM1 , 0 , 0 ) == SYS_ABORTED ) return SYS_ABORTED;
			}
		}
	}
	return SYS_SUCCESS;
}


BOOL Transfer_GET_MOVE_PLACE_SLOT_FOR_BM_AT_SWITCH( int Finger , int bms , int *bm1 , int *bm2 , BOOL fmmode ) {
	int j , k , sc , lc;
	if ( Finger == 2 ) {
		*bm1 = 0;
		*bm2 = 0;
	}
	//=======================================================================================================================
	// 2006.09.29
	//=======================================================================================================================
	if ( (  _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) && ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2006.12.21
		if ( fmmode ) {
			sc = _SCH_COMMON_TRANSFER_OPTION( 3 , bms );
			lc = _SCH_COMMON_TRANSFER_OPTION( 4 , bms );
		}
		else {
			sc = _SCH_COMMON_TRANSFER_OPTION( 5 , bms );
			lc = _SCH_COMMON_TRANSFER_OPTION( 6 , bms );
		}
	}
	else {
		sc = 1;
		lc = _i_SCH_PRM_GET_MODULE_SIZE( bms );
	}
	//=======================================================================================================================
	if ( fmmode ) {
		k = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bms ) );
	}
	else {
		k = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bms ) );
	}
	switch ( k ) {
	case 2 : // Last
		for ( j = lc - 1 ; j >= ( sc - 1 ) ; j-- ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( bms , j + 1 ) <= 0 ) {
				if      ( Finger == 0 ) {
					*bm1 = j + 1;
					*bm2 = 0;
	 				break; // 2005.06.07
				}
				else if ( Finger == 1 ) {
					*bm2 = j + 1;
					*bm1 = 0;
	 				break; // 2005.06.07
				}
				else if ( Finger == 2 ) {
					if ( *bm1 == 0 ) { // 2005.06.07
						*bm1 = j + 1; // 2005.06.07
					} // 2005.06.07
					else if ( *bm2 == 0 ) { // 2005.06.07
						*bm2 = j + 1; // 2005.06.07
						break; // 2005.06.07
					} // 2005.06.07
//					*bm1 = j + 1; // 2005.06.07
//					*bm2 = *bm1; // 2005.06.07
				}
// 				break; // 2005.06.07
			}
		}
		if ( j == ( ( sc - 1 ) - 1 ) ) return FALSE;
		break;
	default : // First
		for ( j = ( sc - 1 ) ; j < lc ; j++ ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( bms , j + 1 ) <= 0 ) {
				if      ( Finger == 0 ) {
					*bm1 = j + 1;
					*bm2 = 0;
	 				break; // 2005.06.07
				}
				else if ( Finger == 1 ) {
					*bm2 = j + 1;
					*bm1 = 0;
	 				break; // 2005.06.07
				}
				else if ( Finger == 2 ) {
					if ( *bm1 == 0 ) { // 2005.06.07
						*bm1 = j + 1; // 2005.06.07
					} // 2005.06.07
					else if ( *bm2 == 0 ) { // 2005.06.07
						*bm2 = j + 1; // 2005.06.07
						break; // 2005.06.07
					} // 2005.06.07
//					*bm1 = j + 1; // 2005.06.07
//					*bm2 = *bm1; // 2005.06.07
				}
//				break; // 2005.06.07
			}
		}
		if ( j == lc ) return FALSE;
		break;
	}
	return TRUE;
}

int Transfer_GET_SUPPLYCLEAR_SLOT_FOR_BM_AT_SWITCH_SUB( int BMS , int Slot , int Mode , BOOL clear ) { // 2004.08.16
	int i , sc , lc , res;
	if ( !Transfer_Enable_Check( BMS ) ) return -1;
	if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
		if ( Mode == 0 ) { // Order
			for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( BMS ) ; i++ ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( BMS , i + 1 ) <= 0 ) {
					return i + 1;
				}
			}
		}
		else { // Overlap
			if ( _i_SCH_IO_GET_MODULE_STATUS( BMS , Slot ) <= 0 ) {
				return Slot;
			}
		}
	}
	else if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) { // 2006.09.29
		if ( clear ) {
			sc = _SCH_COMMON_TRANSFER_OPTION( 5 , BMS );
			lc = _SCH_COMMON_TRANSFER_OPTION( 6 , BMS );
			//
			res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( BMS ) );
		}
		else {
			sc = _SCH_COMMON_TRANSFER_OPTION( 3 , BMS );
			lc = _SCH_COMMON_TRANSFER_OPTION( 4 , BMS );
			//
			res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( BMS ) );
		}
		switch ( res ) {
		case 2 : // Last
			for ( i = lc ; i >= sc ; i-- ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( BMS , i ) <= 0 ) {
					return i;
				}
			}
			break;
		default :
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( BMS , i ) <= 0 ) {
					return i;
				}
			}
			break;
		}
	}
	else {
//		switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( BMS ) ) { // 2004.05.06
//		case 1 : // Last // 2004.05.06
//		case 3 : // 2004.05.06
		if ( clear ) { // 2004.08.16
			res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( BMS ) ); // 2004.08.16
		} // 2004.08.16
		else { // 2004.08.16
			res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( BMS ) ); // 2004.08.16
		} // 2004.08.16
		switch ( res ) { // 2004.05.06 // 2004.08.16
//		switch ( _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( BMS ) ) ) { // 2004.05.06 // 2004.08.16
		case 2 : // Last
			for ( i = _i_SCH_PRM_GET_MODULE_SIZE( BMS ) - 1 ; i >= 0 ; i-- ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( BMS , i + 1 ) <= 0 ) {
					return i + 1;
				}
			}
			break;
		default :
			for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( BMS ) ; i++ ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( BMS , i + 1 ) <= 0 ) {
					return i + 1;
				}
			}
			break;
		}
	}
	return -1;
}

int Transfer_GET_SUPPLYCLEAR_BM_AT_SWITCH_SUB( int Src , BOOL clear ) {
	int i , k , x , res , sc , lc;
	if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) {
		//
		x = _SCH_COMMON_TRANSFER_OPTION( 8 , Src );
		//
		if ( x < BM1 ) {
			x = _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( Src );
			if ( x == 0 ) {
				if ( Transfer_Enable_Check( Src + BM1 ) ) { // 2004.03.11
					return Src + BM1;
				}
				for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) { // 2004.03.11
					if ( Transfer_Enable_Check( k ) ) { // 2004.03.11
						return k; // 2004.03.11
					} // 2004.03.11
				} // 2004.03.11
				return -1;
			}
			x = x - 1;
//			if ( x >= MAX_BM_CHAMBER_DEPTH ) x = 0; // 2003.11.28
			if ( x >= MAX_BM_CHAMBER_DEPTH ) x = x - MAX_BM_CHAMBER_DEPTH; // 2003.11.28
			return x + BM1;
		}
	}
	else if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
		//=================================================================================================
		// 2006.10.11
		//=================================================================================================
		x = _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( Src );
		if ( x != 0 ) {
			k = BM1 + x - 1;
			if ( Transfer_Enable_Check( k ) ) {
				if ( clear ) {
					sc = _SCH_COMMON_TRANSFER_OPTION( 5 , k );
					lc = _SCH_COMMON_TRANSFER_OPTION( 6 , k );
					//
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) );
				}
				else {
					sc = _SCH_COMMON_TRANSFER_OPTION( 3 , k );
					lc = _SCH_COMMON_TRANSFER_OPTION( 4 , k );
					//
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) );
				}
				switch ( res ) {
				case 2 : // Last
					for ( i = lc ; i >= sc ; i-- ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( k , i ) <= 0 ) {
							return k;
						}
					}
					break;
				default :
					for ( i = sc ; i <= lc ; i++ ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( k , i ) <= 0 ) {
							return k;
						}
					}
					break;
				}
			}
			return -1;
		}
		//=================================================================================================
		x = -1;
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( Transfer_Enable_Check( k ) ) {
				if ( clear ) {
					sc = _SCH_COMMON_TRANSFER_OPTION( 5 , k );
					lc = _SCH_COMMON_TRANSFER_OPTION( 6 , k );
					//
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ); // 2004.08.16
				}
				else {
					sc = _SCH_COMMON_TRANSFER_OPTION( 3 , k );
					lc = _SCH_COMMON_TRANSFER_OPTION( 4 , k );
					//
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ); // 2004.08.16
				}
				switch ( res ) {
				case 2 : // Last
					if ( _i_SCH_IO_GET_MODULE_STATUS( k , lc ) <= 0 ) {
						return k;
					}
					break;
				default :
					if ( _i_SCH_IO_GET_MODULE_STATUS( k , sc ) <= 0 ) {
						return k;
					}
					break;
				}
			}
		}
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( Transfer_Enable_Check( k ) ) {
				if ( clear ) {
					sc = _SCH_COMMON_TRANSFER_OPTION( 5 , k );
					lc = _SCH_COMMON_TRANSFER_OPTION( 6 , k );
					//
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) );
				}
				else {
					sc = _SCH_COMMON_TRANSFER_OPTION( 3 , k );
					lc = _SCH_COMMON_TRANSFER_OPTION( 4 , k );
					//
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) );
				}
				switch ( res ) {
				case 2 : // Last
					for ( i = lc ; i >= sc ; i-- ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( k , i ) <= 0 ) {
							return k;
						}
					}
					break;
				default :
					for ( i = sc ; i <= lc ; i++ ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( k , i ) <= 0 ) {
							return k;
						}
					}
					break;
				}
			}
		}
		//=================================================================================================
	}
	else {
		//=================================================================================================
		// 2005.10.11
		//=================================================================================================
		x = _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER( Src );
		if ( x != 0 ) {
			k = BM1 + x - 1;
			if ( Transfer_Enable_Check( k ) ) {
				if ( clear ) {
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) );
				}
				else {
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) );
				}
				switch ( res ) {
				case 2 : // Last
					for ( i = _i_SCH_PRM_GET_MODULE_SIZE( k ) - 1 ; i >= 0 ; i-- ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( k , i + 1 ) <= 0 ) {
							return k;
						}
					}
					break;
				default :
					for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( k ) ; i++ ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( k , i + 1 ) <= 0 ) {
							return k;
						}
					}
					break;
				}
			}
			return -1;
		}
		//=================================================================================================
		x = -1;
		for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
			if ( Transfer_Enable_Check( k ) ) {
//				switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( k ) ) { // 2004.05.06
//				case 1 : // Last // 2004.05.06
//				case 3 : // 2004.05.06
				if ( clear ) { // 2004.08.16
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ); // 2004.08.16
				} // 2004.08.16
				else { // 2004.08.16
					res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ); // 2004.08.16
				} // 2004.08.16
				switch ( res ) { // 2004.05.06 // 2004.08.16
//				switch ( _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ) { // 2004.05.06 // 2004.08.16
				case 2 : // Last
					if ( _i_SCH_IO_GET_MODULE_STATUS( k , _i_SCH_PRM_GET_MODULE_SIZE( k ) ) <= 0 ) {
						return k;
					}
					break;
				default :
					if ( _i_SCH_IO_GET_MODULE_STATUS( k , 1 ) <= 0 ) {
						return k;
					}
					break;
				}
			}
		}
		if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) { // 2005.10.11
			return -1; // 2005.10.11
		}
		else {
			for ( k = BM1 ; k < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; k++ ) {
				if ( Transfer_Enable_Check( k ) ) {
	//				switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( k ) ) { // 2004.05.06
	//				case 1 : // Last // 2004.05.06
	//				case 3 : // 2004.05.06
					if ( clear ) { // 2004.08.16
						res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ); // 2004.08.16
					} // 2004.08.16
					else { // 2004.08.16
						res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_FM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ); // 2004.08.16
					} // 2004.08.16
					switch ( res ) { // 2004.05.06 // 2004.08.16
	//				switch ( _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( k ) ) ) { // 2004.05.06 // 2004.08.16
					case 2 : // Last
						for ( i = _i_SCH_PRM_GET_MODULE_SIZE( k ) - 1 ; i >= 0 ; i-- ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( k , i + 1 ) <= 0 ) {
								return k;
							}
						}
						break;
					default :
						for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( k ) ; i++ ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( k , i + 1 ) <= 0 ) {
								return k;
							}
						}
						break;
					}
				}
			}
		}
	}
	return x;
}


int Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( int BMS , int Slot , int Mode ) { // 2004.08.16
	return Transfer_GET_SUPPLYCLEAR_SLOT_FOR_BM_AT_SWITCH_SUB( BMS , Slot , Mode , TRUE );
}

int Transfer_GET_SUPPLY_SLOT_FOR_BM_AT_SWITCH( int BMS , int Slot , int Mode ) { // 2004.08.16
	return Transfer_GET_SUPPLYCLEAR_SLOT_FOR_BM_AT_SWITCH_SUB( BMS , Slot , Mode , FALSE );
}

int Transfer_GET_CLEAR_BM_AT_SWITCH( int Src ) { // 2004.08.16
	return Transfer_GET_SUPPLYCLEAR_BM_AT_SWITCH_SUB( Src , TRUE );
}

int Transfer_GET_SUPPLY_BM_AT_SWITCH( int Src ) { // 2004.08.16
	return Transfer_GET_SUPPLYCLEAR_BM_AT_SWITCH_SUB( Src , FALSE );
}

int Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AT_FIRST( int bmc ) { // 2005.10.11
	int i , res , sc , lc;
	//========================================================================
	// 2006.10.11
	//========================================================================
	if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
		sc = _SCH_COMMON_TRANSFER_OPTION( 5 , bmc );
		lc = _SCH_COMMON_TRANSFER_OPTION( 6 , bmc );
	}
	else {
		sc = 1;
		lc = _i_SCH_PRM_GET_MODULE_SIZE( bmc );
	}
	//========================================================================
	if ( Transfer_Enable_Check( bmc ) ) {
		res = _SCH_COMMON_TRANSFER_OPTION( 7 , ( CHECKORDER_FOR_TM_PLACE * 100 ) + _i_SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) );
		switch ( res ) {
		case 2 :
			if ( _i_SCH_IO_GET_MODULE_STATUS( bmc , lc ) <= 0 ) {
				return TRUE;
			}
			break;
		default :
			if ( _i_SCH_IO_GET_MODULE_STATUS( bmc , sc ) <= 0 ) {
				return TRUE;
			}
			break;
		}
	}
	//------------------------------------------------------------------------
	if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() != BUFFER_SWITCH_SINGLESWAP ) {
		for ( i = lc ; i >= sc ; i-- ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( bmc , i ) <= 0 ) {
				return TRUE;
			}
		}
	}
	//------------------------------------------------------------------------
	return FALSE;
}

int Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( int bmc ) { // 2005.10.11
	int i , sc , lc;
	if ( Transfer_Enable_Check( bmc ) ) {
		if      ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 1 ) return 1;
		else if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 2 ) {
			if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) return 2;
			//==============================================================================
			// 2006.10.11
			//==============================================================================
			if ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
				sc = _SCH_COMMON_TRANSFER_OPTION( 5 , bmc );
				lc = _SCH_COMMON_TRANSFER_OPTION( 6 , bmc );
				//
				for ( i = lc ; i >= sc ; i-- ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( bmc , i ) <= 0 ) {
						return 0;
					}
				}
			}
			//==============================================================================
			else {
				for ( i = _i_SCH_PRM_GET_MODULE_SIZE( bmc ) - 1 ; i >= 0 ; i-- ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( bmc , i + 1 ) <= 0 ) {
						return 0;
					}
				}
			}
			return 2;
		}
	}
	return 0;
}

BOOL Transfer_GET_FREE_BM_CHECK( BOOL tmdouble , int Src , int *slot , int mode ) {
	int i , sc , lc;
	*slot = -1;
	if ( tmdouble ) { // tmdouble
		for ( i = 0 ; i < _i_SCH_PRM_GET_MODULE_SIZE( Src ) ; i++ ) {
			if ( _i_SCH_IO_GET_MODULE_STATUS( Src , i + 1 ) > 0 ) return FALSE;
			if ( *slot < 0 ) *slot = i + 1;
		}
		return TRUE;
	}
	else {
		//====================================================================================================================
		// 2006.10.11
		//====================================================================================================================
		if ( ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) && ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2006.12.21
			sc = _SCH_COMMON_TRANSFER_OPTION( 5 , Src );
			lc = _SCH_COMMON_TRANSFER_OPTION( 6 , Src );
		}
		else {
			sc = 1;
			lc = _i_SCH_PRM_GET_MODULE_SIZE( Src );
		}
		//====================================================================================================================
		if ( mode == 0 ) { // Random
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( Src , i ) <= 0 ) {
					*slot = i;
					return TRUE;
				}
			}
		}
		else if ( mode == 1 ) { // First
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( Src , i ) <= 0 ) {
					*slot = i;
					return TRUE;
				}
			}
		}
		else if ( mode == 2 ) { // Last
			for ( i = lc ; i >= sc ; i-- ) {
				if ( _i_SCH_IO_GET_MODULE_STATUS( Src , i ) <= 0 ) {
					*slot = i;
					return TRUE;
				}
			}
		}
		//====================================================================================================================
	}
	return FALSE;
}
//
BOOL Transfer_GET_USE_BM_CHECK( int Src ) {
	int i , sc , lc;
	//====================================================================================================================
	// 2006.10.11
	//====================================================================================================================
	if ( ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) && ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2006.12.21
		sc = _SCH_COMMON_TRANSFER_OPTION( 5 , Src );
		lc = _SCH_COMMON_TRANSFER_OPTION( 6 , Src );
	}
	else {
		sc = 1;
		lc = _i_SCH_PRM_GET_MODULE_SIZE( Src );
	}
	//====================================================================================================================
	for ( i = sc ; i <= lc ; i++ ) {
		if ( _i_SCH_IO_GET_MODULE_STATUS( Src , i ) > 0 ) return TRUE;
	}
	return FALSE;
}
//
BOOL Transfer_GET_GO_BM_POSSIBLE_CHECK( int Src ) {
	int i , sc , lc;
	if ( Src <= 0 ) return FALSE;
	//====================================================================================================================
	// 2006.10.11
	//====================================================================================================================
	if ( ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) && ( _i_SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) { // 2006.12.21
		sc = _SCH_COMMON_TRANSFER_OPTION( 5 , Src );
		lc = _SCH_COMMON_TRANSFER_OPTION( 6 , Src );
	}
	else {
		sc = 1;
		lc = _i_SCH_PRM_GET_MODULE_SIZE( Src );
	}
	//====================================================================================================================
	for ( i = sc ; i <= lc ; i++ ) {
		if ( _i_SCH_IO_GET_MODULE_STATUS( Src , i ) <= 0 ) return TRUE;
	}
	return FALSE;
}
//
int Transfer_GET_Slot_for_TM_Action( int slot1 , int slot2 ) {
	if ( ( slot1 > 0 ) && ( slot2 > 0 ) ) {
		return slot1 + ( slot2 * 100 );
	}
	else if ( slot1 > 0 ) {
		return slot1;
	}
	else if ( slot2 > 0 ) {
		if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) return slot2 * 100; // 2004.02.10
		return slot2;
	}
	return 0;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Transfer Part for FEM Type
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int Transfer_User_Running_FEM_Code( int Mode , int Use , int Finger , int Src_Station , int Src_Slot , int Src_Slot2 , int Trg_Station , int Trg_Slot , int Trg_Slot2 , int *BM_Station , int *BM_Slot , int *BM_Slot2 , int *BM_PSlot , int *BM_PSlot2 , int st , int et , int DoArmMX , int *rcode ) {
	int i , j , f , b , x , s , c , p , FM_CM , Finger2 , grp , d , sl , a;
	int DMSKIP , DMYES;
	int SDMSKIPCOUNT , SDMSKIP[MAX_SDUMMY_DEPTH];
	BOOL Find;
	int CLSYES[MAX_TM_CHAMBER_DEPTH] , CLSFIND , BM_Place_Count;
	int BM_ALL_RUN , BM_RUN[MAX_CHAMBER] , BM_WAIT[MAX_CHAMBER];
	int	xW_SOURCE_IN_TM[MAX_TM_CHAMBER_DEPTH][ 2 ];
	int	xW_STATUS_IN_TM[MAX_TM_CHAMBER_DEPTH][ 2 ];
	int	xW_SOURCE_IN_TM2[MAX_TM_CHAMBER_DEPTH][ 2 ];
	int	xW_STATUS_IN_TM2[MAX_TM_CHAMBER_DEPTH][ 2 ];
	int	xW_SOURCE_IN_CHAMBER[ MAX_CHAMBER ];
	int xW_STATUS_IN_CHAMBER[ MAX_CHAMBER ][ 6 ];
	int stmatm , etmatm;
	int clssw;

	if ( Mode == TR_PICK ) {
		if ( Src_Station < PM1 ) {
			if ( Finger == 0 ) {
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				//--------------------------------------------------------
				if ( DoArmMX > 0 ) { // 2003.10.20
					for ( a = 0 ; a < DoArmMX ; a++ ) {
						Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot + a + 1 );
					}
				}
				//--------------------------------------------------------
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , 0 , TRUE , Src_Station , 0 ) == SYS_ABORTED ) {
					*rcode = 101;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , 0 , Src_Station , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 102;
					return SYS_ABORTED;
				}
				//--------------------------------------------------------
			}
			else if ( Finger == 1 ) {
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				//--------------------------------------------------------
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot , TRUE , Src_Station , 0 ) == SYS_ABORTED ) {
					*rcode = 103;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , Src_Slot , Src_Station , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 104;
					return SYS_ABORTED;
				}
				//--------------------------------------------------------
			}
			else {
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot2 );
				//--------------------------------------------------------
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , Src_Station , 0 ) == SYS_ABORTED ) {
					*rcode = 105;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , Src_Slot2 , Src_Station , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 106;
					return SYS_ABORTED;
				}
				//--------------------------------------------------------
			}
			_i_SCH_IO_MTL_SAVE();
		}
		else if ( Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , 1 ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 107;
					return SYS_ABORTED;
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , 1 ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 108;
					return SYS_ABORTED;
				}
			}
			else {
				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , 1 ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 109;
					return SYS_ABORTED;
				}
			}
			_i_SCH_IO_MTL_SAVE();
		}
		else if ( Src_Station < TM ) {
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( Use == 0 ) {
					// For Go FEM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 110;
						return SYS_ABORTED;
					}
				}
				else {
					// For Go TM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 111;
						return SYS_ABORTED;
					}
				}
				if ( Src_Slot2 > 0 ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot2 ) + CM1;
				if ( Src_Slot > 0  ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
				//-------------------------------------------------------------------------------
				if ( Use == 0 ) {
					if ( Finger == 0 ) {
						if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
							*rcode = 112;
							return SYS_ABORTED;
						}
					}
					else if ( Finger == 1 ) {
						if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
							*rcode = 113;
							return SYS_ABORTED;
						}
					}
					else {
						if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
							*rcode = 114;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( Src_Station - BM1 , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 115;
						return SYS_ABORTED;
					}
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( Use == 0 ) {
					// For Go FEM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 116;
						return SYS_ABORTED;
					}
				}
				else {
					// For Go TM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 117;
						return SYS_ABORTED;
					}
				}
				if ( Src_Slot2 > 0 ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot2 ) + CM1;
				if ( Src_Slot > 0  ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
				//-------------------------------------------------------------------------------
				if ( Use == 0 ) {
					if ( Finger == 0 ) {
						if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
							*rcode = 118;
							return SYS_ABORTED;
						}
					}
					else if ( Finger == 1 ) {
						if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
							*rcode = 119;
							return SYS_ABORTED;
						}
					}
					else {
						if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
							*rcode = 120;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( Src_Station - BM1 , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 121;
						return SYS_ABORTED;
					}
				}
			}
			else {
				//-------- Switch Need 2001.07.05
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Src_Station ) ) {
					if ( Use == 0 ) {
						// For Go FEM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 122;
							return SYS_ABORTED;
						}
					}
					else {
						// For Go TM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 123;
							return SYS_ABORTED;
						}
					}
				}
				//-------------------------------------------------------------------------------
				if ( Src_Slot2 > 0 ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot2 ) + CM1;
				if ( Src_Slot > 0  ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
				if ( Use == 0 ) {
					if ( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) <= 0 ) {
						if ( Finger == 0 ) {
							if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
								*rcode = 124;
								return SYS_ABORTED;
							}
						}
						else if ( Finger == 1 ) {
							if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
								*rcode = 125;
								return SYS_ABORTED;
							}
						}
						else {
							if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
								*rcode = 126;
								return SYS_ABORTED;
							}
						}
					}
					else {
						if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) - 1 , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 127;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 128;
						return SYS_ABORTED;
					}
				}
			}
			_i_SCH_IO_MTL_SAVE();
		}
	}
	if ( Mode == TR_PLACE ) {
		if ( Trg_Station < PM1 ) {
			if ( Finger == 0 ) {
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , Trg_Station , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 201;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
					*rcode = 202;
					return SYS_ABORTED;
				}
				//--------------------------------------------------------
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
				if ( DoArmMX > 0 ) { // 2003.10.20
					for ( a = 0 ; a < DoArmMX ; a++ ) {
						Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot + a + 1 );
					}
				}
				//--------------------------------------------------------
			}
			else if ( Finger == 1 ) {
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , Trg_Station , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 203;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
					*rcode = 204;
					return SYS_ABORTED;
				}
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
				//--------------------------------------------------------
			}
			else {
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , Trg_Station , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 205;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
					*rcode = 206;
					return SYS_ABORTED;
				}
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
				//--------------------------------------------------------
			}
			_i_SCH_IO_MTL_SAVE();
		}
		else if ( Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 207;
					return SYS_ABORTED;
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 208;
					return SYS_ABORTED;
				}
			}
			else {
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 209;
					return SYS_ABORTED;
				}
			}
			_i_SCH_IO_MTL_SAVE();
		}
		else if ( Trg_Station < TM ) {
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( Use == 0 ) {
					// For Go FEM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 210;
						return SYS_ABORTED;
					}
				}
				else {
					// For Go TM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 211;
						return SYS_ABORTED;
					}
				}
				//-------------------------------------------------------------------------------
				if ( Use == 0 ) {
					if ( Finger == 0 ) {
						if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , WAFER_SOURCE_IN_FM( 0 , 0 ) + CM1 , 0 ) == SYS_ABORTED ) {
							*rcode = 212;
							return SYS_ABORTED;
						}
					}
					else if ( Finger == 1 ) {
						if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , WAFER_SOURCE_IN_FM( 0 , 1 ) + CM1, 0  ) == SYS_ABORTED ) {
							*rcode = 213;
							return SYS_ABORTED;
						}
					}
					else {
						if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , WAFER_SOURCE_IN_FM( 0 , 0 ) + CM1 , 0 ) == SYS_ABORTED ) {
							*rcode = 214;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( Trg_Station - BM1 , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 215;
						return SYS_ABORTED;
					}
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( Use == 0 ) {
					// For Go FEM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 216;
						return SYS_ABORTED;
					}
				}
				else {
					// For Go TM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 217;
						return SYS_ABORTED;
					}
				}
				//-------------------------------------------------------------------------------
				if ( Use == 0 ) {
					if ( Finger == 0 ) {
						if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , WAFER_SOURCE_IN_FM( 0 , 0 ) + CM1 , 0 ) == SYS_ABORTED ) {
							*rcode = 218;
							return SYS_ABORTED;
						}
					}
					else if ( Finger == 1 ) {
						if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , WAFER_SOURCE_IN_FM( 0 , 1 ) + CM1 , 0 ) == SYS_ABORTED ) {
							*rcode = 219;
							return SYS_ABORTED;
						}
					}
					else {
						if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , WAFER_SOURCE_IN_FM( 0 , 0 ) + CM1 , 0 ) == SYS_ABORTED ) {
							*rcode = 220;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( Trg_Station - BM1 , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 221;
						return SYS_ABORTED;
					}
				}
			}
			else {
				//-------- Switch Need 2001.07.05
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
					if ( Use == 0 ) {
						// For Go FEM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 222;
							return SYS_ABORTED;
						}
					}
					else {
						// For Go TM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 223;
							return SYS_ABORTED;
						}
					}
				}
				//-------------------------------------------------------------------------------
				if ( Use == 0 ) {
					if ( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) <= 0 ) {
						if ( Finger == 0 ) {
							if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , WAFER_SOURCE_IN_FM( 0 , 0 ) + CM1 , 0 ) == SYS_ABORTED ) {
								*rcode = 224;
								return SYS_ABORTED;
							}
							//--------------------------------------------------------
						}
						else if ( Finger == 1 ) {
							if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , WAFER_SOURCE_IN_FM( 0 , 1 ) + CM1 , 0 ) == SYS_ABORTED ) {
								*rcode = 225;
								return SYS_ABORTED;
							}
						}
						else {
							if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , WAFER_SOURCE_IN_FM( 0 , 0 ) + CM1 , 0 ) == SYS_ABORTED ) {
								*rcode = 226;
								return SYS_ABORTED;
							}
						}
					}
					else {
						if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) - 1 , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) - 1 , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 227;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , Finger ) + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 228;
						return SYS_ABORTED;
					}
				}
			}
			_i_SCH_IO_MTL_SAVE();
		}
	}
	if ( Mode == TR_MOVE ) {
		if ( Use == 0 ) {	//c-f-c	//c-f-b	//b-f-c
			//-------- Switch Need 2001.07.05
			if ( Src_Station >= BM1 ) {
				if (
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 )
					) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() != Src_Station ) { // 2003.09.03
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 301;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Src_Station ) ) {
						// For Go FEM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 302;
							return SYS_ABORTED;
						}
					}
				}
			}
			//-------------------------------------------------------------------------------
			if ( Finger == 0 ) {
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				if ( DoArmMX > 0 ) { // 2003.10.20
					for ( a = 0 ; a < DoArmMX ; a++ ) {
						Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot + a + 1 );
					}
				}
				//--------------------------------------------------------
				if ( Src_Station < PM1 ) {
					FM_CM = Src_Station;
				}
				else {
					FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
				}
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
					*rcode = 303;
					return SYS_ABORTED;
				}
				if ( Src_Station < PM1 ) {
					_i_SCH_IO_MTL_SAVE();
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , 0 , FM_CM , Trg_Station , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 304;
						return SYS_ABORTED;
					}
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == Src_Station ) { // 2003.09.03
						_i_SCH_IO_MTL_SAVE();
						if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , 0 , FM_CM , Trg_Station , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
							*rcode = 305;
							return SYS_ABORTED;
						}
					}
				}
				//--------------------------------------------------------
			}
			else if ( Finger == 1 ) {
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				//--------------------------------------------------------
				if ( Src_Station < PM1 ) {
					FM_CM = Src_Station;
				}
				else {
					FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
				}
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
					*rcode = 306;
					return SYS_ABORTED;
				}
				if ( Src_Station < PM1 ) {
					_i_SCH_IO_MTL_SAVE();
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , Src_Slot , FM_CM , Trg_Station , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 307;
						return SYS_ABORTED;
					}
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == Src_Station ) { // 2003.09.03
						_i_SCH_IO_MTL_SAVE();
						if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , Src_Slot , FM_CM , Trg_Station , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
							*rcode = 308;
							return SYS_ABORTED;
						}
					}
				}
				//--------------------------------------------------------
			}
			else {
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot2 );
				//--------------------------------------------------------
				if ( Src_Station < PM1 ) {
					FM_CM = Src_Station;
				}
				else {
					if ( Src_Slot2 > 0 ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot2 ) + CM1;
					if ( Src_Slot > 0 ) FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
				}
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
					*rcode = 309;
					return SYS_ABORTED;
				}
				if ( Src_Station < PM1 ) {
					_i_SCH_IO_MTL_SAVE();
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , Src_Slot2 , FM_CM , Trg_Station , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 310;
						return SYS_ABORTED;
					}
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == Src_Station ) { // 2003.09.03
						_i_SCH_IO_MTL_SAVE();
						if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , Src_Slot2 , FM_CM , Trg_Station , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
							*rcode = 311;
							return SYS_ABORTED;
						}
					}
				}
				//--------------------------------------------------------
			}
			_i_SCH_IO_MTL_SAVE();
			//-------- Switch Need 2001.07.05
			if ( Trg_Station >= BM1 ) {
				if (
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 )
					) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() != Trg_Station ) { // 2003.09.03
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 312;
							return SYS_ABORTED;
						}
					}
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
						// For Go FEM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 313;
							return SYS_ABORTED;
						}
					}
				}
			}
			//-------------------------------------------------------------------------------
			if ( Finger == 0 ) {
				if ( Trg_Station < PM1 ) {
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 314;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 315;
						return SYS_ABORTED;
					}
				}
				else {
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 316;
						return SYS_ABORTED;
					}
				}
				//--------------------------------------------------------
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
				if ( DoArmMX > 0 ) { // 2003.10.20
					for ( a = 0 ; a < DoArmMX ; a++ ) {
						Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot + a + 1 );
					}
				}
				//--------------------------------------------------------
			}
			else if ( Finger == 1 ) {
				if ( Trg_Station < PM1 ) {
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 317;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 318;
						return SYS_ABORTED;
					}
				}
				else {
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 319;
						return SYS_ABORTED;
					}
				}
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
			}
			else {
				if ( Trg_Station < PM1 ) {
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 320;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 321;
						return SYS_ABORTED;
					}
				}
				else {
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 322;
						return SYS_ABORTED;
					}
				}
				//--------------------------------------------------------
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
				Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
				//--------------------------------------------------------
			}
			_i_SCH_IO_MTL_SAVE();
		}
		else if ( Use == 1 ) {	//p-t-p	//p-t-b	//b-t-p	//b-t-b
			if ( Src_Station < PM1 ) {
				FM_CM = Src_Station;
			}
			else if ( Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
				FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , 1 ) + CM1;
			}
			else {
				FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , Src_Slot ) + CM1;
			}
			//
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( Src_Station >= BM1 ) {
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 323;
						return SYS_ABORTED;
					}
				}
				else if ( Src_Station >= PM1 ) { // 2003.09.03
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) + BM1 , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 324;
						return SYS_ABORTED;
					}
				}
				if ( Src_Station < PM1 ) {
				}
				else if ( Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 325;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 326;
						return SYS_ABORTED;
					}
				}
				//-------------------------------------------------------------------------------
				if ( Trg_Station < PM1 ) {
				}
				else if ( Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 327;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 328;
						return SYS_ABORTED;
					}
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( Src_Station < PM1 ) {
				}
				else if ( Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) { // 2003.04.21
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , ( ( Src_Station - PM1 ) / 2 ) + BM1 , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 329;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 330;
						return SYS_ABORTED;
					}
				}
				//
				if ( Src_Station < PM1 ) {
				}
				else if ( Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 331;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 332;
						return SYS_ABORTED;
					}
				}
				//-------------------------------------------------------------------------------
				if ( Trg_Station < PM1 ) {
				}
				else if ( Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 333;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 334;
						return SYS_ABORTED;
					}
				}
			}
			else {
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Src_Station );
				etmatm = _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station );
				//--------
				if ( Src_Station >= BM1 ) {
					if ( stmatm == etmatm ) {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Src_Station ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 335;
								return SYS_ABORTED;
							}
						}
					}
					else if ( stmatm > etmatm ) {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( stmatm , Src_Station ) != BUFFER_IN_S_MODE && _i_SCH_PRM_GET_MODULE_BUFFER_MODE( stmatm , Src_Station ) != BUFFER_OUT_S_MODE ) {
							stmatm--;
						}
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Src_Station ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 336;
								return SYS_ABORTED;
							}
						}
					}
					else if ( stmatm < etmatm ) {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Src_Station ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Src_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 337;
								return SYS_ABORTED;
							}
						}
					}
				}
				//-------------------------------------------------------------------------------
				if ( Src_Station < PM1 ) {
				}
				else if ( Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( stmatm , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 338;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_PICK_FROM_CHAMBER_TT( stmatm , TR_CHECKING_SIDE , Src_Station , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 339;
						return SYS_ABORTED;
					}
				}
				_i_SCH_IO_MTL_SAVE();
				//-------------------------------------------------------------------------------
				if ( stmatm > etmatm ) {
					for ( s = stmatm ; s > etmatm ; s-- ) {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s] ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 340;
								return SYS_ABORTED;
							}
						}
						if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s] , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , BM_Slot[s] , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 341;
							return SYS_ABORTED;
						}
						_i_SCH_IO_MTL_SAVE();
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s] ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 342;
								return SYS_ABORTED;
							}
						}
						if ( EQUIP_PICK_FROM_CHAMBER_TT( s - 1 , TR_CHECKING_SIDE , BM_Station[s] , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , BM_Slot[s] , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 343;
							return SYS_ABORTED;
						}
						_i_SCH_IO_MTL_SAVE();
					}
				}
				else if ( stmatm < etmatm ) {
					for ( s = stmatm ; s < etmatm ; s++ ) {
						if ( ( Trg_Station >= BM1 ) && ( ( s + 1 ) == etmatm ) ) {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( etmatm , Trg_Station ) != BUFFER_IN_S_MODE && _i_SCH_PRM_GET_MODULE_BUFFER_MODE( etmatm , Trg_Station ) != BUFFER_OUT_S_MODE ) {
							}
							else {
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s+1] ) ) {
									if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s+1] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
										*rcode = 344;
										return SYS_ABORTED;
									}
								}
								if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s+1] , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , BM_Slot[s+1] , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 345;
									return SYS_ABORTED;
								}
								_i_SCH_IO_MTL_SAVE();
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s+1] ) ) {
									if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s+1] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
										*rcode = 346;
										return SYS_ABORTED;
									}
								}
								if ( EQUIP_PICK_FROM_CHAMBER_TT( s + 1 , TR_CHECKING_SIDE , BM_Station[s+1] , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , BM_Slot[s+1] , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 347;
									return SYS_ABORTED;
								}
								_i_SCH_IO_MTL_SAVE();
							}
						}
						else {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s+1] ) ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s+1] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 348;
									return SYS_ABORTED;
								}
							}
							if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s+1] , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , BM_Slot[s+1] , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 349;
								return SYS_ABORTED;
							}
							_i_SCH_IO_MTL_SAVE();
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s+1] ) ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s+1] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 350;
									return SYS_ABORTED;
								}
							}
							if ( EQUIP_PICK_FROM_CHAMBER_TT( s + 1 , TR_CHECKING_SIDE , BM_Station[s+1] , Finger , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , BM_Slot[s+1] , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 351;
								return SYS_ABORTED;
							}
							_i_SCH_IO_MTL_SAVE();
						}
					}
				}

				//-------------------------------------------------------------------------------
				if ( Trg_Station >= BM1 ) {
					if ( stmatm >= etmatm ) {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 352;
								return SYS_ABORTED;
							}
						}
					}
					else if ( stmatm < etmatm ) {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( etmatm , Trg_Station ) != BUFFER_IN_S_MODE && _i_SCH_PRM_GET_MODULE_BUFFER_MODE( etmatm , Trg_Station ) != BUFFER_OUT_S_MODE ) {
							etmatm--;
						}
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 353;
								return SYS_ABORTED;
							}
						}
					}
				}
				//-------------------------------------------------------------------------------
				if ( Trg_Station < PM1 ) {
				}
				else if ( Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( etmatm , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 354;
						return SYS_ABORTED;
					}
				}
				else {
					if ( EQUIP_PLACE_TO_CHAMBER_TT( etmatm , TR_CHECKING_SIDE , Trg_Station , Finger , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 355;
						return SYS_ABORTED;
					}
				}
			}
			_i_SCH_IO_MTL_SAVE();
		}
		else if ( Use == 2 ) {	//c-f-b-t-p
			FM_CM = Src_Station;
			//
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ); // 2005.09.10

				Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );

				if      ( Finger == 0 ) {
					Src_Slot2 = 0;
				}
				else if ( Finger == 1 ) {
					Src_Slot2 = Src_Slot;
					Src_Slot = 0;
				}
				else if ( Finger == 2 ) {
					Src_Slot2 = 0;
					Finger = 0;
				}
				else {
					*rcode = 356;
					return SYS_ABORTED;
				}

				BM_Station[0] = _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) + BM1;
				//
				switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE(Trg_Station) ) {
				case 2 : // Last
				case 3 :
					for ( j = _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) - 1 ; j >= 0 ; j-- ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[0] , j + 1 ) <= 0 ) {
							if      ( Finger == 0 ) {
								BM_Slot[0] = j + 1;
								BM_Slot2[0] = 0;
							}
							else if ( Finger == 1 ) {
								BM_Slot2[0] = j + 1;
								BM_Slot[0] = 0;
							}
							break;
						}
					}
					if ( j == -1 ) {
						*rcode = 357;
						return SYS_ABORTED;
					}
					break;
				default : // First
					for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ; j++ ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[0] , j + 1 ) <= 0 ) {
							if      ( Finger == 0 ) {
								BM_Slot[0] = j + 1;
								BM_Slot2[0] = 0;
							}
							else if ( Finger == 1 ) {
								BM_Slot2[0] = j + 1;
								BM_Slot[0] = 0;
							}
							break;
						}
					}
					if ( j == _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ) {
						*rcode = 358;
						return SYS_ABORTED;
					}
					break;
				}

				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 359;
					return SYS_ABORTED;
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ); // 2005.09.10

				Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );

				if      ( Finger == 0 ) {
					Src_Slot2 = 0;
				}
				else if ( Finger == 1 ) {
					Src_Slot2 = Src_Slot;
					Src_Slot = 0;
				}
				else if ( Finger == 2 ) {
					Src_Slot2 = 0;
					Finger = 0;
				}
				else {
					*rcode = 360;
					return SYS_ABORTED;
				}

				BM_Station[0] = BM1 + _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station );

				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[0] , j + 1 ) <= 0 ) {
						if      ( Finger == 0 ) {
							BM_Slot[0] = j + 1;
							BM_Slot2[0] = 0;
						}
						else if ( Finger == 1 ) {
							BM_Slot2[0] = j + 1;
							BM_Slot[0] = 0;
						}
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ) {
					*rcode = 361;
					return SYS_ABORTED;
				}

				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 362;
					return SYS_ABORTED;
				}
			}
			else {
				//=====================================================================
				// 2006.09.29
				//=====================================================================
				if ( Finger == 2 ) {
					if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) Finger = 0; // 2006.12.21
				}
				//=====================================================================
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station );
				//
				for ( s = 0 ; s <= stmatm ; s++ ) {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s] ) ) {
						// For Go FEM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 363;
							return SYS_ABORTED;
						}
					}
				}
				//-------------------------------------------------------------------------------------------------------------------
				// 2004.05.07
				//-------------------------------------------------------------------------------------------------------------------
				if ( stmatm == 0 ) {
					if ( !Transfer_GET_MOVE_PLACE_SLOT_FOR_BM_AT_SWITCH( Finger , BM_Station[0] , &(BM_Slot[0]) , &(BM_Slot2[0]) , TRUE ) ) {
						*rcode = 364;
						return SYS_ABORTED;
					}
					BM_PSlot[0]  = BM_Slot[0];
					BM_PSlot2[0] = BM_Slot2[0];
				}
				//-------------------------------------------------------------------------------------------------------------------
			}
			//-------------------------------------------------------------------------------
			if ( ( Src_Slot > 0 ) && ( Src_Slot2 > 0 ) ) {
				if ( Finger == 0 ) {
					//--------------------------------------------------------
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 365;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , 0 , FM_CM , BM_Station[0] , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 366;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot[0] , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 367;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot2 );
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot2 , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 368;
						return SYS_ABORTED;
					}
					//--------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot2[0] , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 369;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
				}
				else if ( Finger == 1 ) {
					//--------------------------------------------------------
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 370;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , Src_Slot , FM_CM , BM_Station[0] , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 371;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , 0 , BM_Slot[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 372;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot2 );
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , 0 , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 373;
						return SYS_ABORTED;
					}
					//--------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , 0 , BM_Slot2[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 374;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
				}
				else if ( Finger == 2 ) {
					//--------------------------------------------------------
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot2 );
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 375;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , Src_Slot2 , FM_CM , BM_Station[0] , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 376;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot[0] , BM_Slot2[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 377;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
				}
			}
			else {
				//======================================================================================
				// 2007.10.02
				//======================================================================================
				if ( ( Src_Slot > 0 ) && ( Src_Slot2 <= 0 ) && ( Finger == 1 ) ) {
					Src_Slot2 = Src_Slot;
					Src_Slot = 0;
				}
				//======================================================================================
				if ( Src_Slot  > 0 ) {
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot );
				}
				else {
					if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) { // 2004.06.11
						BM_Slot[0] = 0; // 2004.06.11
						for ( s = 0 ; s <= stmatm ; s++ ) BM_PSlot[s] = 0; // 2004.06.11
					}
				}
				if ( Src_Slot2 > 0 ) {
					Transfer_Cass_Info_Operation_Before_Pick( Src_Station , Src_Slot2 );
				}
				else {
					if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) { // 2004.06.11
						BM_Slot2[0] = 0; // 2004.06.11
						for ( s = 0 ; s <= stmatm ; s++ ) BM_PSlot2[s] = 0; // 2004.06.11
					}
				}
				//--------------------------------------------------------
				if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Src_Station , Src_Slot , Src_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
					*rcode = 378;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//--------------------------------
				if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , Src_Slot , Src_Slot2 , FM_CM , BM_Station[0] , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
					*rcode = 379;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//--------------------------------
				//--------------------------------
				if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot[0] , BM_Slot2[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
					*rcode = 380;
					return SYS_ABORTED;
				}
				//--------------------------------
			}
			//--------------------------------
			_i_SCH_IO_MTL_SAVE();
			//--------------------------------
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 381;
					return SYS_ABORTED;
				}

				Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , BM_Station[0] );

				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , BM_Station[0] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_Slot[0] , BM_Slot2[0] ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 382;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//--------------------------------
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 383;
					return SYS_ABORTED;
				}

				_i_SCH_IO_MTL_SAVE();
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 384;
					return SYS_ABORTED;
				}

				Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , BM_Station[0] );
				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , BM_Station[0] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_Slot[0] , BM_Slot2[0] ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 385;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//--------------------------------
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , TR_CHECKING_SIDE , Trg_Station , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 386;
					return SYS_ABORTED;
				}

				_i_SCH_IO_MTL_SAVE();
			}
			else {
				for ( s = 0 ; s <= stmatm ; s++ ) {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s] ) ) {
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 387;
							return SYS_ABORTED;
						}
					}
					//-------------------------------------------------------------------------------
					//----------------------------------------------------------------------------------------------------------------
					// 2004.08.19
					//----------------------------------------------------------------------------------------------------------------
					Finger2 = 2;
					if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( &Finger2 , _i_SCH_PRM_GET_MODULE_GROUP( BM_Station[s] ) , _i_SCH_PRM_GET_MODULE_GROUP( Trg_Station ) , BM_Station[s] , Trg_Station ) ) {
						*rcode = 388;
						return SYS_ABORTED;
					}
					if ( Finger2 == 2 ) {
						Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( s , BM_Station[s] );
					}
					//----------------------------------------------------------------------------------------------------------------
					//Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( s , BM_Station[s] ); // 2004.08.19
					//----------------------------------------------------------------------------------------------------------------
					if ( EQUIP_PICK_FROM_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_PSlot[s] , BM_PSlot2[s] ) , FALSE , FM_CM , FALSE , 0, 0 ) == SYS_ABORTED ) {
						*rcode = 389;
						return SYS_ABORTED; // 2004.05.06
					}
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
					if ( s == stmatm ) {
						if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , Trg_Station , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 390;
							return SYS_ABORTED;
						}
					}
					else {
						if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s+1] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_PSlot[s+1] , BM_PSlot2[s+1] ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 391;
							return SYS_ABORTED;
						}
					}
					_i_SCH_IO_MTL_SAVE();
				}
			}
		}
		else if ( Use == 3 ) {	//p-t-b-f-c
			FM_CM = _i_SCH_IO_GET_MODULE_SOURCE( Src_Station , 1 ) + CM1;
			//
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ); // 2005.09.10

				Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );

				if      ( Finger == 0 ) {
					Src_Slot2 = 0;
					Trg_Slot2 = 0;
				}
				else if ( Finger == 1 ) {
					Src_Slot2 = Src_Slot;
					Src_Slot = 0;
					Trg_Slot2 = Trg_Slot;
					Trg_Slot = 0;
				}
				else if ( Finger == 2 ) {
					Src_Slot2 = 0;
					Trg_Slot2 = 0;
					Finger = 0;
				}
				else {
					*rcode = 392;
					return SYS_ABORTED;
				}

				BM_Station[0] = _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) + BM1;
				//
				switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( BM_Station[0] ) ) {
				case 1 : // Last
				case 3 :
					for ( j = _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) - 1 ; j >= 0 ; j-- ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[0] , j + 1 ) <= 0 ) {
							if      ( Finger == 0 ) {
								BM_Slot[0] = j + 1;
								BM_Slot2[0] = 0;
							}
							else if ( Finger == 1 ) {
								BM_Slot2[0] = j + 1;
								BM_Slot[0] = 0;
							}
							break;
						}
					}
					if ( j == -1 ) {
						*rcode = 393;
						return SYS_ABORTED;
					}
					break;
				default : // First
					for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ; j++ ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[0] , j + 1 ) <= 0 ) {
							if      ( Finger == 0 ) {
								BM_Slot[0] = j + 1;
								BM_Slot2[0] = 0;
							}
							else if ( Finger == 1 ) {
								BM_Slot2[0] = j + 1;
								BM_Slot[0] = 0;
							}
							break;
						}
					}
					if ( j == _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ) {
						*rcode = 394;
						return SYS_ABORTED;
					}
					break;
				}

				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 395;
					return SYS_ABORTED;
				}
				//-------------------------------------------------------------------------------
				Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , Src_Station );

				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 396;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//---------------------------------
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , BM_Station[0] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_Slot[0] , BM_Slot2[0] ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 397;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//
				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 398;
					return SYS_ABORTED;
				}
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ); // 2005.09.10

				Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );

				if      ( Finger == 0 ) {
					Src_Slot2 = 0;
					Trg_Slot2 = 0;
				}
				else if ( Finger == 1 ) {
					Src_Slot2 = Src_Slot;
					Src_Slot = 0;
					Trg_Slot2 = Trg_Slot;
					Trg_Slot = 0;
				}
				else if ( Finger == 2 ) {
					Src_Slot2 = 0;
					Trg_Slot2 = 0;
					Finger = 0;
				}
				else {
					*rcode = 399;
					return SYS_ABORTED;
				}

				BM_Station[0] = _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) + BM1;

				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[0] , j + 1 ) <= 0 ) {
						if      ( Finger == 0 ) {
							BM_Slot[0] = j + 1;
							BM_Slot2[0] = 0;
						}
						else if ( Finger == 1 ) {
							BM_Slot2[0] = j + 1;
							BM_Slot[0] = 0;
						}
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( BM_Station[0] ) ) {
					*rcode = 401;
					return SYS_ABORTED;
				}

				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 402;
					return SYS_ABORTED;
				}
				//-------------------------------------------------------------------------------
				Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , Src_Station );

				if ( EQUIP_PICK_FROM_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , Src_Station , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 403;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//---------------------------------
				if ( EQUIP_PLACE_TO_CHAMBER_TT( _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , TR_CHECKING_SIDE , BM_Station[0] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_Slot[0] , BM_Slot2[0] ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 404;
					return SYS_ABORTED;
				}
				_i_SCH_IO_MTL_SAVE();
				//
				if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
					*rcode = 405;
					return SYS_ABORTED;
				}
			}
			else {
				//=====================================================================
				// 2006.09.29
				//=====================================================================
				if ( Finger == 2 ) {
					if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) Finger = 0; // 2006.12.21
				}
				//=====================================================================
				stmatm = _i_SCH_PRM_GET_MODULE_GROUP( Src_Station );
				//
				for ( s = 0 ; s <= stmatm ; s++ ) {
					if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s] ) ) {
						// For Go FEM-BM
						if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s] , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
							*rcode = 406;
							return SYS_ABORTED;
						}
					}
				}
				//-------------------------------------------------------------------------------------------------------------------
				// 2004.05.07
				//-------------------------------------------------------------------------------------------------------------------
				if ( stmatm == 0 ) {
					if ( !Transfer_GET_MOVE_PLACE_SLOT_FOR_BM_AT_SWITCH( Finger , BM_Station[0] , &(BM_Slot[0]) , &(BM_Slot2[0]) , FALSE ) ) {
						*rcode = 407;
						return SYS_ABORTED;
					}
					BM_PSlot[0]  = BM_Slot[0];
					BM_PSlot2[0] = BM_Slot2[0];
				}
				//-------------------------------------------------------------------------------------------------------------------
				//=====================================================================
				// 2004.06.11
				//=====================================================================
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) { // 2004.06.11
					if ( Finger == 0 ) {
						Src_Slot2 = 0;
						Trg_Slot2 = 0;
						for ( s = 0 ; s <= stmatm ; s++ ) {
							BM_PSlot2[s] = 0;
							BM_Slot2[s] = 0;
						}
					}
					else if ( Finger == 1 ) {
						Src_Slot2 = Src_Slot;
						Src_Slot = 0;
						Trg_Slot2 = Trg_Slot;
						Trg_Slot = 0;
						for ( s = 0 ; s <= stmatm ; s++ ) {
//							BM_PSlot2[s] = BM_PSlot[s]; // 2007.10.02
							if ( BM_PSlot[s] != 0 ) BM_PSlot2[s] = BM_PSlot[s]; // 2007.10.02
//							BM_Slot2[s] = BM_Slot[s]; // 2007.10.02
							if ( BM_Slot[s] != 0 ) BM_Slot2[s] = BM_Slot[s]; // 2007.10.02
							BM_PSlot[s] = 0;
							BM_Slot[s] = 0;
						}
					}
					else if ( Finger == 2 ) {
						Src_Slot2 = 0;
						Trg_Slot2 = 0;
						Finger = 0;
						for ( s = 0 ; s <= stmatm ; s++ ) {
							BM_PSlot2[s] = 0;
							BM_Slot2[s] = 0;
						}
					}
					else {
						*rcode = 408;
						return SYS_ABORTED;
					}
				}
				//======================================================================================
				// 2007.10.02
				//======================================================================================
				//if ( ( Src_Slot > 0 ) && ( Src_Slot <= 0 ) && ( Finger == 1 ) ) {
				//	Src_Slot2 = Src_Slot;
				//	Src_Slot = 0;
				//}
				//======================================================================================
				//======================================================================================
				// 2007.10.02
				//======================================================================================
				if ( ( Trg_Slot > 0 ) && ( Trg_Slot2 <= 0 ) && ( Finger == 1 ) ) {
					Trg_Slot2 = Trg_Slot;
					Trg_Slot = 0;
				}
				//======================================================================================
				//=====================================================================
				for ( s = stmatm ; s >= 0 ; s-- ) {
					if ( s == stmatm ) {
						//----------------------------------------------------------------------------------------------------------------
						// 2004.08.19
						//----------------------------------------------------------------------------------------------------------------
						Finger2 = 2;
						if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( &Finger2 , _i_SCH_PRM_GET_MODULE_GROUP( Src_Station ) , _i_SCH_PRM_GET_MODULE_GROUP( BM_Station[s] ) , Src_Station , BM_Station[s] ) ) {
							*rcode = 409;
							return SYS_ABORTED;
						}
						if ( Finger2 == 2 ) {
							Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( s , Src_Station );
						}
						//----------------------------------------------------------------------------------------------------------------
						//Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( s , Src_Station ); // 2004.08.19
						//----------------------------------------------------------------------------------------------------------------
						if ( EQUIP_PICK_FROM_CHAMBER_TT( s , TR_CHECKING_SIDE , Src_Station , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 410;
							return SYS_ABORTED;
						}
						_i_SCH_IO_MTL_SAVE();
						//---------------------------------
						if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_PSlot[s] , BM_PSlot2[s] ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 411;
							return SYS_ABORTED;
						}
						_i_SCH_IO_MTL_SAVE();
						//
					}
					else {
						//-------------------------------------------------------------------------------
						Finger2 = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( s , BM_Station[s+1] );
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[s+1] ) ) {
							if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[s+1] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
								*rcode = 412;
								return SYS_ABORTED;
							}
						}
						if ( EQUIP_PICK_FROM_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s+1] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_PSlot[s+1] , BM_PSlot2[s+1] ) , FALSE , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 413;
							return SYS_ABORTED;
						}
						_i_SCH_IO_MTL_SAVE();
						//---------------------------------
						if ( EQUIP_PLACE_TO_CHAMBER_TT( s , TR_CHECKING_SIDE , BM_Station[s] , Finger2 , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( BM_PSlot[s] , BM_PSlot2[s] ) , 0 , FM_CM , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 414;
							return SYS_ABORTED;
						}
						_i_SCH_IO_MTL_SAVE();
						//
					}
				}
				//-------- Switch Need 2001.07.05
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( BM_Station[0] ) ) {
					// For Go FEM-BM
					if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , BM_Station[0] , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
						*rcode = 415;
						return SYS_ABORTED;
					}
				}
			}
			//-------------------------------------------------------------------------------
			if ( ( Trg_Slot > 0 ) && ( Trg_Slot2 > 0 ) ) {
				if ( Finger == 0 ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot[0] , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 416;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 417;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 418;
						return SYS_ABORTED;
					}
					//--------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot2[0] , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 419;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot2 , 0 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 420;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot2 , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 421;
						return SYS_ABORTED;
					}
					//--------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
				}
				else if ( Finger == 1 ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , 0 , BM_Slot[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 422;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 423;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 424;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					//--------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , 0 , BM_Slot2[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 425;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot2 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 426;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 427;
						return SYS_ABORTED;
					}
					//--------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
				}
				else if ( Finger == 2 ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot[0] , BM_Slot2[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 428;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 429;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 430;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
				}
			}
			else if ( Trg_Slot > 0 ) {
				if ( ( Finger == 0 ) || ( Finger == 2 ) ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot[0] , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 431;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					//--------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
//					if ( DoArmMX > 0 ) { // 2003.10.20 // 2007.10.02
//						for ( a = 0 ; a < DoArmMX ; a++ ) { // 2007.10.02
//							Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot + a + 1 ); // 2007.10.02
//						} // 2007.10.02
//					} // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 432;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 433;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					if ( DoArmMX > 0 ) { // 2003.10.20 // 2007.10.02
						for ( a = 0 ; a < DoArmMX ; a++ ) { // 2007.10.02
							Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot + a + 1 ); // 2007.10.02
						} // 2007.10.02
					} // 2007.10.02
					//--------------------------------
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------------------------------
				}
				else if ( Finger == 1 ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , 0 , BM_Slot[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 434;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 435;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 436;
						return SYS_ABORTED;
					}
					//--------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
				}
			}
			else if ( Trg_Slot2 > 0 ) {
				if ( ( Finger == 0 ) || ( Finger == 2 ) ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , BM_Slot2[0] , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 437;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot2 , 0 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 438;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot2 , 0 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 439;
						return SYS_ABORTED;
					}
					//--------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
				}
				else if ( Finger == 1 ) {
					//--------------------------------------------------------
					if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , BM_Station[0] , 0 , BM_Slot2[0] , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 440;
						return SYS_ABORTED;
					}
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
					//--------------------------------
//					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------
					if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot2 , FM_CM , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
						*rcode = 441;
						return SYS_ABORTED;
					}
					_i_SCH_IO_MTL_SAVE();
					if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot2 , TRUE , FM_CM , 0 ) == SYS_ABORTED ) {
						*rcode = 442;
						return SYS_ABORTED;
					}
					//--------------------------------
					Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 ); // 2007.10.02
					//--------------------------------------------------------
					_i_SCH_IO_MTL_SAVE();
				}
			}
		}
	}
	if ( Mode == TR_CLEAR ) {
		//============================================================================================================
		// 2005.01.06
		//============================================================================================================
		if ( !Transfer_Enable_Check_until_Process_Finished() ) {
			*rcode = 701;
			return SYS_ABORTED;
		}
		//============================================================================================================
		if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
			for ( i = 0 ; i < _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
				CLSYES[i] = -1;
				xW_SOURCE_IN_TM[i][ TA_STATION ] = WAFER_SOURCE_IN_TM( i , TA_STATION );
				xW_STATUS_IN_TM[i][ TA_STATION ] = WAFER_STATUS_IN_TM( i , TA_STATION );
				//
				xW_SOURCE_IN_TM[i][ TB_STATION ] = WAFER_SOURCE_IN_TM( i , TB_STATION );
				xW_STATUS_IN_TM[i][ TB_STATION ] = WAFER_STATUS_IN_TM( i , TB_STATION );
				//
				xW_SOURCE_IN_TM2[i][ TA_STATION ] = WAFER_SOURCE_IN_TM2( i , TA_STATION );
				xW_STATUS_IN_TM2[i][ TA_STATION ] = WAFER_STATUS_IN_TM2( i , TA_STATION );
				//
				xW_SOURCE_IN_TM2[i][ TB_STATION ] = WAFER_SOURCE_IN_TM2( i , TB_STATION );
				xW_STATUS_IN_TM2[i][ TB_STATION ] = WAFER_STATUS_IN_TM2( i , TB_STATION );
			}
			for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
				xW_SOURCE_IN_CHAMBER[ i ] = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 );
				xW_STATUS_IN_CHAMBER[ i ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
				xW_STATUS_IN_CHAMBER[ i ][ 2 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 2 );
				xW_STATUS_IN_CHAMBER[ i ][ 3 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 3 );
				xW_STATUS_IN_CHAMBER[ i ][ 4 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 4 );
				xW_STATUS_IN_CHAMBER[ i ][ 5 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 5 );
			}
			xW_SOURCE_IN_CHAMBER[ AL ]      = _i_SCH_IO_GET_MODULE_SOURCE( AL , 1 );
			xW_STATUS_IN_CHAMBER[ AL ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( AL , 1 );
			xW_SOURCE_IN_CHAMBER[ IC ]      = _i_SCH_IO_GET_MODULE_SOURCE( IC , 1 );
			xW_STATUS_IN_CHAMBER[ IC ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( IC , 1 );
			//
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM ) ) {
				for ( i = 0 ; i < _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
					if ( ( xW_STATUS_IN_TM[i][ TA_STATION ] > 0 ) || ( xW_STATUS_IN_TM[i][ TB_STATION ] > 0 ) ) {
						if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 702;
							return SYS_ABORTED;
						}
					}
					else {
						if ( xW_STATUS_IN_CHAMBER[ PM1 + i ][ 1 ] > 0 ) {
							if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 703;
								return SYS_ABORTED;
							}
						}
					}
				}
			}
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( FM ) ) {
				if ( EQUIP_CLEAR_CHAMBER( FM , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 704;
					return SYS_ABORTED;
				}
			}
		}
		else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
			//=====================================================================
			// 2005.11.08
			//=====================================================================
			//=====================================================================
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM ) ) {
				for ( i = 0 ; ( i < MAX_TM_CHAMBER_DEPTH ) && ( i < MAX_BM_CHAMBER_DEPTH ) ; i++ ) { // 2005.11.08
					if ( !_i_SCH_PRM_GET_MODULE_MODE( i + TM ) || !_i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) continue;
					if ( !Transfer_Enable_Check( i + TM ) ) continue;
					if ( !Transfer_Enable_Check( i + BM1 ) ) continue;
					//
					if ( ( WAFER_STATUS_IN_TM( i , TA_STATION ) > 0 ) || ( WAFER_STATUS_IN_TM( i , TB_STATION ) > 0 ) ) {
						if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 705;
							return SYS_ABORTED;
						}
					}
					else {
						if ( Transfer_GET_USE_BM_CHECK( i + BM1 ) ) {
							if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 706;
								return SYS_ABORTED;
							}
						}
						else {
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
								if ( !_i_SCH_PRM_GET_MODULE_MODE( PM1 + j ) ) continue;
								if ( _i_SCH_PRM_GET_MODULE_GROUP( PM1 + j ) != i ) continue;
								if ( !Transfer_Enable_Check( PM1 + j ) ) continue;
								if ( _i_SCH_IO_GET_MODULE_STATUS( PM1 + j , 1 ) <= 0 ) continue;
								if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 707;
									return SYS_ABORTED;
								}
								break;
							}
						}
					}
				}
			}
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( FM ) ) {
				if ( EQUIP_CLEAR_CHAMBER( FM , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 708;
					return SYS_ABORTED;
				}
			}
		}
		else {
			for ( i = 0 ; i <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; i++ ) {
				CLSYES[i] = -1;
				xW_SOURCE_IN_TM[i][ TA_STATION ] = WAFER_SOURCE_IN_TM( i , TA_STATION );
				xW_STATUS_IN_TM[i][ TA_STATION ] = WAFER_STATUS_IN_TM( i , TA_STATION );
				//
				xW_SOURCE_IN_TM[i][ TB_STATION ] = WAFER_SOURCE_IN_TM( i , TB_STATION );
				xW_STATUS_IN_TM[i][ TB_STATION ] = WAFER_STATUS_IN_TM( i , TB_STATION );
				//
				xW_SOURCE_IN_TM2[i][ TA_STATION ] = WAFER_SOURCE_IN_TM2( i , TA_STATION );
				xW_STATUS_IN_TM2[i][ TA_STATION ] = WAFER_STATUS_IN_TM2( i , TA_STATION );
				//
				xW_SOURCE_IN_TM2[i][ TB_STATION ] = WAFER_SOURCE_IN_TM2( i , TB_STATION );
				xW_STATUS_IN_TM2[i][ TB_STATION ] = WAFER_STATUS_IN_TM2( i , TB_STATION );
			}
			for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
				xW_SOURCE_IN_CHAMBER[ i ] = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 );
				xW_STATUS_IN_CHAMBER[ i ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
				xW_STATUS_IN_CHAMBER[ i ][ 2 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 2 );
				xW_STATUS_IN_CHAMBER[ i ][ 3 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 3 );
				xW_STATUS_IN_CHAMBER[ i ][ 4 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 4 );
				xW_STATUS_IN_CHAMBER[ i ][ 5 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 5 );
			}
			xW_SOURCE_IN_CHAMBER[ AL ]      = _i_SCH_IO_GET_MODULE_SOURCE( AL , 1 );
			xW_STATUS_IN_CHAMBER[ AL ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( AL , 1 );
			xW_SOURCE_IN_CHAMBER[ IC ]      = _i_SCH_IO_GET_MODULE_SOURCE( IC , 1 );
			xW_STATUS_IN_CHAMBER[ IC ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( IC , 1 );
			//----------------------------------------------------------------
			DMSKIP = 0;
			if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() != DM_NOT_USE ) && ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() > 0 ) ) {
				if ( xW_STATUS_IN_CHAMBER[ _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ][ _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() ] > 0 ) {
					if ( xW_SOURCE_IN_CHAMBER[ _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ] == MAX_CASSETTE_SIDE ) { // 2002.05.10
						if ( Transfer_Enable_Check( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) ) {
							DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
						}
					}
				}
			}
			//------------------------------------------------------------------------------------------
			//-- 2002.05.10
			//------------------------------------------------------------------------------------------
			SDMSKIPCOUNT = 0;
			for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
				if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) != 0 ) {
					if ( xW_STATUS_IN_CHAMBER[ _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) ][ _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d ) ] > 0 ) {
						if ( xW_SOURCE_IN_CHAMBER[ _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) ] > MAX_CASSETTE_SIDE ) {
							if ( Transfer_Enable_Check( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) ) ) {
								SDMSKIP[SDMSKIPCOUNT] = _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d );
								SDMSKIPCOUNT++;
							}
						}
					}
				}
			}
			//
			DMYES = 0;
			for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
				if ( Transfer_Enable_Check( i ) ) {
					for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) { // 2002.05.10
						if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == i ) break;
					}
					if ( d == MAX_SDUMMY_DEPTH ) {
						if ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) {
							if ( xW_SOURCE_IN_CHAMBER[ i ] == MAX_CASSETTE_SIDE ) { // 2002.05.10
								if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) ) {
									*rcode = 710;
									return SYS_ABORTED;
								}
								if ( DMYES != 0 ) {
									*rcode = 711;
									return SYS_ABORTED;
								}
								DMYES++;
							}
							if ( xW_SOURCE_IN_CHAMBER[ i ] > MAX_CASSETTE_SIDE ) {
								for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
									if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] - 1 + PM1 ) ) break;
								}
								if ( d == MAX_SDUMMY_DEPTH ) {
									*rcode = 712;
									return SYS_ABORTED;
								}
							}
						}
					}
				}
			}
			//
			for ( i = 0 ; i <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; i++ ) {
				if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i,TA_STATION ) && ( xW_STATUS_IN_TM[ i ][ TA_STATION ] > 0 ) ) {
					if ( xW_SOURCE_IN_TM[ i ][ TA_STATION ] == MAX_CASSETTE_SIDE ) { // 2002.05.10
						if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) ) {
							*rcode = 713;
							return SYS_ABORTED;
						}
						if ( DMYES  != 0 ) {
							*rcode = 714;
							return SYS_ABORTED;
						}
						DMYES++;
					}
					if ( xW_SOURCE_IN_TM[ i ][ TA_STATION ] > MAX_CASSETTE_SIDE ) { // 2002.05.10
						for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
							if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == ( xW_STATUS_IN_TM[ i ][ TA_STATION ] - 1 + PM1 ) ) break;
						}
						if ( d == MAX_SDUMMY_DEPTH ) {
							*rcode = 715;
							return SYS_ABORTED;
						}
					}
				}
				if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i,TB_STATION ) && ( xW_STATUS_IN_TM[ i ][ TB_STATION ] > 0 ) ) {
					if ( xW_SOURCE_IN_TM[ i ][ TB_STATION ] >= MAX_CASSETTE_SIDE ) {
						if ( ( _i_SCH_PRM_GET_DUMMY_PROCESS_MODE() == DM_NOT_USE ) || ( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() <= 0 ) ) {
							*rcode = 716;
							return SYS_ABORTED;
						}
						if ( DMYES  != 0 ) {
							*rcode = 717;
							return SYS_ABORTED;
						}
						DMYES++;
					}
					if ( xW_SOURCE_IN_TM[ i ][ TB_STATION ] > MAX_CASSETTE_SIDE ) { // 2002.05.10
						for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
							if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == ( xW_STATUS_IN_TM[ i ][ TB_STATION ] - 1 + PM1 ) ) break;
						}
						if ( d == MAX_SDUMMY_DEPTH ) {
							*rcode = 718;
							return SYS_ABORTED;
						}
					}
				}
			}
			//
			for ( j = 0 ; j <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; j++ ) {
				for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
					if ( DMSKIP != i ) {
						for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.10
							if ( SDMSKIP[d] == i ) break;
						}
						if ( d == SDMSKIPCOUNT ) {
							if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == j ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
								CLSYES[j] = i;
							}
						}
					}
				}
			}
			//
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( TM ) ) {
				for ( i = 1 ; i <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; i++ ) {
					if ( ( xW_STATUS_IN_TM[i][ TA_STATION ] > 0 ) || ( xW_STATUS_IN_TM[i][ TB_STATION ] > 0 ) ) {
						if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 719;
							return SYS_ABORTED;
						}
					}
					else {
						for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
							if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( j ) == i ) && ( xW_STATUS_IN_CHAMBER[ j ][ 1 ] > 0 ) ) {
								if ( EQUIP_CLEAR_CHAMBER( TM + i , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 720;
									return SYS_ABORTED;
								}
								break;
							}
						}
					}
				}
				if ( EQUIP_CLEAR_CHAMBER( TM , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 721;
					return SYS_ABORTED;
				}
			}
			if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( FM ) ) {
				if ( EQUIP_CLEAR_CHAMBER( FM , 0 , 0 ) == SYS_ABORTED ) {
					*rcode = 722;
					return SYS_ABORTED;
				}
			}
		}
		//=================================================================================================================
		//=================================================================================================================
		// Starting Clear Action
		//=================================================================================================================
		//=================================================================================================================

		//========================================
		// Clear Action Group 1
		//========================================
		if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
			do {
				grp = 0;
				for ( i = PM1 ; i < PM1 + _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() ; i++ ) {
//					if ( !Transfer_Enable_Check( i ) ) continue; // 2002.09.12 // 2004.06.16
					if ( !Transfer_Enable_Check( i - PM1 + BM1 ) ) continue; // 2002.09.12
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == ( i - PM1 + BM1 ) ) continue; // 2002.09.12

					d = 0; // 2006.01.24

					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i - PM1 , TA_STATION ) && ( xW_STATUS_IN_TM[i - PM1][ TA_STATION ] > 0 ) ) {
						Trg_Slot2 = 0;
						//-------------------------------------------------------------
						// 2006.02.03
						//-------------------------------------------------------------
						p = 0;
//						switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
//						case 1 : // Last
//						case 3 :
//							p = 2;
//							break;
//						}
						//-------------------------------------------------------------
						if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() <= 1 ) {
							switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
							case 1 : // Last
							case 3 :
								p = 2;
								break;
							}
						}
						else {
							if ( _SCH_CASSETTE_LAST_RESULT_GET( xW_SOURCE_IN_TM[i - PM1][ TA_STATION ] + CM1 , xW_STATUS_IN_TM[i - PM1][ TA_STATION ] ) == -2 ) {
								switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
								case 2 : // Last
								case 3 :
									p = 2;
									break;
								}
							}
							else {
								switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
								case 1 : // Last
								case 3 :
									p = 2;
									break;
								}
							}
						}
						//-------------------------------------------------------------------------------
						if ( !Transfer_GET_FREE_BM_CHECK( FALSE , i - PM1 + BM1 , &Trg_Slot , p ) ) {
							*rcode = 723;
							return SYS_ABORTED;
						}
						//-------------------------------------------------------------------------------
						if ( EQUIP_PLACE_TO_CHAMBER_TT( i - PM1 , TR_CHECKING_SIDE , i - PM1 + BM1 , TA_STATION , 1 , Trg_Slot , 0 , xW_SOURCE_IN_TM[i - PM1][ TA_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 724;
							return SYS_ABORTED;
						}
						//-----------------------------------------------
						xW_STATUS_IN_TM[i - PM1][ TA_STATION ] = 0;
						//-----------------------------------------------
						if ( _i_SCH_PRM_GET_CLSOUT_USE( i - PM1 + BM1 ) == 1 ) {
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( i - PM1 + BM1 ) ) ) {
								*rcode = 725;
								return SYS_ABORTED;
							}
						}
						d = 1; // 2006.01.24
					}
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i - PM1 , TB_STATION ) && ( xW_STATUS_IN_TM[i - PM1][ TB_STATION ] > 0 ) ) {
						Trg_Slot2 = 0;
						//-------------------------------------------------------------
						// 2006.02.03
						//-------------------------------------------------------------
						p = 0;
//						switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
//						case 1 : // Last
//						case 3 :
//							p = 2;
//							break;
//						}
						//-------------------------------------------------------------
						if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() <= 1 ) {
							switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
							case 1 : // Last
							case 3 :
								p = 2;
								break;
							}
						}
						else {
							if ( _SCH_CASSETTE_LAST_RESULT_GET( xW_SOURCE_IN_TM[i - PM1][ TB_STATION ] + CM1 , xW_STATUS_IN_TM[i - PM1][ TB_STATION ] ) == -2 ) {
								switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
								case 2 : // Last
								case 3 :
									p = 2;
									break;
								}
							}
							else {
								switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
								case 1 : // Last
								case 3 :
									p = 2;
									break;
								}
							}
						}
						//-------------------------------------------------------------------------------
						if ( !Transfer_GET_FREE_BM_CHECK( FALSE , i - PM1 + BM1 , &Trg_Slot , p ) ) {
							*rcode = 726;
							return SYS_ABORTED;
						}
						//-------------------------------------------------------------------------------
						if ( EQUIP_PLACE_TO_CHAMBER_TT( i - PM1 , TR_CHECKING_SIDE , i - PM1 + BM1 , TB_STATION , 1 , Trg_Slot , 0 , xW_SOURCE_IN_TM[i - PM1][ TB_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 727;
							return SYS_ABORTED;
						}
						//-----------------------------------------------
						xW_STATUS_IN_TM[i - PM1][ TB_STATION ] = 0;
						//-----------------------------------------------
						if ( _i_SCH_PRM_GET_CLSOUT_USE( i - PM1 + BM1 ) == 1 ) {
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( i - PM1 + BM1 ) ) ) {
								*rcode = 728;
								return SYS_ABORTED;
							}
						}
						d = 1; // 2006.01.24
					}
					//
					if ( !Transfer_Enable_Check( i ) ) {
						//------------------------------------------------------------------------------------
						if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 0 ) { // 2005.09.10
						//------------------------------------------------------------------------------------
							if ( Transfer_GET_USE_BM_CHECK( i - PM1 + BM1 ) ) { // 2004.06.1
								EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i - PM1 + BM1 , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
							}
						}
						continue; // 2002.09.12 // 2004.06.16
					}
					//
//					if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 1 ) { // 2006.01.24 // 2006.02.03
					if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() >= 1 ) { // 2006.01.24 // 2006.02.03
						if ( d == 0 ) {
							if ( Transfer_GET_USE_BM_CHECK( i - PM1 + BM1 ) ) d = 1;
						}
					}
					else {
						d = 0;
					}
					//
					if ( d != 0 ) { // 2006.01.24
						grp = 1; // 2006.01.24
					}
					else {
						if ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) {
							Trg_Slot2 = 0;
							//-------------------------------------------------------------
							p = 0;
							switch ( _i_SCH_PRM_GET_MODULE_DOUBLE_WHAT_SIDE( i ) ) {
							case 1 : // Last
							case 3 :
								p = 2;
								break;
							}
							//-------------------------------------------------------------
	//						if ( !Transfer_GET_FREE_BM_CHECK( FALSE , i - PM1 + BM1 , &Trg_Slot , p ) ) return SYS_ABORTED; // 2003.08.13
							if ( Transfer_GET_FREE_BM_CHECK( FALSE , i - PM1 + BM1 , &Trg_Slot , p ) ) { // 2003.08.13
								//-------------------------------------------------------------
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , i - PM1 + BM1 , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 729;
									return SYS_ABORTED;
								}

								if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i - PM1 , TA_STATION ) ) {
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
										if ( EQUIP_CLEAR_CHAMBER( i , TA_STATION , Trg_Slot ) == SYS_ABORTED ) {
											*rcode = 730;
											return SYS_ABORTED;
										}
									}
									if ( EQUIP_PICK_FROM_CHAMBER_TT( i - PM1 , TR_CHECKING_SIDE , i , TA_STATION , 1 , 1 , FALSE , xW_SOURCE_IN_CHAMBER[ i ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
										*rcode = 731;
										return SYS_ABORTED;
									}
									//=====================================
									xW_SOURCE_IN_TM[i - PM1][ TA_STATION ] = xW_SOURCE_IN_CHAMBER[ i ];
									xW_STATUS_IN_TM[i - PM1][ TA_STATION ] = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
									xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
									//=====================================
									if ( EQUIP_PLACE_TO_CHAMBER_TT( i - PM1 , TR_CHECKING_SIDE , i - PM1 + BM1 , TA_STATION , 1 , Trg_Slot , 0 , xW_SOURCE_IN_TM[i - PM1][ TA_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
										*rcode = 732;
										return SYS_ABORTED;
									}
									//-----------------------------------------------
									xW_STATUS_IN_TM[i - PM1][ TA_STATION ] = 0;
									//-----------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_USE( i - PM1 + BM1 ) == 1 ) {
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( i - PM1 + BM1 ) ) ) {
											*rcode = 733;
											return SYS_ABORTED;
										}
									}
								}
								else if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i - PM1 , TB_STATION ) ) {
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
										if ( EQUIP_CLEAR_CHAMBER( i , TB_STATION , Trg_Slot ) == SYS_ABORTED ) {
											*rcode = 734;
											return SYS_ABORTED;
										}
									}
									if ( EQUIP_PICK_FROM_CHAMBER_TT( i - PM1 , TR_CHECKING_SIDE , i , TB_STATION , 1 , 1 , FALSE , xW_SOURCE_IN_CHAMBER[ i ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
										*rcode = 735;
										return SYS_ABORTED;
									}
									//=====================================
									xW_SOURCE_IN_TM[i - PM1][ TB_STATION ] = xW_SOURCE_IN_CHAMBER[ i ];
									xW_STATUS_IN_TM[i - PM1][ TB_STATION ] = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
									xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
									//=====================================
									if ( EQUIP_PLACE_TO_CHAMBER_TT( i - PM1 , TR_CHECKING_SIDE , i - PM1 + BM1 , TB_STATION , 1 , Trg_Slot , 0 , xW_SOURCE_IN_TM[i - PM1][ TB_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
										*rcode = 736;
										return SYS_ABORTED;
									}
									//-----------------------------------------------
									xW_STATUS_IN_TM[i - PM1][ TB_STATION ] = 0;
									//-----------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_USE( i - PM1 + BM1 ) == 1 ) {
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( i - PM1 + BM1 ) ) ) {
											*rcode = 737;
											return SYS_ABORTED;
										}
									}
								}
							}
							else {
								grp = 1; // 2003.08.13
							}
						}
					}
					if ( Transfer_GET_USE_BM_CHECK( i - PM1 + BM1 ) ) {
						//------------------------------------------------------------------------------------
						if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 0 ) { // 2005.09.10
						//------------------------------------------------------------------------------------
							EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i - PM1 + BM1 , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
						}
					}
				}

				p = 1;
				while( TRUE ) {
					_i_SCH_IO_MTL_SAVE();
					if ( p == 0 ) break;
					p = 0;
					//---------------------------------------------------------------------------
					// Place to FM
					//---------------------------------------------------------------------------
					for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
						if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( i ) && ( WAFER_STATUS_IN_FM( 0 ,i ) > 0 ) ) {
							Trg_Station = WAFER_SOURCE_IN_FM( 0 , i );
							if ( Trg_Station >= MAX_CASSETTE_SIDE ) {
								Trg_Station = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
								Trg_Slot = WAFER_STATUS_IN_FM( 0 ,i );
								if ( Trg_Station == 0 ) {
									*rcode = 738;
									return SYS_ERROR;
								}
								if ( !Transfer_Enable_Check( Trg_Station ) ) {
									*rcode = 739;
									return SYS_ERROR;
								}
								if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) > 0 ) {
									*rcode = 740;
									return SYS_ERROR;
								}
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
									*rcode = 741;
									return SYS_ABORTED;
								}
								if ( i == 0 ) {
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 742;
										return SYS_ABORTED;
									}
								}
								else {
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 743;
										return SYS_ABORTED;
									}
								}
								//--------------------------------------------------------
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
								//--------------------------------------------------------
							}
							else {
								Trg_Station = Trg_Station + CM1;
								Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,i );
								if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) {
									*rcode = 744;
									return SYS_ERROR;
								}
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
									*rcode = 745;
									return SYS_ABORTED;
								}
								if ( i == 0 ) {
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 746;
										return SYS_ABORTED;
									}
								}
								else {
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 747;
										return SYS_ABORTED;
									}
								}
								//--------------------------------------------------------
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
								//--------------------------------------------------------
							}
							p = 1;
							break;
						}
					}
					if ( p == 1 ) continue;
					//---------------------------------------------------------------------------
					// Pick From FAL
					//---------------------------------------------------------------------------
					if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) {
						if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_AL_WAFER_SLOT() ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 748;
									return SYS_ABORTED;
								}
							}
							else {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 749;
									return SYS_ABORTED;
								}
							}
						}
						else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
								*rcode = 750;
								return SYS_ABORTED;
							}
						}
						else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
								*rcode = 751;
								return SYS_ABORTED;
							}
						}
						p = 1;
						continue;
					}
					//---------------------------------------------------------------------------
					// Pick From FIC
					//---------------------------------------------------------------------------
					if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
						if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 752;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 753;
									return SYS_ABORTED;
								}
							}
							p = 1;
							continue;
						}
					}
					else {
						if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 754;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 755;
									return SYS_ABORTED;
								}
							}
							p = 1;
							continue;
						}
					}
					//---------------------------------------------------------------------------
					// Pick From BM
					//---------------------------------------------------------------------------

/*

// 2015.01.23
					for ( i = BM1 ; i < ( BM1 + _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) {
						if ( !Transfer_Enable_Check( i ) ) continue; // 2002.09.12
						if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == i ) continue; // 2002.09.12
						Trg_Slot = 0;
						for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
								Trg_Slot = j + 1;
								break;
							}
						}
						if ( Trg_Slot > 0 ) {
							//-------------------------------------------------------------------------------
							CLSYES[ i ] = 1; // 2003.05.08
							//-------------------------------------------------------------------------------
							//------------------------------------------------------------------------------------
							if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() != 0 ) { // 2005.09.10
							//------------------------------------------------------------------------------------
								EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
							}
							//------------------------------------------------------------------------------------
							if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , i ) != SYS_SUCCESS ) {
								*rcode = 756;
								return SYS_ABORTED;
							}
							//------------------------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
								if ( EQUIP_CLEAR_CHAMBER( i , Trg_Slot , 0 ) == SYS_ABORTED ) {
									*rcode = 757;
									return SYS_ABORTED;
								}
							}
							s = _i_SCH_IO_GET_MODULE_SOURCE( i , Trg_Slot ) + CM1;
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , Trg_Slot , 0 , TRUE , s , 0 ) == SYS_ABORTED ) {
									*rcode = 758;
									return SYS_ABORTED;
								}
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
									*rcode = 759;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , 0 , Trg_Slot , TRUE , s , 0 ) == SYS_ABORTED ) {
									*rcode = 760;
									return SYS_ABORTED;
								}
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
									*rcode = 761;
									return SYS_ABORTED;
								}
							}
							p = 1;
							break;
						}
					}

					*/


// 2015.01.23
					//
					d = 0;
					//
					for ( i = BM1 ; i < ( BM1 + _i_SCH_PRM_GET_DFIM_MAX_PM_COUNT() ) ; i++ ) {
						if ( !Transfer_Enable_Check( i ) ) continue; // 2002.09.12
						if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == i ) continue; // 2002.09.12
						Trg_Slot = 0;
						for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
								Trg_Slot = j + 1;
								break;
							}
						}
						if ( Trg_Slot > 0 ) {
							//-------------------------------------------------------------------------------
							CLSYES[ i ] = 1; // 2003.05.08
							//-------------------------------------------------------------------------------
							//------------------------------------------------------------------------------------
							if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() != 0 ) { // 2005.09.10
							//------------------------------------------------------------------------------------
								EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
							}
							//------------------------------------------------------------------------------------
							if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , i ) != SYS_SUCCESS ) {
								*rcode = 756;
								return SYS_ABORTED;
							}
							//------------------------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
								if ( EQUIP_CLEAR_CHAMBER( i , Trg_Slot , 0 ) == SYS_ABORTED ) {
									*rcode = 757;
									return SYS_ABORTED;
								}
							}
							//
							s = _i_SCH_IO_GET_MODULE_SOURCE( i , Trg_Slot ) + CM1;
							//
							if ( d == 0 ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , Trg_Slot , 0 , TRUE , s , 0 ) == SYS_ABORTED ) {
										*rcode = 758;
										return SYS_ABORTED;
									}
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 759;
										return SYS_ABORTED;
									}
									//
									if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) d = 1;
									//
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , 0 , Trg_Slot , TRUE , s , 0 ) == SYS_ABORTED ) {
										*rcode = 760;
										return SYS_ABORTED;
									}
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 761;
										return SYS_ABORTED;
									}
								}
							}
							else {
								//
								d = 2;
								//
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , 0 , Trg_Slot , TRUE , s , 0 ) == SYS_ABORTED ) {
										*rcode = 760;
										return SYS_ABORTED;
									}
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 761;
										return SYS_ABORTED;
									}
								}
							}
							//
							p = 1;
							//
							if ( d != 1 ) break;
							//
						}
					}




				}
			}
			while( grp == 1 ); // 2003.08.13

			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( Transfer_Enable_Check( i + CM1 ) && _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i + CM1 ) ) {
					if ( CLSYES[ i + BM1 ] == 1 ) { // 2003.05.08
						if ( EQUIP_CLEAR_CHAMBER( i + CM1 , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 762;
							return SYS_ABORTED;
						}
					}
				}
			}
		}

		//========================================
		// Clear Action Group 2
		//========================================
		if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
			//=====================================================================================
			// 2005.11.08
			//=====================================================================================
			while( TRUE ) {
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				for ( i = BM1 ; ( i < ( BM1 + MAX_TM_CHAMBER_DEPTH ) ) && ( i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ; i++ ) {
					if ( !_i_SCH_PRM_GET_MODULE_MODE( i - BM1 + TM ) || !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
					if ( !Transfer_Enable_Check( i ) ) continue;
					//
					if ( Transfer_GET_USE_BM_CHECK( i ) ) {
						EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
					}
				}
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				p = 1;
				while( TRUE ) {
					_i_SCH_IO_MTL_SAVE();
					if ( p == 0 ) break;
					p = 0;
					//---------------------------------------------------------------------------
					// Place to FM
					//---------------------------------------------------------------------------
					for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
						if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( i ) && ( WAFER_STATUS_IN_FM( 0 , i ) > 0 ) ) {
							Trg_Station = WAFER_SOURCE_IN_FM( 0 , i );
							Trg_Station = Trg_Station + CM1;
							Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,i );
							if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) {
								*rcode = 763;
								return SYS_ERROR;
							}
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
								*rcode = 764;
								return SYS_ABORTED;
							}
							if ( i == 0 ) {
								if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
									*rcode = 765;
									return SYS_ABORTED;
								}
							}
							else {
								if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
									*rcode = 766;
									return SYS_ABORTED;
								}
							}
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
							//--------------------------------------------------------
							p = 1;
							break;
						}
					}
					if ( p == 1 ) continue;
					//---------------------------------------------------------------------------
					// Pick From FAL
					//---------------------------------------------------------------------------
					if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) {
						if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_AL_WAFER_SLOT() ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 774;
									return SYS_ABORTED;
								}
							}
							else {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 775;
									return SYS_ABORTED;
								}
							}
						}
						else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
							if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
								*rcode = 776;
								return SYS_ABORTED;
							}
						}
						else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
							if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
								*rcode = 777;
								return SYS_ABORTED;
							}
						}
						p = 1;
						continue;
					}
					//---------------------------------------------------------------------------
					// Pick From FIC
					//---------------------------------------------------------------------------
					if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
						if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 778;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) { // 2003.01.25
									 *rcode = 779;
									 return SYS_ABORTED;
								}
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 780;
									return SYS_ABORTED;
								}
							}
							p = 1;
							continue;
						}
					}
					else {
						if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 781;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) { // 2003.01.25
									 *rcode = 782;
									 return SYS_ABORTED;
								}
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 783;
									return SYS_ABORTED;
								}
							}
							p = 1;
							continue;
						}
					}
					//---------------------------------------------------------------------------
					// Pick From BM
					//---------------------------------------------------------------------------
					for ( i = BM1 ; ( i < ( BM1 + MAX_TM_CHAMBER_DEPTH ) ) && ( i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ; i++ ) {
						if ( !_i_SCH_PRM_GET_MODULE_MODE( i - BM1 + TM ) || !_i_SCH_PRM_GET_MODULE_MODE( i ) ) continue;
						if ( !Transfer_Enable_Check( i ) ) continue;
						//
						Trg_Slot = 0;
						for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
								Trg_Slot = j + 1;
								break;
							}
						}
						if ( Trg_Slot > 0 ) {
							//-------------------------------------------------------------------------------
							CLSYES[ i ] = 1; // 2003.05.08
							//-------------------------------------------------------------------------------
							if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , i ) != SYS_SUCCESS ) {
								*rcode = 784;
								return SYS_ABORTED;
							}
							//------------------------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
								if ( EQUIP_CLEAR_CHAMBER( i , Trg_Slot , 0 ) == SYS_ABORTED ) {
									*rcode = 785;
									return SYS_ABORTED;
								}
							}
							s = _i_SCH_IO_GET_MODULE_SOURCE( i , Trg_Slot ) + CM1;
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , Trg_Slot , 0 , TRUE , s , 0 ) == SYS_ABORTED ) {
									*rcode = 786;
									return SYS_ABORTED;
								}
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
									*rcode = 787;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) { // 2003.01.25
									 *rcode = 788;
									 return SYS_ABORTED;
								}
								if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , i , 0 , Trg_Slot , TRUE , s , 0 ) == SYS_ABORTED ) {
									*rcode = 789;
									return SYS_ABORTED;
								}
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
									*rcode = 790;
									return SYS_ABORTED;
								}
							}
							p = 1;
							break;
						}
					}
				}
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				for ( i = 0 ; ( i < MAX_TM_CHAMBER_DEPTH ) && ( i < MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) && _i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) {
						xW_SOURCE_IN_TM[i][ TA_STATION ] = WAFER_SOURCE_IN_TM( i , TA_STATION );
						xW_STATUS_IN_TM[i][ TA_STATION ] = WAFER_STATUS_IN_TM( i , TA_STATION );
						//
						xW_SOURCE_IN_TM[i][ TB_STATION ] = WAFER_SOURCE_IN_TM( i , TB_STATION );
						xW_STATUS_IN_TM[i][ TB_STATION ] = WAFER_STATUS_IN_TM( i , TB_STATION );
						//
						xW_SOURCE_IN_TM2[i][ TA_STATION ] = WAFER_SOURCE_IN_TM2( i , TA_STATION );
						xW_STATUS_IN_TM2[i][ TA_STATION ] = WAFER_STATUS_IN_TM2( i , TA_STATION );
						//
						xW_SOURCE_IN_TM2[i][ TB_STATION ] = WAFER_SOURCE_IN_TM2( i , TB_STATION );
						xW_STATUS_IN_TM2[i][ TB_STATION ] = WAFER_STATUS_IN_TM2( i , TB_STATION );
					}
					else {
						xW_SOURCE_IN_TM[i][ TA_STATION ] = 0;
						xW_STATUS_IN_TM[i][ TA_STATION ] = 0;
						//
						xW_SOURCE_IN_TM[i][ TB_STATION ] = 0;
						xW_STATUS_IN_TM[i][ TB_STATION ] = 0;
						//
						xW_SOURCE_IN_TM2[i][ TA_STATION ] = 0;
						xW_STATUS_IN_TM2[i][ TA_STATION ] = 0;
						//
						xW_SOURCE_IN_TM2[i][ TB_STATION ] = 0;
						xW_STATUS_IN_TM2[i][ TB_STATION ] = 0;
					}
				}
				//=====================================================================
				for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
					//
					xW_SOURCE_IN_CHAMBER[ i ] = 0;
					xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
					xW_STATUS_IN_CHAMBER[ i ][ 2 ] = 0;
					xW_STATUS_IN_CHAMBER[ i ][ 3 ] = 0;
					xW_STATUS_IN_CHAMBER[ i ][ 4 ] = 0;
					xW_STATUS_IN_CHAMBER[ i ][ 5 ] = 0;
					//
					if ( _i_SCH_PRM_GET_MODULE_MODE( i ) ) {
						if ( ( _i_SCH_PRM_GET_MODULE_GROUP( i ) < MAX_TM_CHAMBER_DEPTH ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) < MAX_BM_CHAMBER_DEPTH ) ) {
							if ( _i_SCH_PRM_GET_MODULE_MODE( _i_SCH_PRM_GET_MODULE_GROUP( i ) + TM ) && _i_SCH_PRM_GET_MODULE_MODE( _i_SCH_PRM_GET_MODULE_GROUP( i ) + BM1 ) ) {
								xW_SOURCE_IN_CHAMBER[ i ] = _i_SCH_IO_GET_MODULE_SOURCE( i , 1 );
								xW_STATUS_IN_CHAMBER[ i ][ 1 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 1 );
								xW_STATUS_IN_CHAMBER[ i ][ 2 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 2 );
								xW_STATUS_IN_CHAMBER[ i ][ 3 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 3 );
								xW_STATUS_IN_CHAMBER[ i ][ 4 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 4 );
								xW_STATUS_IN_CHAMBER[ i ][ 5 ] = _i_SCH_IO_GET_MODULE_STATUS( i , 5 );
							}
						}
					}
				}
				//=====================================================================
				d = 0;
				//=====================================================================
				for ( i = 0 ; ( i < MAX_TM_CHAMBER_DEPTH ) && ( i < MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( ( xW_STATUS_IN_TM[i][ TA_STATION ] > 0 ) || ( xW_STATUS_IN_TM[i][ TB_STATION ] > 0 ) ) {
						EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i + BM1 , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 );
						d = 1;
					}
					else {
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( xW_STATUS_IN_CHAMBER[ PM1 + j ][ 1 ] > 0 ) {
								if ( _i_SCH_PRM_GET_MODULE_GROUP( PM1 + j ) == i ) {
									EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i + BM1 , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 );
									d = 1;
									break;
								}
							}
						}
					}
				}
				//=====================================================================
				if ( d == 0 ) break;
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				//---------------------------------------------------------------------------------------------------------------------------
				p = 1;
				while( TRUE ) {
					_i_SCH_IO_MTL_SAVE();
					if ( p == 0 ) break;
					p = 0;
					//---------------------------------------------------------------------------
					// Place from TM
					//---------------------------------------------------------------------------
					for ( i = 0 ; ( i < MAX_TM_CHAMBER_DEPTH ) && ( i < MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( !_i_SCH_PRM_GET_MODULE_MODE( i + TM ) || !_i_SCH_PRM_GET_MODULE_MODE( i + BM1 ) ) continue;
						if ( !Transfer_Enable_Check( i + BM1 ) ) continue;
						//
						if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TA_STATION ) && ( xW_STATUS_IN_TM[ i ][ TA_STATION ] > 0 ) ) {
							Trg_Slot2 = 0;
							if ( !Transfer_GET_FREE_BM_CHECK( FALSE , i + BM1 , &Trg_Slot , 0 ) ) continue;
							//-------------------------------------------------------------------------------
							if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , i + BM1 ) != SYS_SUCCESS ) {
								*rcode = 791;
								return SYS_ABORTED;
							}
							//------------------------------------------------------------------------------------
							if ( EQUIP_PLACE_TO_CHAMBER_TT( i , TR_CHECKING_SIDE , i + BM1 , TA_STATION , 1 , Trg_Slot , 0 , xW_SOURCE_IN_TM[ i ][ TA_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 792;
								return SYS_ABORTED;
							}
							//-----------------------------------------------
							xW_STATUS_IN_TM[ i ][ TA_STATION ] = 0;
							//-----------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_USE( i + BM1 ) == 1 ) {
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( i + BM1 ) ) ) {
									*rcode = 793;
									return SYS_ABORTED;
								}
							}
							p = 1;
							break;
						}
						//
						if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TB_STATION ) && ( xW_STATUS_IN_TM[ i ][ TB_STATION ] > 0 ) ) {
							Trg_Slot2 = 0;
							if ( !Transfer_GET_FREE_BM_CHECK( FALSE , i + BM1 , &Trg_Slot , 0 ) ) continue;
							//-------------------------------------------------------------------------------
							if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , i + BM1 ) != SYS_SUCCESS ) {
								*rcode = 794;
								return SYS_ABORTED;
							}
							//------------------------------------------------------------------------------------
							if ( EQUIP_PLACE_TO_CHAMBER_TT( i , TR_CHECKING_SIDE , i + BM1 , TB_STATION , 1 , Trg_Slot , 0 , xW_SOURCE_IN_TM[ i ][ TA_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 795;
								return SYS_ABORTED;
							}
							//-----------------------------------------------
							xW_STATUS_IN_TM[ i ][ TB_STATION ] = 0;
							//-----------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_USE( i + BM1 ) == 1 ) {
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( i + BM1 ) ) ) {
									*rcode = 796;
									return SYS_ABORTED;
								}
							}
							p = 1;
							break;
						}
						//
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							if ( _i_SCH_PRM_GET_MODULE_GROUP( PM1 + j ) != i ) continue;
							if ( !Transfer_Enable_Check( PM1 + j ) ) continue;
							if ( xW_STATUS_IN_CHAMBER[ PM1 + j ][ 1 ] <= 0 ) continue;
							//
							if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TA_STATION ) ) {
								if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( PM1 + j ) ) {
									if ( EQUIP_CLEAR_CHAMBER( PM1 + j , TA_STATION , Trg_Slot ) == SYS_ABORTED ) {
										*rcode = 797;
										return SYS_ABORTED;
									}
								}
								if ( EQUIP_PICK_FROM_CHAMBER_TT( i , TR_CHECKING_SIDE , PM1 + j , TA_STATION , 1 , 1 , FALSE , xW_SOURCE_IN_CHAMBER[ PM1 + j ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 798;
									return SYS_ABORTED;
								}
								//=====================================
								xW_SOURCE_IN_TM[i][ TA_STATION ] = xW_SOURCE_IN_CHAMBER[ PM1 + j ];
								xW_STATUS_IN_TM[i][ TA_STATION ] = xW_STATUS_IN_CHAMBER[ PM1 + j ][ 1 ];
								xW_STATUS_IN_CHAMBER[ PM1 + j ][ 1 ] = 0;
								//=====================================
								p = 1;
								break;
							}
							else if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TB_STATION ) ) {
								if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( PM1 + j ) ) {
									if ( EQUIP_CLEAR_CHAMBER( PM1 + j , TB_STATION , Trg_Slot ) == SYS_ABORTED ) {
										*rcode = 799;
										return SYS_ABORTED;
									}
								}
								if ( EQUIP_PICK_FROM_CHAMBER_TT( i , TR_CHECKING_SIDE , PM1 + j , TB_STATION , 1 , 1 , FALSE , xW_SOURCE_IN_CHAMBER[ PM1 + j ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 801;
									return SYS_ABORTED;
								}
								//=====================================
								xW_SOURCE_IN_TM[i][ TB_STATION ] = xW_SOURCE_IN_CHAMBER[ PM1 + j ];
								xW_STATUS_IN_TM[i][ TB_STATION ] = xW_STATUS_IN_CHAMBER[ PM1 + j ][ 1 ];
								xW_STATUS_IN_CHAMBER[ PM1 + j ][ 1 ] = 0;
								//=====================================
								p = 1;
								break;
							}
						}
						if ( p == 1 ) break;
					}
				}
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( Transfer_Enable_Check( i + CM1 ) && _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i + CM1 ) ) {
					if ( CLSYES[ i + BM1 ] == 1 ) { // 2003.05.08
						if ( EQUIP_CLEAR_CHAMBER( i + CM1 , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 802;
							return SYS_ABORTED;
						}
					}
				}
			}
			//=====================================================================================
		}

		//========================================
		// Clear Action Group 3
		//========================================
		if (
			( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) &&
			( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_6_INLIGN_PM2_V1 ) &&
			( ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) && ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) ) // 2006.12.21
			) {
			for ( i = 0 ; i < MAX_CHAMBER ; i++ ) { // 2005.10.11
				BM_WAIT[i] = 0; // 2003.11.10 // 2005.10.11
			}
			while ( TRUE ) {
				clssw = 0;
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
					BM_RUN[i] = 0;
//					BM_WAIT[i] = 0; // 2003.11.10 // 2005.10.11
					if ( BM_WAIT[i] == 1 ) BM_WAIT[i] = 0; // 2005.10.11
				}
				BM_ALL_RUN = 0;
				if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( xW_STATUS_IN_TM[0][ TA_STATION ] > 0 ) ) BM_ALL_RUN++;
				if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( xW_STATUS_IN_TM[0][ TB_STATION ] > 0 ) ) BM_ALL_RUN++;
				if ( ( Transfer_Enable_Check( AL ) ) && ( xW_STATUS_IN_CHAMBER[ AL ][ 1 ] > 0 ) ) BM_ALL_RUN++;
				if ( ( Transfer_Enable_Check( IC ) ) && ( xW_STATUS_IN_CHAMBER[ IC ][ 1 ] > 0 ) ) BM_ALL_RUN++;
				for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
					if (
						( Transfer_Enable_Check( i ) ) &&
						( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 )
						) {
						BM_ALL_RUN++ ;
						BM_RUN[xW_SOURCE_IN_CHAMBER[ i ]]++;
					}
				}
				//-----------------------------------------------------------------------------------
				// 2004.05.06
				//-----------------------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 1 ) {
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( Transfer_Enable_Check( i ) ) {
							for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
									BM_ALL_RUN = 0;
									break;
								}
							}
						}
					}
				}
				if ( BM_ALL_RUN != 0 ) {
					if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() != 2 ) { // 2005.10.11
						for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
							if ( BM_RUN[i] != 0 ) {
								c = Transfer_GET_CLEAR_BM_AT_SWITCH( i ); // 2003.11.10
								if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
									if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
										*rcode = 803;
										return SYS_ABORTED;
									}
									BM_WAIT[ c ] = 1; // 2003.11.10
								}
							}
						}
					}
					else { // 2005.10.11
						if ( BM_ALL_RUN > 1 ) {
							j = 0;
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( Transfer_Enable_Check( i ) ) {
									if ( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AT_FIRST( i ) ) {
										j++;
										//------------------------------------------------------------------------------------
										EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , i , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 );
										BM_WAIT[ i ] = 2;
										//------------------------------------------------------------------------------------
										if ( BM_ALL_RUN == j ) break;
									}
								}
							}
						}
					}
					//--------------------------------------------------------------------------------------------------------
					// Pick IC + Place BM
					//--------------------------------------------------------------------------------------------------------
					if ( ( clssw == 0 ) && ( Transfer_Enable_Check( IC ) ) && ( xW_STATUS_IN_CHAMBER[ IC ][ 1 ] > 0 ) ) {
						if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , IC );
						else               f = Finger;
						s = xW_STATUS_IN_CHAMBER[ IC ][ 1 ];
						x = xW_SOURCE_IN_CHAMBER[ IC ];
						c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
						if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
							if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , IC , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 804;
								return SYS_ABORTED;
							}
							_i_SCH_IO_MTL_SAVE();
							//-------------------------------------------------------------------
							b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
							if ( b == -1 ) {
								*rcode = 805;
								return SYS_ERROR;
							}
							//-------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
								if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
									*rcode = 806;
									return SYS_ABORTED;
								}
							}
							//-------------------------------------------------------------------
							if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 807;
									return SYS_ABORTED;
								}
								BM_WAIT[ c ] = 1;
							}
							else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
								//-------------------------------------------------------------------------------
								if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
									*rcode = 808;
									return SYS_ABORTED;
								}
								//------------------------------------------------------------------------------------
								BM_WAIT[ c ] = 1;
								//-----------------------------------------------------------------------------------
							}
							//-------------------------------------------------------------------
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 809;
								return SYS_ABORTED;
							}
							//--------------------------------------------------------
							xW_STATUS_IN_CHAMBER[ IC ][ 1 ] = 0;
							//--------------------------------------------------------
							_i_SCH_IO_MTL_SAVE();
							//--------------------------------------------------------
							switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
							case 0 : // No Action
								break;
							case 1 : // Skip
								clssw = 1;
								break;
							case 2 : // Vent
								EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
//								BM_WAIT[ i ] = 3; // 2006.02.14
								BM_WAIT[ c ] = 3; // 2006.02.14
								break;
							}
							//--------------------------------------------------------
						}
						else {
							clssw = 1;
						}
					}
					//--------------------------------------------------------------------------------------------------------
					// Pick AL + Place BM
					//--------------------------------------------------------------------------------------------------------
					if ( ( clssw == 0 ) && ( Transfer_Enable_Check( AL ) ) && ( xW_STATUS_IN_CHAMBER[ AL ][ 1 ] > 0 ) ) {
						if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , AL );
						else               f = Finger;
						s = xW_STATUS_IN_CHAMBER[ AL ][ 1 ];
						x = xW_SOURCE_IN_CHAMBER[ AL ];
						c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
						if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
							if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , AL , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 810;
								return SYS_ABORTED;
							}
							_i_SCH_IO_MTL_SAVE();
							//-------------------------------------------------------------------
							b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
							if ( b == -1 ) {
								*rcode = 811;
								return SYS_ERROR;
							}
							//-------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
								if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
									*rcode = 812;
									return SYS_ABORTED;
								}
							}
							//-------------------------------------------------------------------
							if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 813;
									return SYS_ABORTED;
								}
								BM_WAIT[ c ] = 1;
							}
							else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
								//-------------------------------------------------------------------------------
								if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
									*rcode = 814;
									return SYS_ABORTED;
								}
								//------------------------------------------------------------------------------------
								BM_WAIT[ c ] = 1;
								//-----------------------------------------------------------------------------------
							}
							//-------------------------------------------------------------------
							if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 815;
								return SYS_ABORTED;
							}
							//--------------------------------------------------------
							xW_STATUS_IN_CHAMBER[ AL ][ 1 ] = 0;
							//--------------------------------------------------------
							_i_SCH_IO_MTL_SAVE();
							//--------------------------------------------------------
							switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
							case 0 : // No Action
								break;
							case 1 : // Skip
								clssw = 1;
								break;
							case 2 : // Vent
								EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
//								BM_WAIT[ i ] = 3; // 2006.02.14
								BM_WAIT[ c ] = 3; // 2006.02.14
								break;
							}
							//--------------------------------------------------------
						}
						else {
							clssw = 1;
						}
					}
					//
					if ( clssw == 0 ) {
						CLSYES[0] = -1;
						for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
							if ( ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
								CLSYES[0] = i;
							}
						}
						//--------------------------------------------------------------------------------------------------------
						// CLSOUT is Present
						//--------------------------------------------------------------------------------------------------------
						if ( CLSYES[0] >= 0 ) {
							CLSFIND = FALSE;
							if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( xW_STATUS_IN_TM[0][ TA_STATION ] <= 0 ) ) {
								if ( Finger != 1 ) {
									CLSFIND = TRUE;
								}
							}
							if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( xW_STATUS_IN_TM[0][ TB_STATION ] <= 0 ) ) {
								if ( Finger != 0 ) {
									CLSFIND = TRUE;
								}
							}
							if ( CLSFIND ) {
								//--------------------------------------------------------------------------------------------------------
								// Pick PM + Place ClsOutPM
								//--------------------------------------------------------------------------------------------------------
								for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
									if ( ( clssw == 0 ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) ) {
										//----------------------------------------------------------------------------------------------------------------
										//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i ); // 2004.08.19
										//else               f = Finger; // 2004.08.19
										//----------------------------------------------------------------------------------------------------------------
										s = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										x = xW_SOURCE_IN_CHAMBER[ i ];
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
										//----------------------------------------------------------------------------------------------------------------
										// 2004.08.19
										//----------------------------------------------------------------------------------------------------------------
										Finger2 = Finger;
										if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( &Finger2 , 0 , 0 , i , c ) ) {
											*rcode = 816;
											return SYS_ABORTED;
										}
										if ( Finger2 == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i );
										else                f = Finger2;
										//----------------------------------------------------------------------------------------------------------------
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
											if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
												if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) {
													*rcode = 817;
													return SYS_ABORTED;
												}
											}
											if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
												*rcode = 818;
												return SYS_ABORTED;
											}
											_i_SCH_IO_MTL_SAVE();
											//-------------------------------------------------------------------
											b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
											if ( b == -1 ) {
												*rcode = 819;
												return SYS_ERROR;
											}
											//-------------------------------------------------------------------
											if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
												if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
													*rcode = 820;
													return SYS_ABORTED;
												}
											}
											//-------------------------------------------------------------------
											if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
												if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
													*rcode = 821;
													return SYS_ABORTED;
												}
												BM_WAIT[ c ] = 1;
											}
											else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
												//-------------------------------------------------------------------------------
												if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
													*rcode = 822;
													return SYS_ABORTED;
												}
												//------------------------------------------------------------------------------------
												BM_WAIT[ c ] = 1;
												//-----------------------------------------------------------------------------------
											}
											//-------------------------------------------------------------------
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
												*rcode = 823;
												return SYS_ABORTED;
											}
											//--------------------------------------------------------
											xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
											//--------------------------------------------------------
											_i_SCH_IO_MTL_SAVE();
											//--------------------------------------------------------
											switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
											case 0 : // No Action
												break;
											case 1 : // Skip
												clssw = 1;
												break;
											case 2 : // Vent
												EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
				//								BM_WAIT[ i ] = 3; // 2006.02.14
												BM_WAIT[ c ] = 3; // 2006.02.14
												break;
											}
											//--------------------------------------------------------
										}
										else {
											clssw = 1;
										}
									}
								}
							}
							//--------------------------------------------------------------------------------------------------------
							// Place BM with Arm A
							// Place ClsOut with Arm A + Pick ClsOut with Arm A + Place BM with Arm A
							//--------------------------------------------------------------------------------------------------------
							if ( ( clssw == 0 ) && _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( xW_STATUS_IN_TM[0][ TA_STATION ] > 0 ) ) {
								CLSFIND = -1;
								if ( Finger != 1 ) {
									if ( _SCH_CASSETTE_LAST_RESULT_GET( xW_SOURCE_IN_TM[0][ TA_STATION ] + CM1 , xW_STATUS_IN_TM[0][ TA_STATION ] ) == -2 ) {
										s = xW_STATUS_IN_TM[0][ TA_STATION ];
										x = xW_SOURCE_IN_TM[0][ TA_STATION ];
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
											//-------------------------------------------------------------------
											b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
											if ( b == -1 ) {
												*rcode = 824;
												return SYS_ERROR;
											}
											//-------------------------------------------------------------------
											if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
												if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
													*rcode = 825;
													return SYS_ABORTED;
												}
											}
											//-------------------------------------------------------------------
											if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
												if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
													*rcode = 826;
													return SYS_ABORTED;
												}
												BM_WAIT[ c ] = 1;
											}
											else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
												//-------------------------------------------------------------------------------
												if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
													*rcode = 827;
													return SYS_ABORTED;
												}
												//------------------------------------------------------------------------------------
												BM_WAIT[ c ] = 1;
												//-----------------------------------------------------------------------------------
											}
											//--------------------------------------------------------
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TA_STATION , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
												*rcode = 828;
												return SYS_ABORTED;
											}
											//--------------------------------------------------------
											xW_STATUS_IN_TM[0][ TA_STATION ] = 0;
											//--------------------------------------------------------
											_i_SCH_IO_MTL_SAVE();
											//--------------------------------------------------------
											switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
											case 0 : // No Action
												break;
											case 1 : // Skip
												clssw = 1;
												break;
											case 2 : // Vent
												EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
				//								BM_WAIT[ i ] = 3; // 2006.02.14
												BM_WAIT[ c ] = 3; // 2006.02.14
												break;
											}
											//--------------------------------------------------------
										}
										else {
											clssw = 1;
										}
									}
									else {
										for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
											if ( ( Transfer_Enable_Check( i ) ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
												CLSFIND = i;
											}
										}
										if ( CLSFIND >= 0 ) {
											s = xW_STATUS_IN_TM[0][ TA_STATION ];
											x = xW_SOURCE_IN_TM[0][ TA_STATION ];
											c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
											if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , CLSFIND , TA_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 829;
													return SYS_ABORTED;
												}
												_i_SCH_IO_MTL_SAVE();
												if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) {
														*rcode = 830;
														return SYS_ABORTED;
													}
												}
												if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
													if ( EQUIP_CLEAR_CHAMBER( CLSFIND , TA_STATION , s ) == SYS_ABORTED ) {
														*rcode = 831;
														return SYS_ABORTED;
													}
												}
												if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , CLSFIND , TA_STATION , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 832;
													return SYS_ABORTED;
												}
												_i_SCH_IO_MTL_SAVE();
												//-------------------------------------------------------------------
												b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
												if ( b == -1 ) {
													*rcode = 833;
													return SYS_ERROR;
												}
												//-------------------------------------------------------------------
												if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
													if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
														*rcode = 834;
														return SYS_ABORTED;
													}
												}
												//-------------------------------------------------------------------
												if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
													if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
														*rcode = 835;
														return SYS_ABORTED;
													}
													BM_WAIT[ c ] = 1;
												}
												else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
													//-------------------------------------------------------------------------------
													if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
														*rcode = 836;
														return SYS_ABORTED;
													}
													//------------------------------------------------------------------------------------
													BM_WAIT[ c ] = 1;
													//-----------------------------------------------------------------------------------
												}
												//--------------------------------------------------------
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TA_STATION , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 837;
													return SYS_ABORTED;
												}
												//--------------------------------------------------------
												xW_STATUS_IN_TM[0][ TA_STATION ] = 0;
												//--------------------------------------------------------
												_i_SCH_IO_MTL_SAVE();
												//--------------------------------------------------------
												switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
												case 0 : // No Action
													break;
												case 1 : // Skip
													clssw = 1;
													break;
												case 2 : // Vent
													EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
				//									BM_WAIT[ i ] = 3; // 2006.02.14
													BM_WAIT[ c ] = 3; // 2006.02.14
													break;
												}
												//--------------------------------------------------------
											}
											else {
												clssw = 1;
											}
										}
										else {
											*rcode = 838;
											return SYS_ERROR;
										}
									}
								}
							}
							//--------------------------------------------------------------------------------------------------------
							// Place BM with Arm B
							// Place ClsOut with Arm B + Pick ClsOut with Arm B + Place BM with Arm B
							//--------------------------------------------------------------------------------------------------------
							if ( ( clssw == 0 ) && _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( xW_STATUS_IN_TM[0][ TB_STATION ] > 0 ) ) {
								CLSFIND = -1;
								if ( Finger != 0 ) {
									if ( _SCH_CASSETTE_LAST_RESULT_GET( xW_SOURCE_IN_TM[0][ TB_STATION ] + CM1 , xW_STATUS_IN_TM[0][ TB_STATION ] ) == -2 ) {
										s = xW_STATUS_IN_TM[0][ TB_STATION ];
										x = xW_SOURCE_IN_TM[0][ TB_STATION ];
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
											//-------------------------------------------------------------------
											b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
											if ( b == -1 ) {
												*rcode = 839;
												return SYS_ERROR;
											}
											//-------------------------------------------------------------------
											if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
												if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
													*rcode = 840;
													return SYS_ABORTED;
												}
											}
											//-------------------------------------------------------------------
											if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
												if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
													*rcode = 841;
													return SYS_ABORTED;
												}
												BM_WAIT[ c ] = 1;
											}
											else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
												//-------------------------------------------------------------------------------
												if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
													*rcode = 842;
													return SYS_ABORTED;
												}
												//------------------------------------------------------------------------------------
												BM_WAIT[ c ] = 1;
												//-----------------------------------------------------------------------------------
											}
											//--------------------------------------------------------
											if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TB_STATION , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
												*rcode = 843;
												return SYS_ABORTED;
											}
											//--------------------------------------------------------
											xW_STATUS_IN_TM[0][ TB_STATION ] = 0;
											//--------------------------------------------------------
											_i_SCH_IO_MTL_SAVE();
											//--------------------------------------------------------
											switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
											case 0 : // No Action
												break;
											case 1 : // Skip
												clssw = 1;
												break;
											case 2 : // Vent
												EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
				//								BM_WAIT[ i ] = 3; // 2006.02.14
												BM_WAIT[ c ] = 3; // 2006.02.14
												break;
											}
											//--------------------------------------------------------
										}
										else {
											clssw = 1;
										}
									}
									else {
										for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
											if ( ( Transfer_Enable_Check( i ) ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
												CLSFIND = i;
											}
										}
										if ( CLSFIND >= 0 ) {
											s = xW_STATUS_IN_TM[0][ TB_STATION ];
											x = xW_SOURCE_IN_TM[0][ TB_STATION ];
											c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
											if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , CLSFIND , TB_STATION , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 844;
													return SYS_ABORTED;
												}
												_i_SCH_IO_MTL_SAVE();
												if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) {
														*rcode = 845;
														return SYS_ABORTED;
													}
												}
												if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
													if ( EQUIP_CLEAR_CHAMBER( CLSFIND , TB_STATION , s ) == SYS_ABORTED ) {
														*rcode = 846;
														return SYS_ABORTED;
													}
												}
												if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , CLSFIND , TB_STATION , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 847;
													return SYS_ABORTED;
												}
												_i_SCH_IO_MTL_SAVE();
												//-------------------------------------------------------------------
												b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
												if ( b == -1 ) {
													*rcode = 848;
													return SYS_ERROR;
												}
												//-------------------------------------------------------------------
												if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
													if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
														*rcode = 849;
														return SYS_ABORTED;
													}
												}
												//-------------------------------------------------------------------
												if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
													if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
														*rcode = 850;
														return SYS_ABORTED;
													}
													BM_WAIT[ c ] = 1;
												}
												else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
													//-------------------------------------------------------------------------------
													if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
														*rcode = 851;
														return SYS_ABORTED;
													}
													//------------------------------------------------------------------------------------
													BM_WAIT[ c ] = 1;
													//-----------------------------------------------------------------------------------
												}
												//--------------------------------------------------------
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TB_STATION , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 852;
													return SYS_ABORTED;
												}
												//--------------------------------------------------------
												xW_STATUS_IN_TM[0][ TB_STATION ] = 0;
												//--------------------------------------------------------
												_i_SCH_IO_MTL_SAVE();
												//--------------------------------------------------------
												switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
												case 0 : // No Action
													break;
												case 1 : // Skip
													clssw = 1;
													break;
												case 2 : // Vent
													EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
				//									BM_WAIT[ i ] = 3; // 2006.02.14
													BM_WAIT[ c ] = 3; // 2006.02.14
													break;
												}
												//--------------------------------------------------------
											}
											else {
												clssw = 1;
											}
										}
										else {
											*rcode = 853;
											return SYS_ERROR;
										}
									}
								}
							}
							//--------------------------------------------------------------------------------------------------------
							// Place PM + Place BM
							//--------------------------------------------------------------------------------------------------------
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								if ( ( clssw == 0 ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) ) {
									if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i );
									else               f = Finger;
									s = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
									x = xW_SOURCE_IN_CHAMBER[ i ];
									c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
									if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
											if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) {
												*rcode = 854;
												return SYS_ABORTED;
											}
										}
										if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 855;
											return SYS_ABORTED;
										}
										_i_SCH_IO_MTL_SAVE();
										//-------------------------------------------------------------------
										b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
										if ( b == -1 ) {
											*rcode = 856;
											return SYS_ERROR;
										}
										//-------------------------------------------------------------------
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
											if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
												*rcode = 857;
												return SYS_ABORTED;
											}
										}
										//-------------------------------------------------------------------
										if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
											if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
												*rcode = 858;
												return SYS_ABORTED;
											}
											BM_WAIT[ c ] = 1;
										}
										else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
											//-------------------------------------------------------------------------------
											if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
												*rcode = 859;
												return SYS_ABORTED;
											}
											//------------------------------------------------------------------------------------
											BM_WAIT[ c ] = 1;
											//-----------------------------------------------------------------------------------
										}
										//-------------------------------------------------------------------
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 860;
											return SYS_ABORTED;
										}
										//--------------------------------------------------------
										xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
										//--------------------------------------------------------
										_i_SCH_IO_MTL_SAVE();
										//--------------------------------------------------------
										switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
										case 0 : // No Action
											break;
										case 1 : // Skip
											clssw = 1;
											break;
										case 2 : // Vent
											EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
			//								BM_WAIT[ i ] = 3; // 2006.02.14
											BM_WAIT[ c ] = 3; // 2006.02.14
											break;
										}
										//--------------------------------------------------------
									}
									else {
										clssw = 1;
									}
								}
							}
							//--------------------------------------------------------------------------------------------------------
							// Pick PM + Place ClsOut + Pick ClsOut + Place BM
							//--------------------------------------------------------------------------------------------------------
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								if ( ( clssw == 0 ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 0 ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) ) {
									//----------------------------------------------------------------------------------------------------------------
									//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i );	// 2004.08.19
									//else               f = Finger;	// 2004.08.19
									//----------------------------------------------------------------------------------------------------------------
									s = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
									x = xW_SOURCE_IN_CHAMBER[ i ];
									c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
									//----------------------------------------------------------------------------------------------------------------
									// 2004.08.19
									//----------------------------------------------------------------------------------------------------------------
									Finger2 = Finger;
									if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( &Finger2 , 0 , 0 , i , c ) ) {
										*rcode = 861;
										return SYS_ABORTED;
									}
									if ( Finger2 == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i );
									else                f = Finger2;
									//----------------------------------------------------------------------------------------------------------------
									if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
											if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) {
												*rcode = 862;
												return SYS_ABORTED;
											}
										}
										if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 863;
											return SYS_ABORTED;
										}
										//-------------
										xW_STATUS_IN_TM[0][ f ] = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										xW_SOURCE_IN_TM[0][ f ] = xW_SOURCE_IN_CHAMBER[ i ];
										xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
										//-------------
										_i_SCH_IO_MTL_SAVE();
										CLSFIND = -1;
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( xW_STATUS_IN_CHAMBER[ j ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 1 ) ) {
												CLSFIND = j;
											}
										}
										if ( CLSFIND >= 0 ) {
											s = xW_STATUS_IN_TM[0][ f ];
											x = xW_SOURCE_IN_TM[0][ f ];
											c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
											if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
												//-------------------------------------------------------------------
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , CLSFIND , f , s , 1 , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 864;
													return SYS_ABORTED;
												}
												_i_SCH_IO_MTL_SAVE();
												if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
													if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) {
														*rcode = 865;
														return SYS_ABORTED;
													}
												}
												if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
													if ( EQUIP_CLEAR_CHAMBER( CLSFIND , f , s ) == SYS_ABORTED ) {
														*rcode = 866;
														return SYS_ABORTED;
													}
												}
												if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , CLSFIND , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 867;
													return SYS_ABORTED;
												}
												_i_SCH_IO_MTL_SAVE();
												//-------------------------------------------------------------------
												b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
												if ( b == -1 ) {
													*rcode = 868;
													return SYS_ERROR;
												}
												//-------------------------------------------------------------------
												if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
													if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
														*rcode = 869;
														return SYS_ABORTED;
													}
												}
												//-------------------------------------------------------------------
												if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
													if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
														*rcode = 870;
														return SYS_ABORTED;
													}
													BM_WAIT[ c ] = 1;
												}
												else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
													//-------------------------------------------------------------------------------
													if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
														*rcode = 871;
														return SYS_ABORTED;
													}
													//------------------------------------------------------------------------------------
													BM_WAIT[ c ] = 1;
													//-----------------------------------------------------------------------------------
												}
												//-------------------------------------------------------------------
												if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
													*rcode = 872;
													return SYS_ABORTED;
												}
												//--------------------------------------------------------
												xW_STATUS_IN_TM[0][ f ] = 0;
												//--------------------------------------------------------
												_i_SCH_IO_MTL_SAVE();
												//--------------------------------------------------------
												switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
												case 0 : // No Action
													break;
												case 1 : // Skip
													clssw = 1;
													break;
												case 2 : // Vent
													EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
				//									BM_WAIT[ i ] = 3; // 2006.02.14
													BM_WAIT[ c ] = 3; // 2006.02.14
													break;
												}
												//--------------------------------------------------------
											}
											else {
												clssw = 1;
											}
										}
										else {
											*rcode = 873;
											return SYS_ERROR;
										}
									}
									else {
										clssw = 1;
									}
								}
							}
						}
						//--------------------------------------------------------------------------------------------------------
						// CLSOUT is Absent
						//--------------------------------------------------------------------------------------------------------
						else {
							//--------------------------------------------------------------------------------------------------------
							// Place BM with Arm A
							//--------------------------------------------------------------------------------------------------------
							if ( ( clssw == 0 ) && _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TA_STATION ) && ( xW_STATUS_IN_TM[0][ TA_STATION ] > 0 ) ) {
								if ( Finger != 1 ) {
									s = xW_STATUS_IN_TM[0][ TA_STATION ];
									x = xW_SOURCE_IN_TM[0][ TA_STATION ];
									c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
									if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
										//-------------------------------------------------------------------
										b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
										if ( b == -1 ) {
											*rcode = 874;
											return SYS_ERROR;
										}
										//-------------------------------------------------------------------
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
											if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
												*rcode = 875;
												return SYS_ABORTED;
											}
										}
										//-------------------------------------------------------------------
										if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
											if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
												*rcode = 876;
												return SYS_ABORTED;
											}
											BM_WAIT[ c ] = 1;
										}
										else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
											//-------------------------------------------------------------------------------
											if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
												*rcode = 877;
												return SYS_ABORTED;
											}
											//------------------------------------------------------------------------------------
											BM_WAIT[ c ] = 1;
											//-----------------------------------------------------------------------------------
										}
										//-------------------------------------------------------------------
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TA_STATION , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 878;
											return SYS_ABORTED;
										}
										//--------------------------------------------------------
										xW_STATUS_IN_TM[0][ TA_STATION ] = 0;
										//--------------------------------------------------------
										_i_SCH_IO_MTL_SAVE();
										//--------------------------------------------------------
										switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
										case 0 : // No Action
											break;
										case 1 : // Skip
											clssw = 1;
											break;
										case 2 : // Vent
											EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
			//								BM_WAIT[ i ] = 3; // 2006.02.14
											BM_WAIT[ c ] = 3; // 2006.02.14
											break;
										}
										//--------------------------------------------------------
									}
									else {
										clssw = 1;
									}
								}
							}
							//--------------------------------------------------------------------------------------------------------
							// Place BM with Arm B
							//--------------------------------------------------------------------------------------------------------
							if ( ( clssw == 0 ) && _i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0,TB_STATION ) && ( xW_STATUS_IN_TM[0][ TB_STATION ] > 0 ) ) {
								if ( Finger != 0 ) {
									s = xW_STATUS_IN_TM[0][ TB_STATION ];
									x = xW_SOURCE_IN_TM[0][ TB_STATION ];
									c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
									if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
										//-------------------------------------------------------------------
										b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
										if ( b == -1 ) {
											*rcode = 879;
											return SYS_ERROR;
										}
										//-------------------------------------------------------------------
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
											if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
												*rcode = 880;
												return SYS_ABORTED;
											}
										}
										//-------------------------------------------------------------------
										if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
											if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
												*rcode = 881;
												return SYS_ABORTED;
											}
											BM_WAIT[ c ] = 1;
										}
										else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
											//-------------------------------------------------------------------------------
											if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
												*rcode = 882;
												return SYS_ABORTED;
											}
											//------------------------------------------------------------------------------------
											BM_WAIT[ c ] = 1;
											//-----------------------------------------------------------------------------------
										}
										//-------------------------------------------------------------------
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , TB_STATION , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 883;
											return SYS_ABORTED;
										}
										//--------------------------------------------------------
										xW_STATUS_IN_TM[0][ TB_STATION ] = 0;
										//--------------------------------------------------------
										_i_SCH_IO_MTL_SAVE();
										//--------------------------------------------------------
										switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
										case 0 : // No Action
											break;
										case 1 : // Skip
											clssw = 1;
											break;
										case 2 : // Vent
											EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
			//								BM_WAIT[ i ] = 3; // 2006.02.14
											BM_WAIT[ c ] = 3; // 2006.02.14
											break;
										}
										//--------------------------------------------------------
									}
									else {
										clssw = 1;
									}
								}
							}
							//--------------------------------------------------------------------------------------------------------
							// Pick PM + Place BM
							//--------------------------------------------------------------------------------------------------------
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								if ( ( clssw == 0 ) && ( Transfer_Enable_Check( i ) ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) ) {
									//----------------------------------------------------------------------------------------------------------------
									//if ( Finger == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i ); // 2004.08.19
									//else               f = Finger; // 2004.08.19
									//----------------------------------------------------------------------------------------------------------------
									s = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
									x = xW_SOURCE_IN_CHAMBER[ i ];
									c = Transfer_GET_CLEAR_BM_AT_SWITCH( x );
									//----------------------------------------------------------------------------------------------------------------
									// 2004.08.19
									//----------------------------------------------------------------------------------------------------------------
									Finger2 = Finger;
									if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( &Finger2 , 0 , 0 , i , c ) ) {
										*rcode = 884;
										return SYS_ABORTED;
									}
									if ( Finger2 == 2 ) f = Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( 0 , i );
									else                f = Finger2;
									//----------------------------------------------------------------------------------------------------------------
									if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) { // 2003.11.10
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i ) ) {
											if ( EQUIP_CLEAR_CHAMBER( i , f , s ) == SYS_ABORTED ) {
												*rcode = 885;
												return SYS_ABORTED;
											}
										}
										if ( EQUIP_PICK_FROM_CHAMBER_TT( 0 , TR_CHECKING_SIDE , i , f , s , 1 , FALSE , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 886;
											return SYS_ABORTED;
										}
										_i_SCH_IO_MTL_SAVE();
										//-------------------------------------------------------------------
										b = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , s , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( x ) % 2 );
										if ( b == -1 ) {
											*rcode = 887;
											return SYS_ERROR;
										}
										//-------------------------------------------------------------------
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( c ) ) {
											if ( EQUIP_CLEAR_CHAMBER( c , s , 0 ) == SYS_ABORTED ) {
												*rcode = 888;
												return SYS_ABORTED;
											}
										}
										//-------------------------------------------------------------------
										if ( BM_WAIT[ c ] == 0 ) { // 2003.11.10
											if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
												*rcode = 889;
												return SYS_ABORTED;
											}
											BM_WAIT[ c ] = 1;
										}
										else if ( BM_WAIT[ c ] == 2 ) { // 2005.10.11
											//-------------------------------------------------------------------------------
											if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , c ) != SYS_SUCCESS ) {
												*rcode = 890;
												return SYS_ABORTED;
											}
											//------------------------------------------------------------------------------------
											BM_WAIT[ c ] = 1;
											//-----------------------------------------------------------------------------------
										}
										//-------------------------------------------------------------------
										if ( EQUIP_PLACE_TO_CHAMBER_TT( 0 , TR_CHECKING_SIDE , c , f , s , b , 0 , c , FALSE , 0 , 0 ) == SYS_ABORTED ) {
											*rcode = 891;
											return SYS_ABORTED;
										}
										//--------------------------------------------------------
										xW_STATUS_IN_CHAMBER[ i ][ 1 ] = 0;
										//--------------------------------------------------------
										_i_SCH_IO_MTL_SAVE();
										//--------------------------------------------------------
										switch( Transfer_GET_CLEAR_BM_CHECK_FOR_SPAWN_BM_AFTER_PLACE( c ) ) {
										case 0 : // No Action
											break;
										case 1 : // Skip
											clssw = 1;
											break;
										case 2 : // Vent
											EQUIP_SPAWN_WAITING_BM( TR_CHECKING_SIDE , c , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 );
			//								BM_WAIT[ i ] = 3; // 2006.02.14
											BM_WAIT[ c ] = 3; // 2006.02.14
											break;
										}
										//--------------------------------------------------------
									}
									else {
										clssw = 1;
									}
								}
							}
						}
					}
				}
				//-----------------------------------------------------------------------------------
				for ( i = 0 ; i < MAX_CHAMBER ; i++ ) BM_RUN[i] = 0;
				BM_ALL_RUN = 0;
				for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
					if ( Transfer_Enable_Check( i ) ) {
						for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
								BM_ALL_RUN++;
								BM_RUN[i]++;
							}
						}
					}
				}
				if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) BM_ALL_RUN++;
				if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) BM_ALL_RUN++;
				if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.10.28
					for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
						if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) BM_ALL_RUN++;
					}
				}
				if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) BM_ALL_RUN++;
				if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) BM_ALL_RUN++;
				if ( BM_ALL_RUN != 0 ) {
					if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() != 2 ) { // 2005.10.11
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							if ( BM_RUN[i] != 0 ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , i , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 892;
									return SYS_ABORTED;
								}
							}
						}
					}
					Find = FALSE; // 2006.10.11
					while (TRUE) {
						//==========================================================================================
						// 2006.10.11
						//==========================================================================================
						if ( Find ) {
							clssw = 1;
						}
						//==========================================================================================
						_i_SCH_IO_MTL_SAVE();
						Find = FALSE;
						//---------------------------------------------------------------------------
						// Place to FM
						//---------------------------------------------------------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.10.20
							Trg_Slot2 = 0;
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) {
								Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
								Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
								Find = TRUE;
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
									*rcode = 893;
									return SYS_ERROR;
								}
							}
							for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
								if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) {
									if ( Find ) {
										if ( Trg_Station != ( WAFER_SOURCE_IN_FM( 0 , 2 + a ) + CM1 ) ) {
											*rcode = 894;
											return SYS_ERROR;
										}
										if ( ( Trg_Slot + a + 1 ) != WAFER_STATUS_IN_FM( 0 ,2 + a ) ) {
											*rcode = 895;
											return SYS_ERROR;
										}
									}
									else {
										Trg_Station = WAFER_SOURCE_IN_FM( 0 , 2 + a ) + CM1;
										Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,2 + a ) - a - 1;
										Find = TRUE;
										if ( Trg_Slot <= 0 ) {
											*rcode = 896;
											return SYS_ERROR;
										}
									}
								}
							}
							if ( Find ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) {
									*rcode = 897;
									return SYS_ERROR;
								}
								for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot + a + 1 ) != CWM_ABSENT ) {
										*rcode = 898;
										return SYS_ERROR;
									}
								}
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
									*rcode = 899;
									return SYS_ABORTED;
								}
								if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
									*rcode = 901;
									return SYS_ABORTED;
								}
								//--------------------------------------------------------
								//--------------------------------------------------------
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
								for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
									Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot + a + 1 );
								}
								//--------------------------------------------------------
								continue;
							}
							//=================================================================================================
							// 2006.02.24
							//=================================================================================================
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
								Trg_Station = WAFER_SOURCE_IN_FM( 0 , TB_STATION ) + CM1;
								Trg_Slot  = 0;
								Trg_Slot2 = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
								Find = TRUE;
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , Trg_Slot2 ) ) {
									*rcode = 902;
									return SYS_ERROR;
								}
							}
							if ( Find ) {
								if ( ( Trg_Slot > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) ) {
									*rcode = 903;
									return SYS_ERROR;
								}
								if ( ( Trg_Slot2 > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) != CWM_ABSENT ) ) {
									*rcode = 904;
									return SYS_ERROR;
								}
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
									*rcode = 905;
									return SYS_ABORTED;
								}
								if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
									*rcode = 906;
									return SYS_ABORTED;
								}
								//--------------------------------------------------------
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
								//--------------------------------------------------------
								continue;
							}
							//=================================================================================================
						}
						else {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
									if ( WAFER_SOURCE_IN_FM( 0 , TA_STATION ) == WAFER_SOURCE_IN_FM( 0 , TB_STATION ) ) {
										Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
										Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
										Trg_Slot2   = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
										if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
											i = Trg_Slot;
											Trg_Slot = Trg_Slot2;
											Trg_Slot2 = i;
										}
										else if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION ,Trg_Slot2 ) ) {
											i = Trg_Slot;
											Trg_Slot = Trg_Slot2;
											Trg_Slot2 = i;
										}
										Find = TRUE;
									}
									else {
										Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
										Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
										Trg_Slot2   = 0;
										Find = TRUE;
									}
								}
								else {
									Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
									Trg_Slot  = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
									Trg_Slot2 = 0;
									Find = TRUE;
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
										*rcode = 907;
										return SYS_ERROR;
									}
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
								Trg_Station = WAFER_SOURCE_IN_FM( 0 , TB_STATION ) + CM1;
								Trg_Slot  = 0;
								Trg_Slot2 = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
								Find = TRUE;
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , Trg_Slot2 ) ) {
									*rcode = 908;
									return SYS_ERROR;
								}
							}

							if ( Find ) {
								if ( ( Trg_Slot > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) ) {
									*rcode = 909;
									return SYS_ERROR;
								}
								if ( ( Trg_Slot2 > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) != CWM_ABSENT ) ) {
									*rcode = 910;
									return SYS_ERROR;
								}
								if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
									*rcode = 911;
									return SYS_ABORTED;
								}
								if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
									*rcode = 912;
									return SYS_ABORTED;
								}
								//--------------------------------------------------------
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
								Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
								//--------------------------------------------------------
								continue;
							}
						}
						//---------------------------------------------------------------------------
						// Pick From FAL
						//---------------------------------------------------------------------------
						if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_AL_WAFER_SLOT() ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 913;
										return SYS_ABORTED;
									}
								}
								else {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 914;
										return SYS_ABORTED;
									}
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 915;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 916;
									return SYS_ABORTED;
								}
							}
							continue;
						}
						//---------------------------------------------------------------------------
						// Pick From FIC
						//---------------------------------------------------------------------------
						if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_IC_WAFER_SLOT() ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 917;
										return SYS_ABORTED;
									}
								}
								else {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 918;
										return SYS_ABORTED;
									}
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 919;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 920;
									return SYS_ABORTED;
								}
							}
							continue; // 2006.02.24
						}
						//---------------------------------------------------------------------------
						// Pick From BM(Place To FAL/FIC & Pick)
						//---------------------------------------------------------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.10.20
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( Transfer_Enable_Check( i ) ) {
									if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2006.02.24
										Trg_Slot = 0;
										Trg_Slot2 = 0;
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
												Trg_Station = i;
												Trg_Slot = j + 1;
												c = _i_SCH_IO_GET_MODULE_SOURCE( i , j + 1 );
												s = _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 );
												break;
											}
										}
										if ( Trg_Slot > 0 ) {
											Find = TRUE;
											for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
												if ( ( Trg_Slot + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) ) {
													Trg_Slot2 = Trg_Slot;
													Trg_Slot = 0;
													break;
												}
												if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot + a + 1 ) > 0 ) {
													if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot + a + 1 ) != ( s + a + 1 ) ) {
														Trg_Slot2 = Trg_Slot;
														Trg_Slot = 0;
														break;
													}
													if ( _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot + a + 1 ) != c ) {
														Trg_Slot2 = Trg_Slot;
														Trg_Slot = 0;
														break;
													}
												}
												if ( _i_SCH_IO_GET_MODULE_STATUS( c + CM1 , s + a + 1 ) != CWM_ABSENT ) {
													Trg_Slot2 = Trg_Slot;
													Trg_Slot = 0;
													break;
												}
											}
											break;
										}
									}
									else {
										Trg_Slot = 0;
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
												Trg_Slot = j + 1;
												c = _i_SCH_IO_GET_MODULE_SOURCE( i , Trg_Slot ); // 2006.02.24
												s = _i_SCH_IO_GET_MODULE_STATUS( i , Trg_Slot ); // 2006.02.24
												break; // 2003.10.28
											}
										}
										if ( Trg_Slot > 0 ) {
											Trg_Slot2 = Trg_Slot;
											Trg_Station = i;
											i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
											Find = TRUE;
											if ( ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + Trg_Slot ) > _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) ) {
												Trg_Slot = _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) - _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
											}
											//=============================================================================================
											// 2006.02.24
											//=============================================================================================
											for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
												if ( ( Trg_Slot2 + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) ) break;
												if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 + a + 1 ) > 0 ) {
													if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 + a + 1 ) != ( s + a + 1 ) ) {
														*rcode = 921;
														return SYS_ERROR;
													}
													if ( _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 + a + 1 ) != c ) {
														*rcode = 922;
														return SYS_ERROR;
													}
												}
											}
											//=============================================================================================
											break;
										}
									}
								}
							}
							if ( Find ) {
								//------------------------------------------------------------------------------------
								// 2005.10.11
								//------------------------------------------------------------------------------------
								if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 2 ) {
									if      ( BM_WAIT[ Trg_Station ] == 2 ) {
										*rcode = 923;
										return SYS_ERROR;
									}
									else if ( BM_WAIT[ Trg_Station ] == 3 ) {
										//-------------------------------------------------------------------------------
										if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , Trg_Station ) != SYS_SUCCESS ) {
											*rcode = 924;
											return SYS_ABORTED;
										}
										//------------------------------------------------------------------------------------
										BM_WAIT[ Trg_Station ] = 0;
									}
									else {
										if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
											*rcode = 925;
											return SYS_ABORTED;
										}
										BM_WAIT[ Trg_Station ] = 0;
									}
								}
								//-----------------------------------------------------------------------------------
								// 2006.02.14
								//-------------------------------------------------------------------------------
								if ( _i_SCH_PRM_GET_CLSOUT_USE( Trg_Station ) == 1 ) {
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
										*rcode = 926;
										return SYS_ABORTED;
									}
								}
								//-------------------------------------------------------------------------------
								if ( Trg_Slot <= 0 ) { // 2006.02.24
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
										if ( EQUIP_CLEAR_CHAMBER( Trg_Station , 0 , Trg_Slot2 ) == SYS_ABORTED ) {
											*rcode = 927;
											return SYS_ABORTED;
										}
									}
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot2 , TRUE , i , 0 ) == SYS_ABORTED ) {
										*rcode = 928;
										return SYS_ABORTED;
									}
									_i_SCH_IO_MTL_SAVE();
									//
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot2 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 929;
										return SYS_ABORTED;
									}
								}
								else {
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
										if ( EQUIP_CLEAR_CHAMBER( Trg_Station , Trg_Slot , 0 ) == SYS_ABORTED ) {
											*rcode = 930;
											return SYS_ABORTED;
										}
									}
									if ( Trg_Slot2 > 0 ) i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 ) + CM1;
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , i , 0 ) == SYS_ABORTED ) {
										*rcode = 931;
										return SYS_ABORTED;
									}
									_i_SCH_IO_MTL_SAVE();
									//
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 932;
										return SYS_ABORTED;
									}
									//-------------------------------------------------------------------------------
								}
								continue;
							}
						}
						else {
							if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS(  _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) , 1 ) > 0 ) {
									Trg_Station = _SCH_COMMON_TRANSFER_OPTION( 2 , -1 );
									Trg_Slot  = 1;
									Trg_Slot2 = 0;
									Find = TRUE;
								}
							}
							else {
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									if ( Transfer_Enable_Check( i ) ) {
										Trg_Slot = 0;
										Trg_Slot2 = 0;
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
												if      ( ( Trg_Slot <= 0 ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) )
													Trg_Slot = j + 1;
												else if ( ( Trg_Slot2 <= 0 ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) )
													Trg_Slot2 = j + 1;
											}
										}
										if ( Trg_Slot > 0 || Trg_Slot2 > 0 ) {
											Trg_Station = i;
											i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
											Find = TRUE;
											break;
										}
									}
								}
							}
							if ( Find ) {
								//-------------------------------------------------------------------------------
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) ) ) {
										i = Trg_Slot;
										Trg_Slot = Trg_Slot2;
										Trg_Slot2 = i;
									}
									else if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) ) ) {
										i = Trg_Slot;
										Trg_Slot = Trg_Slot2;
										Trg_Slot2 = i;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) ) ) {
										*rcode = 933;
										return SYS_ERROR;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) ) ) {
										*rcode = 934;
										return SYS_ERROR;
									}
								}
								//------------------------------------------------------------------------------------
								// 2005.10.11
								//------------------------------------------------------------------------------------
								if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 2 ) {
									if      ( BM_WAIT[ Trg_Station ] == 2 ) {
										*rcode = 935;
										return SYS_ERROR;
									}
									else if ( BM_WAIT[ Trg_Station ] == 3 ) {
										//-------------------------------------------------------------------------------
										if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , Trg_Station ) != SYS_SUCCESS ) {
											*rcode = 936;
											return SYS_ABORTED;
										}
										//------------------------------------------------------------------------------------
										BM_WAIT[ Trg_Station ] = 0;
									}
									else {
										if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
											*rcode = 937;
											return SYS_ABORTED;
										}
										BM_WAIT[ Trg_Station ] = 0;
									}
								}
								//-----------------------------------------------------------------------------------
								// 2006.02.14
								//-------------------------------------------------------------------------------
								if ( _i_SCH_PRM_GET_CLSOUT_USE( Trg_Station ) == 1 ) {
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
										*rcode = 938;
										return SYS_ABORTED;
									}
								}
								//-------------------------------------------------------------------------------
								if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
									if ( EQUIP_CLEAR_CHAMBER( Trg_Station , Trg_Slot , Trg_Slot2 ) == SYS_ABORTED ) {
										*rcode = 939;
										return SYS_ABORTED;
									}
								}
								//
								if ( Trg_Slot2 > 0 ) {
									i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 ) + CM1;
									//
									switch ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i ) ) { // 2008.09.10
									case 1 :
										if ( Trg_Slot > 0 ) {
											Trg_Slot2 = 0;
										}
										else {
											Trg_Slot = Trg_Slot2;
											Trg_Slot2 = 0;
										}
										break;
									}
									//
								}
								//
								if ( Trg_Slot > 0 ) {
									i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot ) + CM1;
									//
									switch ( _i_SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( i ) ) { // 2008.09.10
									case 2 :
										if ( Trg_Slot2 > 0 ) {
											Trg_Slot = 0;
										}
										else {
											Trg_Slot2 = Trg_Slot;
											Trg_Slot  = 0;
										}
										break;
									}
									//
								}
								//
								if ( ( Trg_Slot > 0 ) || ( Trg_Slot2 > 0 ) ) { // 2008.09.10
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , i , 0 ) == SYS_ABORTED ) {
										*rcode = 940;
										return SYS_ABORTED;
									}
									_i_SCH_IO_MTL_SAVE();
									//
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 941;
										return SYS_ABORTED;
									}
								}
								continue;
							}
						}
						//---------------------------------------------------------------------------
						break;
					}
				}
				if ( clssw == 0 ) {
					for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
						if ( Transfer_Enable_Check( i + CM1 ) && _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i + CM1 ) ) {
							if ( EQUIP_CLEAR_CHAMBER( i + CM1 , 0 , 0 ) == SYS_ABORTED ) {
								*rcode = 942;
								return SYS_ABORTED;
							}
						}
					}
					break;
				}
			} // While
		} // Switch Mode End




		//========================================
		// Clear Action Group 4
		//========================================
		if (
			( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) &&
			( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_6_INLIGN_PM2_V1 ) &&
			( ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) == 0 ) || ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) ) // 2006.12.21
			) {
			BM_Place_Count = 0;
			while (TRUE) {
				_i_SCH_IO_MTL_SAVE();
				Find = FALSE;
				//=====================================================================
				// 2006.02.24
				//=====================================================================
				if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) == 0 ) { // 2006.02.24 // 2006.12.21
					if ( BM_Place_Count == 2 ) {
						if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
							BM_Place_Count = 0;
						}
						else {
							for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									if ( Transfer_Enable_Check( i ) ) {
										if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
											Trg_Slot = 0;
											if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
												Trg_Station = i;
												Trg_Slot = j;
												i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
												BM_Place_Count = 1;
												Find = TRUE;
												break;
											}
										}
									}
								}
							}
							if ( !Find ) BM_Place_Count = 0;
						}
					}
					else if ( BM_Place_Count == 1 ) {
						BM_Place_Count = 0;
					}
					Find = FALSE;
					if ( BM_Place_Count == 0 ) {
						//---------------------------------------------------------------------------
						// Place to FM
						//---------------------------------------------------------------------------
						if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
								if ( WAFER_SOURCE_IN_FM( 0 , TA_STATION ) == WAFER_SOURCE_IN_FM( 0 , TB_STATION ) ) {
									Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
									Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
									Trg_Slot2   = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
										i = Trg_Slot;
										Trg_Slot = Trg_Slot2;
										Trg_Slot2 = i;
									}
									else if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION ,Trg_Slot2 ) ) {
										i = Trg_Slot;
										Trg_Slot = Trg_Slot2;
										Trg_Slot2 = i;
									}
									Find = TRUE;
								}
								else {
									Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
									Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
									Trg_Slot2   = 0;
									Find = TRUE;
								}
							}
							else {
								Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
								Trg_Slot  = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
								Trg_Slot2 = 0;
								Find = TRUE;
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
									*rcode = 943;
									return SYS_ERROR;
								}
							}
						}
						else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
							Trg_Station = WAFER_SOURCE_IN_FM( 0 , TB_STATION ) + CM1;
							Trg_Slot  = 0;
							Trg_Slot2 = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
							Find = TRUE;
							if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , Trg_Slot2 ) ) {
								*rcode = 944;
								return SYS_ERROR;
							}
						}
						if ( Find ) {
							if ( ( Trg_Slot > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) ) {
								*rcode = 945;
								return SYS_ERROR;
							}
							if ( ( Trg_Slot2 > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) != CWM_ABSENT ) ) {
								*rcode = 946;
								return SYS_ERROR;
							}
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
								*rcode = 947;
								return SYS_ABORTED;
							}
							if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
								*rcode = 948;
								return SYS_ABORTED;
							}
							//--------------------------------------------------------
							Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
							Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
							//--------------------------------------------------------
							continue;
						}
						//---------------------------------------------------------------------------
						// Pick From FAL
						//---------------------------------------------------------------------------
						if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) {
							if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_AL_WAFER_SLOT() ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 949;
										return SYS_ABORTED;
									}
								}
								else {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 950;
										return SYS_ABORTED;
									}
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 951;
									return SYS_ABORTED;
								}
							}
							else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
								if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
									*rcode = 952;
									return SYS_ABORTED;
								}
							}
							continue;
						}
						//---------------------------------------------------------------------------
						// Pick From FIC
						//---------------------------------------------------------------------------
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_IC_WAFER_SLOT() ) ) {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 953;
											return SYS_ABORTED;
										}
									}
									else {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 954;
											return SYS_ABORTED;
										}
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 955;
										return SYS_ABORTED;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 956;
										return SYS_ABORTED;
									}
								}
								continue;
							}
						}
						else {
							if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_IC_WAFER_SLOT() ) ) {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 957;
											return SYS_ABORTED;
										}
									}
									else {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 958;
											return SYS_ABORTED;
										}
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 959;
										return SYS_ABORTED;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 960;
										return SYS_ABORTED;
									}
								}
								continue;
							}
						}
						//---------------------------------------------------------------------------
						// Pick From BM(Place To FAL/FIC & Pick)
						//---------------------------------------------------------------------------
						if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
							if ( _i_SCH_IO_GET_MODULE_STATUS( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) , 1 ) > 0 ) {
								Trg_Station = _SCH_COMMON_TRANSFER_OPTION( 2 , -1 );
								Trg_Slot  = 1;
								Trg_Slot2 = 0;
								b = 0;
								grp = -1;
								Find = TRUE;
							}
						}
						else {
							for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
										if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_S_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_S_MODE ) ) {
											for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
												if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
													if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
														Trg_Station = i;
														Trg_Slot = j + 1;
														f = TA_STATION;
														grp = p;
														b = 0;
														Find = TRUE;
														p = 99999;
														break;
													}
													if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
														Trg_Station = i;
														Trg_Slot = j + 1;
														f = TB_STATION;
														grp = p;
														b = 0;
														Find = TRUE;
														p = 99999;
														break;
													}
												}
											}
										}
										else {
											if ( p == 0 ) {
												Trg_Slot = 0;
												Trg_Slot2 = 0;
												for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
													if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
														if      ( ( Trg_Slot <= 0 ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) )
															Trg_Slot = j + 1;
														else if ( ( Trg_Slot2 <= 0 ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) )
															Trg_Slot2 = j + 1;
													}
												}
												if ( Trg_Slot > 0 || Trg_Slot2 > 0 ) {
													Trg_Station = i;
													i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
													grp = p - 1;
													b = 0;
													Find = TRUE;
													p = 99999;
													break;
												}
											}
											else {
												for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
													if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
														if ( _i_SCH_IO_GET_MODULE_SOURCE( i , j + 1 ) == MAX_CASSETTE_SIDE ) { // 2002.05.17
															if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) >= p ) {
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TA_STATION;
																	grp = p;
																	b = 1;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TB_STATION;
																	grp = p;
																	b = 1;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
															}
															else {
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p - 1 , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p - 1 ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p - 1 ][ TA_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TA_STATION;
																	grp = p - 1;
																	b = 0;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p - 1 , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p - 1 ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p - 1 ][ TB_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TB_STATION;
																	grp = p - 1;
																	b = 0;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
															}
														}
														else if ( _i_SCH_IO_GET_MODULE_SOURCE( i , j + 1 ) > MAX_CASSETTE_SIDE ) { // 2002.05.17
															s = _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 );
															if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) >= p ) {
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TA_STATION;
																	grp = p;
																	b = 1;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TB_STATION;
																	grp = p;
																	b = 1;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
															}
															else {
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p - 1 , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p - 1 ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p - 1 ][ TA_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TA_STATION;
																	grp = p - 1;
																	b = 0;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
																if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p - 1 , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p - 1 ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p - 1 ][ TB_STATION ] <= 0 ) ) ) {
																	Trg_Station = i;
																	Trg_Slot = j + 1;
																	f = TB_STATION;
																	grp = p - 1;
																	b = 0;
																	Find = TRUE;
																	p = 99999;
																	break;
																}
															}
														}
														else {
															if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p - 1 , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p - 1 ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p - 1 ][ TA_STATION ] <= 0 ) ) ) {
																Trg_Station = i;
																Trg_Slot = j + 1;
																f = TA_STATION;
																grp = p - 1;
																b = 0;
																Find = TRUE;
																p = 99999;
																break;
															}
															if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p - 1 , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p - 1 ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p - 1 ][ TB_STATION ] <= 0 ) ) ) {
																Trg_Station = i;
																Trg_Slot = j + 1;
																f = TB_STATION;
																grp = p - 1;
																b = 0;
																Find = TRUE;
																p = 99999;
																break;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
						if ( Find ) {
							if ( grp < 0 ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) ) ) {
										i = Trg_Slot;
										Trg_Slot = Trg_Slot2;
										Trg_Slot2 = i;
									}
									else if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) ) ) {
										i = Trg_Slot;
										Trg_Slot = Trg_Slot2;
										Trg_Slot2 = i;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) ) ) {
										*rcode = 961;
										return SYS_ERROR;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) ) ) {
										*rcode = 962;
										return SYS_ERROR;
									}
								}
								//-------- Switch Need 2001.07.05
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
									// For Go FEM-BM
									if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
										*rcode = 963;
										return SYS_ABORTED;
									}
								}
								//-------------------------------------------------------------------------------
								if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
									if ( EQUIP_CLEAR_CHAMBER( Trg_Station , Trg_Slot , Trg_Slot2 ) == SYS_ABORTED ) {
										*rcode = 964;
										return SYS_ABORTED;
									}
								}
								if ( Trg_Slot2 > 0 ) i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 ) + CM1;
								if ( Trg_Slot > 0 ) i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot ) + CM1;
								if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , i , 0 ) == SYS_ABORTED ) {
									*rcode = 965;
									return SYS_ABORTED;
								}
								_i_SCH_IO_MTL_SAVE();
								//
								if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
									if ( ( Trg_Slot > 0 ) && ( Trg_Slot2 > 0 ) ) {
										Trg_Slot = 1;
										Trg_Slot = 2;
									}
									else if ( Trg_Slot > 0 ) {
										Trg_Slot = 1;
									}
									else if ( Trg_Slot2 > 0 ) {
										Trg_Slot2 = 1;
									}
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 966;
										return SYS_ABORTED;
									}
								}
								else {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 967;
										return SYS_ABORTED;
									}
								}
							}
							else {
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
									// For Go FEM-BM
									if ( b == 0 ) {
										if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
											*rcode = 968;
											return SYS_ABORTED;
										}
									}
									else if ( b == 1 ) {
										if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
											*rcode = 969;
											return SYS_ABORTED;
										}
									}
								}
								//-------------------------------------------------------------------------------
								i         = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot );
								Trg_Slot2 = _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot );
								//
								if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
									if ( EQUIP_CLEAR_CHAMBER( Trg_Station , f , Trg_Slot ) == SYS_ABORTED ) {
										*rcode = 970;
										return SYS_ABORTED;
									}
								}
								if ( EQUIP_PICK_FROM_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , f , Trg_Slot2 , Trg_Slot , FALSE , i + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
									*rcode = 971;
									return SYS_ABORTED;
								}
								//=====================================
								xW_SOURCE_IN_TM[grp][ f ] = i;
								xW_STATUS_IN_TM[grp][ f ] = Trg_Slot2;
								xW_STATUS_IN_TM2[grp][ f ] = 0;
								//=====================================
								_i_SCH_IO_MTL_SAVE();
							}
							continue;
						}
					}
				}
				//---------------------------------------------------------------------------
				//---------------------------------------------------------------------------
				//---------------------------------------------------------------------------
				//---------------------------------------------------------------------------
				// Place to BM with Arm A
				//---------------------------------------------------------------------------
				for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p,TA_STATION ) && ( ( xW_STATUS_IN_TM[p][ TA_STATION ] > 0 ) || ( xW_STATUS_IN_TM2[p][ TA_STATION ] > 0 ) ) ) {
						if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
							//-------------------------------------------------------------------------
							// 2002.08.12
							//-------------------------------------------------------------------------
							if ( xW_SOURCE_IN_TM[p][ TA_STATION ] == MAX_CASSETTE_SIDE ) {
								if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) ||
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_OUT_MODE ) ) { // 2002.08.12
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) == p ) {
									grp = p;
									b = 2;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) || // 2002.08.12
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else if ( xW_SOURCE_IN_TM[p][ TA_STATION ] > MAX_CASSETTE_SIDE ) {
								s = xW_STATUS_IN_TM[p][ TA_STATION ];
								if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) ||
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_OUT_MODE ) ) { // 2002.08.12
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) == p ) {
									grp = p;
									Trg_Station = s - 1 + PM1;
									b = 3;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) || // 2002.08.12
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else {
								if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.02.24 // 2006.12.21
									Trg_Slot = 0;
									if ( xW_STATUS_IN_TM[p][ TA_STATION ] > 0 ) {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TA_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM[p][ TA_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TA_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 972;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
									else {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TA_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM2[p][ TA_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TA_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 973;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j - 1;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) || // 2002.08.12
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
						}
						else {
							if ( xW_SOURCE_IN_TM[p][ TA_STATION ] == MAX_CASSETTE_SIDE ) { // 2002.05.17
								if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) == p ) {
									grp = p;
									b = 2;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else if ( xW_SOURCE_IN_TM[p][ TA_STATION ] > MAX_CASSETTE_SIDE ) { // 2002.05.17
								s = xW_STATUS_IN_TM[p][ TA_STATION ];
								if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) == p ) {
									grp = p;
									Trg_Station = s - 1 + PM1;
									b = 3;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else {
								if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.02.24 // 2006.12.21
									Trg_Slot = 0;
									if ( xW_STATUS_IN_TM[p][ TA_STATION ] > 0 ) {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TA_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM[p][ TA_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TA_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 974;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
									else {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TA_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM2[p][ TA_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TA_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 975;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j - 1;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( Find ) {
					if ( b == 2 ) {
						DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
						if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) {
							*rcode = 976;
							return SYS_ABORTED;
						}
						if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , DMSKIP , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 977;
							return SYS_ABORTED;
						}
						//--------------------------------------------------------
						Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
						//--------------------------------------------------------
						xW_STATUS_IN_TM[grp][ TA_STATION ] = 0;
						xW_STATUS_IN_TM2[grp][ TA_STATION ] = 0;
					}
					else if ( b == 3 ) { // 2002.05.17
						for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) {
							if ( SDMSKIP[d] == Trg_Station ) break;
						}
						if ( d != SDMSKIPCOUNT ) {
							*rcode = 978;
							return SYS_ABORTED;
						}
						for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
							if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == Trg_Station ) break;
						}
						if ( d == MAX_SDUMMY_DEPTH ) {
							*rcode = 979;
							return SYS_ABORTED;
						}
						sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
						SDMSKIP[SDMSKIPCOUNT] = Trg_Station;
						SDMSKIPCOUNT++;
						if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
							*rcode = 980;
							return SYS_ABORTED;
						}
						if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , TA_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 981;
							return SYS_ABORTED;
						}
						//--------------------------------------------------------
						Transfer_Cass_Info_Operation_After_Place( Trg_Station , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
						//--------------------------------------------------------
						xW_STATUS_IN_TM[grp][ TA_STATION ] = 0;
						xW_STATUS_IN_TM2[grp][ TA_STATION ] = 0;
					}
					else {
						Trg_Slot2 = Trg_Slot + 1;
						if ( xW_STATUS_IN_TM[grp][ TA_STATION ] == 0 ) Trg_Slot = 0;
						if ( xW_STATUS_IN_TM2[grp][ TA_STATION ] == 0 ) Trg_Slot2 = 0;
						//-------- Switch Need 2001.07.05
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
							// For Go TM-BM
							if ( b == 0 ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 982;
									return SYS_ABORTED;
								}
							}
							else if ( b == 1 ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 983;
									return SYS_ABORTED;
								}
							}
						}
						//-------------------------------------------------------------------------------
						if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , TA_STATION , 1 , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , xW_SOURCE_IN_TM[grp][ TA_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 984;
							return SYS_ABORTED;
						}
						//-----------------------------------------------
						xW_STATUS_IN_TM[grp][ TA_STATION ] = 0;
						xW_STATUS_IN_TM2[grp][ TA_STATION ] = 0;
						//-----------------------------------------------
						if ( _i_SCH_PRM_GET_CLSOUT_USE( Trg_Station ) == 1 ) {
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
								*rcode = 985;
								return SYS_ABORTED;
							}
						}
					}
					BM_Place_Count = 2;
					continue;
				}
				//---------------------------------------------------------------------------
				// Place to BM with Arm B
				//---------------------------------------------------------------------------
				for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
					if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[p][ TB_STATION ] > 0 ) || ( xW_STATUS_IN_TM2[p][ TB_STATION ] > 0 ) ) ) {
						if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
							//-------------------------------------------------------------------------
							// 2002.08.12
							//-------------------------------------------------------------------------
							if ( xW_SOURCE_IN_TM[p][ TB_STATION ] == MAX_CASSETTE_SIDE ) {
								if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) ||
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_OUT_MODE ) ) { // 2002.08.12
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) == p ) {
									grp = p;
									b = 2;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) || // 2002.08.12
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else if ( xW_SOURCE_IN_TM[p][ TB_STATION ] >= MAX_CASSETTE_SIDE ) { // 2002.05.17
								s = xW_STATUS_IN_TM[p][ TB_STATION ];
								if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) ||
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_OUT_MODE ) ) { // 2002.08.12
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) == p ) {
									grp = p;
									Trg_Station = s - 1 + PM1;
									b = 3;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) || // 2002.08.12
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else {
								if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.02.24 // 2006.12.21
									Trg_Slot = 0;
									if ( xW_STATUS_IN_TM[p][ TB_STATION ] > 0 ) {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TB_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM[p][ TB_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TB_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 986;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
									else {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TB_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM2[p][ TB_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TB_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 987;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j - 1;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) || // 2002.08.12
												( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
						}
						else {
							if ( xW_SOURCE_IN_TM[p][ TB_STATION ] == MAX_CASSETTE_SIDE ) { // 2002.05.17
								if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER() ) == p ) {
									grp = p;
									b = 2;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else if ( xW_SOURCE_IN_TM[p][ TB_STATION ] >= MAX_CASSETTE_SIDE ) { // 2002.05.17
								s = xW_STATUS_IN_TM[p][ TB_STATION ];
								if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) > p ) {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p + 1 ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p + 1 , i ) == BUFFER_IN_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 1;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
								else if ( _i_SCH_PRM_GET_MODULE_GROUP( s - 1 + PM1 ) == p ) {
									grp = p;
									Trg_Station = s - 1 + PM1;
									b = 3;
									Find = TRUE;
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
							else {
								if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.02.24 // 2006.12.21
									Trg_Slot = 0;
									if ( xW_STATUS_IN_TM[p][ TB_STATION ] > 0 ) {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TB_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM[p][ TB_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TB_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 988;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
									else {
										c = Transfer_GET_CLEAR_BM_AT_SWITCH( xW_SOURCE_IN_TM[p][ TB_STATION ] );
										if ( Transfer_GET_GO_BM_POSSIBLE_CHECK( c ) ) {
											j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( c , xW_STATUS_IN_TM2[p][ TB_STATION ] , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( xW_SOURCE_IN_TM[p][ TB_STATION ] ) % 2 );
											if ( j == -1 ) {
												*rcode = 989;
												return SYS_ERROR;
											}
											Trg_Station = c;
											Trg_Slot = j - 1;
											grp = p;
											b = 0;
											Find = TRUE;
											break;
										}
									}
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
											if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
												Trg_Slot = 0;
												if ( Transfer_GET_FREE_BM_CHECK( ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) , i , &j , 0 ) ) {
													Trg_Station = i;
													Trg_Slot = j;
													grp = p;
													b = 0;
													Find = TRUE;
													i = ( MAX_BM_CHAMBER_DEPTH + BM1 );
													p = 9999;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				if ( Find ) {
					if ( b == 2 ) {
						DMSKIP = _i_SCH_PRM_GET_DUMMY_PROCESS_CHAMBER();
						if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( DMSKIP ) ) ) {
							*rcode = 990;
							return SYS_ABORTED;
						}
						if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , DMSKIP , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , _i_SCH_PRM_GET_DUMMY_PROCESS_SLOT() , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 991;
							return SYS_ABORTED;
						}
						//--------------------------------------------------------
						Transfer_Cass_Info_Operation_After_Place( DMSKIP , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
						//--------------------------------------------------------
						xW_STATUS_IN_TM[grp][ TB_STATION ] = 0;
						xW_STATUS_IN_TM2[grp][ TB_STATION ] = 0;
					}
					else if ( b == 3 ) { // 2002.05.17
						for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) {
							if ( SDMSKIP[d] == Trg_Station ) break;
						}
						if ( d != SDMSKIPCOUNT ) {
							*rcode = 992;
							return SYS_ABORTED;
						}
						for ( d = 0 ; d < MAX_SDUMMY_DEPTH ; d++ ) {
							if ( _i_SCH_PRM_GET_SDUMMY_PROCESS_CHAMBER( d ) == Trg_Station ) break;
						}
						if ( d == MAX_SDUMMY_DEPTH ) {
							*rcode = 993;
							return SYS_ABORTED;
						}
						sl = _i_SCH_PRM_GET_SDUMMY_PROCESS_SLOT( d );
						SDMSKIP[SDMSKIPCOUNT] = Trg_Station;
						SDMSKIPCOUNT++;
						if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
							*rcode = 994;
							return SYS_ABORTED;
						}
						if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , TB_STATION , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 , sl , 0 , 0 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 995;
							return SYS_ABORTED;
						}
						//--------------------------------------------------------
						Transfer_Cass_Info_Operation_After_Place( Trg_Station , _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() + 1 );
						//--------------------------------------------------------
						xW_STATUS_IN_TM[grp][ TB_STATION ] = 0;
						xW_STATUS_IN_TM2[grp][ TB_STATION ] = 0;
					}
					else {
						Trg_Slot2 = Trg_Slot + 1;
						if ( xW_STATUS_IN_TM[grp][ TB_STATION ] == 0 ) Trg_Slot = 0;
						if ( xW_STATUS_IN_TM2[grp][ TB_STATION ] == 0 ) Trg_Slot2 = 0;
						//-------- Switch Need 2001.07.05
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Trg_Station ) ) {
							// For Go TM-BM
							if ( b == 0 ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_TM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 996;
									return SYS_ABORTED;
								}
							}
							else if ( b == 1 ) {
								if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
									*rcode = 997;
									return SYS_ABORTED;
								}
							}
						}
						//-------------------------------------------------------------------------------
						if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , TB_STATION , 1 , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , xW_SOURCE_IN_TM[grp][ TB_STATION ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
							*rcode = 998;
							return SYS_ABORTED;
						}
						//-----------------------------------------------
						xW_STATUS_IN_TM[grp][ TB_STATION ] = 0;
						xW_STATUS_IN_TM2[grp][ TB_STATION ] = 0;
						//-----------------------------------------------
						if ( _i_SCH_PRM_GET_CLSOUT_USE( Trg_Station ) == 1 ) {
							if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
								*rcode = 999;
								return SYS_ABORTED;
							}
						}
					}
					BM_Place_Count = 2;
					continue;
				}
				//---------------------------------------------------------------------------
				// Pick From PM
				//---------------------------------------------------------------------------
				for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
					if ( CLSYES[p] >= 0 ) {
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.17
									if ( SDMSKIP[d] == i ) break;
								}
								if ( d != SDMSKIPCOUNT ) continue;
								if ( ( DMSKIP != i ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) && ( ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) || ( xW_STATUS_IN_CHAMBER[ i ][ 2 ] > 0 ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
										f = TA_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										if ( Src_Slot > 0 )	Trg_Slot = 1; else Trg_Slot = 0;
										Src_Slot2 = xW_STATUS_IN_CHAMBER[ i ][ 2 ];
										if ( Src_Slot2 > 0 )	Trg_Slot2 = 2; else Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										p = 9999;
										break;
									}
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
										f = TB_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										if ( Src_Slot > 0 )	Trg_Slot = 1; else Trg_Slot = 0;
										Src_Slot2 = xW_STATUS_IN_CHAMBER[ i ][ 2 ];
										if ( Src_Slot2 > 0 )	Trg_Slot2 = 2; else Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										p = 9999;
										break;
									}
								}
							}
						}
						else {
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.17
									if ( SDMSKIP[d] == i ) break;
								}
								if ( d != SDMSKIPCOUNT ) continue;
								if ( ( DMSKIP != i ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 1 ) ) {
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
										f = TA_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										Trg_Slot  = 1;
										Src_Slot2 = 0;
										Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										break;
									}
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
										f = TB_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										Trg_Slot  = 1;
										Src_Slot2 = 0;
										Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										break;
									}
								}
							}
						}
					}
				}
				if ( Find ) {
					if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
						if ( EQUIP_CLEAR_CHAMBER( Trg_Station , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) ) == SYS_ABORTED ) {
							*rcode = 1001;
							return SYS_ABORTED;
						}
					}
					if ( EQUIP_PICK_FROM_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , FALSE , xW_SOURCE_IN_CHAMBER[ Trg_Station ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 1002;
						return SYS_ABORTED;
					}
					//=====================================
					xW_SOURCE_IN_TM[grp][ f ] = xW_SOURCE_IN_CHAMBER[ Trg_Station ];
					xW_STATUS_IN_TM[grp][ f ] = xW_STATUS_IN_CHAMBER[ Trg_Station ][ 1 ];
					xW_STATUS_IN_TM2[grp][ f ] = xW_STATUS_IN_CHAMBER[ Trg_Station ][ 2 ];
					xW_STATUS_IN_CHAMBER[ Trg_Station ][ 1 ] = 0;
					xW_STATUS_IN_CHAMBER[ Trg_Station ][ 2 ] = 0;
					//=====================================
					BM_Place_Count = 2;
					continue;
				}
				for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
					if ( CLSYES[p] >= 0 ) {
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.17
									if ( SDMSKIP[d] == i ) break;
								}
								if ( d != SDMSKIPCOUNT ) continue;
								if ( ( DMSKIP != i ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) && ( ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) || ( xW_STATUS_IN_CHAMBER[ i ][ 2 ] > 0 ) ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 0 ) ) {
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
										f = TA_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										if ( Src_Slot > 0 )	Trg_Slot = 1; else Trg_Slot = 0;
										Src_Slot2 = xW_STATUS_IN_CHAMBER[ i ][ 2 ];
										if ( Src_Slot2 > 0 )	Trg_Slot2 = 2; else Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										CLSFIND = -1;
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( j ) == p ) && ( xW_STATUS_IN_CHAMBER[ j ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 1 ) ) {
												CLSFIND = j;
											}
										}
										if ( CLSFIND >= 0 ) {
											Find = TRUE;
											p = 9999;
										}
										break;
									}
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
										f = TB_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										if ( Src_Slot > 0 )	Trg_Slot = 1; else Trg_Slot = 0;
										Src_Slot2 = xW_STATUS_IN_CHAMBER[ i ][ 2 ];
										if ( Src_Slot2 > 0 )	Trg_Slot2 = 2; else Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										CLSFIND = -1;
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( j ) == p ) && ( xW_STATUS_IN_CHAMBER[ j ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 1 ) ) {
												CLSFIND = j;
											}
										}
										if ( CLSFIND >= 0 ) {
											Find = TRUE;
											p = 9999;
										}
										break;
									}
								}
							}
						}
						else {
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.17
									if ( SDMSKIP[d] == i ) break;
								}
								if ( d != SDMSKIPCOUNT ) continue;
								if ( ( DMSKIP != i ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( i ) == 0 ) ) {
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
										f = TA_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										Trg_Slot  = 1;
										Src_Slot2 = 0;
										Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										CLSFIND = -1;
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( j ) == p ) && ( xW_STATUS_IN_CHAMBER[ j ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 1 ) ) {
												CLSFIND = j;
											}
										}
										if ( CLSFIND >= 0 ) {
											Find = TRUE;
											p = 9999;
										}
										break;
									}
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
										f = TB_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										Trg_Slot  = 1;
										Src_Slot2 = 0;
										Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										CLSFIND = -1;
										for ( j = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; j >= PM1 ; j-- ) {
											if ( ( Transfer_Enable_Check( j ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( j ) == p ) && ( xW_STATUS_IN_CHAMBER[ j ][ 1 ] <= 0 ) && ( _i_SCH_PRM_GET_CLSOUT_USE( j ) == 1 ) ) {
												CLSFIND = j;
											}
										}
										if ( CLSFIND >= 0 ) {
											Find = TRUE;
											p = 9999;
										}
										break;
									}
								}
							}
						}
					}
				}
				if ( Find ) {
					if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
						if ( EQUIP_CLEAR_CHAMBER( Trg_Station , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) ) == SYS_ABORTED ) {
							*rcode = 1003;
							return SYS_ABORTED;
						}
					}
					if ( EQUIP_PICK_FROM_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , FALSE , xW_SOURCE_IN_CHAMBER[ Trg_Station ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 1004;
						return SYS_ABORTED;
					}
					if ( EQUIP_PLACE_TO_CHAMBER_TT( grp , TR_CHECKING_SIDE , CLSFIND , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , 0 , xW_SOURCE_IN_CHAMBER[ Trg_Station ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 1005;
						return SYS_ABORTED;
					}
					if ( _i_SCH_PRM_GET_CLSOUT_USE( CLSFIND ) == 1 ) {
						if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( CLSFIND ) ) ) {
							*rcode = 1006;
							return SYS_ABORTED;
						}
					}
					if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( CLSFIND ) ) {
						if ( EQUIP_CLEAR_CHAMBER( CLSFIND , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) ) == SYS_ABORTED ) {
							*rcode = 1007;
							return SYS_ABORTED;
						}
					}
					if ( EQUIP_PICK_FROM_CHAMBER_TT( grp , TR_CHECKING_SIDE , CLSFIND , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , FALSE , xW_SOURCE_IN_CHAMBER[ Trg_Station ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 1008;
						return SYS_ABORTED;
					}
					//=====================================
					xW_SOURCE_IN_TM[grp][ f ] = xW_SOURCE_IN_CHAMBER[ Trg_Station ];
					xW_STATUS_IN_TM[grp][ f ] = xW_STATUS_IN_CHAMBER[ Trg_Station ][ 1 ];
					xW_STATUS_IN_TM2[grp][ f ] = xW_STATUS_IN_CHAMBER[ Trg_Station ][ 2 ];
					xW_STATUS_IN_CHAMBER[ Trg_Station ][ 1 ] = 0;
					xW_STATUS_IN_CHAMBER[ Trg_Station ][ 2 ] = 0;
					//=====================================
					BM_Place_Count = 2;
					continue;
				}
				for ( p = 0 ; p <= _i_SCH_PRM_GET_DFIM_MAX_MODULE_GROUP() ; p++ ) {
					if ( CLSYES[p] < 0 ) {
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.17
									if ( SDMSKIP[d] == i ) break;
								}
								if ( d != SDMSKIPCOUNT ) continue;
								if ( ( DMSKIP != i ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) && ( ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) || ( xW_STATUS_IN_CHAMBER[ i ][ 2 ] > 0 ) ) ) {
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
										f = TA_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										if ( Src_Slot > 0 )	Trg_Slot = 1; else Trg_Slot = 0;
										Src_Slot2 = xW_STATUS_IN_CHAMBER[ i ][ 2 ];
										if ( Src_Slot2 > 0 )	Trg_Slot2 = 2; else Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										p = 9999;
										break;
									}
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
										f = TB_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										if ( Src_Slot > 0 )	Trg_Slot = 1; else Trg_Slot = 0;
										Src_Slot2 = xW_STATUS_IN_CHAMBER[ i ][ 2 ];
										if ( Src_Slot2 > 0 )	Trg_Slot2 = 2; else Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										p = 9999;
										break;
									}
								}
							}
						}
						else {
							for ( i = ( PM1 + MAX_PM_CHAMBER_DEPTH ) - 1 ; i >= PM1 ; i-- ) {
								for ( d = 0 ; d < SDMSKIPCOUNT ; d++ ) { // 2002.05.17
									if ( SDMSKIP[d] == i ) break;
								}
								if ( d != SDMSKIPCOUNT ) continue;
								if ( ( DMSKIP != i ) && ( Transfer_Enable_Check( i ) ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) && ( xW_STATUS_IN_CHAMBER[ i ][ 1 ] > 0 ) ) {
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TA_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TA_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TA_STATION ] <= 0 ) ) ) {
										f = TA_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										Trg_Slot  = 1;
										Src_Slot2 = 0;
										Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										p = 9999;
										break;
									}
									if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( p , TB_STATION ) && ( ( xW_STATUS_IN_TM[ p ][ TB_STATION ] <= 0 ) && ( xW_STATUS_IN_TM2[ p ][ TB_STATION ] <= 0 ) ) ) {
										f = TB_STATION;
										Src_Slot  = xW_STATUS_IN_CHAMBER[ i ][ 1 ];
										Trg_Slot  = 1;
										Src_Slot2 = 0;
										Trg_Slot2 = 0;
										Trg_Station = i;
										i = -1;
										grp = p;
										Find = TRUE;
										p = 9999;
										break;
									}
								}
							}
						}
					}
				}
				if ( Find ) {
					if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
						if ( EQUIP_CLEAR_CHAMBER( Trg_Station , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) ) == SYS_ABORTED ) {
							*rcode = 1009;
							return SYS_ABORTED;
						}
					}
					if ( EQUIP_PICK_FROM_CHAMBER_TT( grp , TR_CHECKING_SIDE , Trg_Station , f , Transfer_GET_Slot_for_TM_Action( Src_Slot , Src_Slot2 ) , Transfer_GET_Slot_for_TM_Action( Trg_Slot , Trg_Slot2 ) , FALSE , xW_SOURCE_IN_CHAMBER[ Trg_Station ] + CM1 , FALSE , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 1010;
						return SYS_ABORTED;
					}
					//=====================================
					xW_SOURCE_IN_TM[grp][ f ] = xW_SOURCE_IN_CHAMBER[ Trg_Station ];
					xW_STATUS_IN_TM[grp][ f ] = xW_STATUS_IN_CHAMBER[ Trg_Station ][ 1 ];
					xW_STATUS_IN_TM2[grp][ f ] = xW_STATUS_IN_CHAMBER[ Trg_Station ][ 2 ];
					xW_STATUS_IN_CHAMBER[ Trg_Station ][ 1 ] = 0;
					xW_STATUS_IN_CHAMBER[ Trg_Station ][ 2 ] = 0;
					//=====================================
					BM_Place_Count = 2;
					continue;
				}
				//=====================================================================
				// 2006.02.24
				//=====================================================================
				if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.12.21
					//-----------------------------------------------------------------------------------
					BM_Place_Count = 0; // 2006.02.24
					//-----------------------------------------------------------------------------------
					for ( i = 0 ; i < MAX_CHAMBER ; i++ ) BM_RUN[i] = 0;
					BM_ALL_RUN = 0;
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( Transfer_Enable_Check( i ) ) {
							for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
									BM_ALL_RUN++;
									BM_RUN[i]++;
								}
							}
						}
					}
					if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) BM_ALL_RUN++;
					if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) BM_ALL_RUN++;
					if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.10.28
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) BM_ALL_RUN++;
						}
					}
					if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) BM_ALL_RUN++;
					if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) BM_ALL_RUN++;
					if ( BM_ALL_RUN != 0 ) {
						if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() != 2 ) { // 2005.10.11
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( BM_RUN[i] != 0 ) {
									if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , i , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
										*rcode = 1011;
										return SYS_ABORTED;
									}
								}
							}
						}
						while (TRUE) {
							_i_SCH_IO_MTL_SAVE();
							Find = FALSE;
							//---------------------------------------------------------------------------
							// Place to FM
							//---------------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.10.20
								Trg_Slot2 = 0;
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) {
									Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
									Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
									Find = TRUE;
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
										*rcode = 1012;
										return SYS_ERROR;
									}
								}
								for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
									if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) {
										if ( Find ) {
											if ( Trg_Station != ( WAFER_SOURCE_IN_FM( 0 , 2 + a ) + CM1 ) ) {
												*rcode = 1013;
												return SYS_ERROR;
											}
											if ( ( Trg_Slot + a + 1 ) != WAFER_STATUS_IN_FM( 0 ,2 + a ) ) {
												*rcode = 1014;
												return SYS_ERROR;
											}
										}
										else {
											Trg_Station = WAFER_SOURCE_IN_FM( 0 , 2 + a ) + CM1;
											Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,2 + a ) - a - 1;
											Find = TRUE;
											if ( Trg_Slot <= 0 ) {
												*rcode = 1015;
												return SYS_ERROR;
											}
										}
									}
								}
								if ( Find ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) {
										*rcode = 1016;
										return SYS_ERROR;
									}
									for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
										if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot + a + 1 ) != CWM_ABSENT ) {
											*rcode = 1017;
											return SYS_ERROR;
										}
									}
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
										*rcode = 1018;
										return SYS_ABORTED;
									}
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 1019;
										return SYS_ABORTED;
									}
									Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
									for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
										Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot + a + 1 );
									}
									//--------------------------------------------------------
									BM_Place_Count = 1; // 2006.02.24
									//--------------------------------------------------------
									continue;
								}
								//=================================================================================================
								// 2006.02.24
								//=================================================================================================
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
									Trg_Station = WAFER_SOURCE_IN_FM( 0 , TB_STATION ) + CM1;
									Trg_Slot  = 0;
									Trg_Slot2 = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
									Find = TRUE;
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , Trg_Slot2 ) ) {
										*rcode = 1020;
										return SYS_ERROR;
									}
								}
								if ( Find ) {
									if ( ( Trg_Slot > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) ) {
										*rcode = 1021;
										return SYS_ERROR;
									}
									if ( ( Trg_Slot2 > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) != CWM_ABSENT ) ) {
										*rcode = 1022;
										return SYS_ERROR;
									}
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
										*rcode = 1023;
										return SYS_ABORTED;
									}
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 1024;
										return SYS_ABORTED;
									}
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
									Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
									//--------------------------------------------------------
									//--------------------------------------------------------
									BM_Place_Count = 1; // 2006.02.24
									//--------------------------------------------------------
									continue;
								}
								//=================================================================================================
							}
							else {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TA_STATION ) > 0 ) ) {
									if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
										if ( WAFER_SOURCE_IN_FM( 0 , TA_STATION ) == WAFER_SOURCE_IN_FM( 0 , TB_STATION ) ) {
											Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
											Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
											Trg_Slot2   = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
											if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
												i = Trg_Slot;
												Trg_Slot = Trg_Slot2;
												Trg_Slot2 = i;
											}
											else if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION ,Trg_Slot2 ) ) {
												i = Trg_Slot;
												Trg_Slot = Trg_Slot2;
												Trg_Slot2 = i;
											}
											Find = TRUE;
										}
										else {
											Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
											Trg_Slot    = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
											Trg_Slot2   = 0;
											Find = TRUE;
										}
									}
									else {
										Trg_Station = WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1;
										Trg_Slot  = WAFER_STATUS_IN_FM( 0 ,TA_STATION );
										Trg_Slot2 = 0;
										Find = TRUE;
										if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , Trg_Slot ) ) {
											*rcode = 1025;
											return SYS_ERROR;
										}
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) && ( WAFER_STATUS_IN_FM( 0 ,TB_STATION ) > 0 ) ) {
									Trg_Station = WAFER_SOURCE_IN_FM( 0 , TB_STATION ) + CM1;
									Trg_Slot  = 0;
									Trg_Slot2 = WAFER_STATUS_IN_FM( 0 ,TB_STATION );
									Find = TRUE;
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , Trg_Slot2 ) ) {
										*rcode = 1026;
										return SYS_ERROR;
									}
								}

								if ( Find ) {
									if ( ( Trg_Slot > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) ) {
										*rcode = 1027;
										return SYS_ERROR;
									}
									if ( ( Trg_Slot2 > 0 ) && ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) != CWM_ABSENT ) ) {
										*rcode = 1028;
										return SYS_ERROR;
									}
									if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
										*rcode = 1029;
										return SYS_ABORTED;
									}
									if ( _i_SCH_EQ_PLACE_TO_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , Trg_Station , 0 ) == SYS_ABORTED ) {
										*rcode = 1030;
										return SYS_ABORTED;
									}
									//--------------------------------------------------------
									Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot );
									Transfer_Cass_Info_Operation_After_Place( Trg_Station , Trg_Slot2 );
									//--------------------------------------------------------
									BM_Place_Count = 1; // 2006.02.24
									//--------------------------------------------------------
									continue;
								}
							}
							//---------------------------------------------------------------------------
							// Pick From FAL
							//---------------------------------------------------------------------------
							if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_AL_WAFER_SLOT() ) ) {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 1031;
											return SYS_ABORTED;
										}
									}
									else {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 1032;
											return SYS_ABORTED;
										}
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 1 , 0 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 1033;
										return SYS_ABORTED;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( AL , 0 , 1 , FM_AL_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 1034;
										return SYS_ABORTED;
									}
								}
								//--------------------------------------------------------
								BM_Place_Count = 1; // 2006.02.24
								//--------------------------------------------------------
								continue;
							}
							//---------------------------------------------------------------------------
							// Pick From FIC
							//---------------------------------------------------------------------------
							if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &Trg_Slot ) ) {
								if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , FM_IC_WAFER_SLOT() ) ) {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 1035;
											return SYS_ABORTED;
										}
									}
									else {
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
											*rcode = 1036;
											return SYS_ABORTED;
										}
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 1 , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 1037;
										return SYS_ABORTED;
									}
								}
								else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , 1 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PICK ) == SYS_ABORTED ) {
										*rcode = 1038;
										return SYS_ABORTED;
									}
								}
								//--------------------------------------------------------
								BM_Place_Count = 1; // 2006.02.24
								//--------------------------------------------------------
								continue; // 2006.02.24
							}
							//---------------------------------------------------------------------------
							// Pick From BM(Place To FAL/FIC & Pick)
							//---------------------------------------------------------------------------
							if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.10.20
								for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
									if ( Transfer_Enable_Check( i ) ) {
										if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2006.02.24
											Trg_Slot = 0;
											Trg_Slot2 = 0;
											for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
												if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
													Trg_Station = i;
													Trg_Slot = j + 1;
													c = _i_SCH_IO_GET_MODULE_SOURCE( i , j + 1 );
													s = _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 );
													break;
												}
											}
											if ( Trg_Slot > 0 ) {
												Find = TRUE;
												for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
													if ( ( Trg_Slot + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) ) {
														Trg_Slot2 = Trg_Slot;
														Trg_Slot = 0;
														break;
													}
													if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot + a + 1 ) > 0 ) {
														if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot + a + 1 ) != ( s + a + 1 ) ) {
															Trg_Slot2 = Trg_Slot;
															Trg_Slot = 0;
															break;
														}
														if ( _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot + a + 1 ) != c ) {
															Trg_Slot2 = Trg_Slot;
															Trg_Slot = 0;
															break;
														}
													}
													if ( _i_SCH_IO_GET_MODULE_STATUS( c + CM1 , s + a + 1 ) != CWM_ABSENT ) {
														Trg_Slot2 = Trg_Slot;
														Trg_Slot = 0;
														break;
													}
												}
												break;
											}
										}
										else {
											Trg_Slot = 0;
											for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
												if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
													Trg_Slot = j + 1;
													c = _i_SCH_IO_GET_MODULE_SOURCE( i , Trg_Slot ); // 2006.02.24
													s = _i_SCH_IO_GET_MODULE_STATUS( i , Trg_Slot ); // 2006.02.24
													break; // 2003.10.28
												}
											}
											if ( Trg_Slot > 0 ) {
												Trg_Slot2 = Trg_Slot;
												Trg_Station = i;
												i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
												Find = TRUE;
												if ( ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) + Trg_Slot ) > _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) ) {
													Trg_Slot = _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) - _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
												}
												//=============================================================================================
												// 2006.02.24
												//=============================================================================================
												for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
													if ( ( Trg_Slot2 + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( Trg_Station ) ) break;
													if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 + a + 1 ) > 0 ) {
														if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 + a + 1 ) != ( s + a + 1 ) ) {
															*rcode = 1039;
															return SYS_ERROR;
														}
														if ( _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 + a + 1 ) != c ) {
															*rcode = 1040;
															return SYS_ERROR;
														}
													}
												}
												//=============================================================================================
												break;
											}
										}
									}
								}
								if ( Find ) {
									//------------------------------------------------------------------------------------
									// 2005.10.11
									//------------------------------------------------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 2 ) {
										if      ( BM_WAIT[ Trg_Station ] == 2 ) {
											*rcode = 1041;
											return SYS_ERROR;
										}
										else if ( BM_WAIT[ Trg_Station ] == 3 ) {
											//-------------------------------------------------------------------------------
											if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , Trg_Station ) != SYS_SUCCESS ) {
												*rcode = 1042;
												return SYS_ABORTED;
											}
											//------------------------------------------------------------------------------------
											BM_WAIT[ Trg_Station ] = 0;
										}
										else {
											if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
												*rcode = 1043;
												return SYS_ABORTED;
											}
											BM_WAIT[ Trg_Station ] = 0;
										}
									}
									//-----------------------------------------------------------------------------------
									// 2006.02.14
									//-------------------------------------------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_USE( Trg_Station ) == 1 ) {
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
											*rcode = 1044;
											return SYS_ABORTED;
										}
									}
									//-------------------------------------------------------------------------------
									if ( Trg_Slot <= 0 ) { // 2006.02.24
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
											if ( EQUIP_CLEAR_CHAMBER( Trg_Station , 0 , Trg_Slot2 ) == SYS_ABORTED ) {
												*rcode = 1045;
												return SYS_ABORTED;
											}
										}
										if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , 0 , Trg_Slot2 , TRUE , i , 0 ) == SYS_ABORTED ) {
											*rcode = 1046;
											return SYS_ABORTED;
										}
										_i_SCH_IO_MTL_SAVE();
										//
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , 0 , Trg_Slot2 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
											*rcode = 1047;
											return SYS_ABORTED;
										}
									}
									else {
										//-------------------------------------------------------------------------------
										if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
											if ( EQUIP_CLEAR_CHAMBER( Trg_Station , Trg_Slot , 0 ) == SYS_ABORTED ) {
												*rcode = 1048;
												return SYS_ABORTED;
											}
										}
										if ( Trg_Slot2 > 0 ) i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 ) + CM1;
										if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , 0 , TRUE , i , 0 ) == SYS_ABORTED ) {
											*rcode = 1049;
											return SYS_ABORTED;
										}
										_i_SCH_IO_MTL_SAVE();
										//
										if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , 0 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
											*rcode = 1050;
											return SYS_ABORTED;
										}
									}
									//--------------------------------------------------------
									BM_Place_Count = 1; // 2006.02.24
									//--------------------------------------------------------
									continue;
								}
							}
							else {
								if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) , 1 ) > 0 ) {
										Trg_Station = _SCH_COMMON_TRANSFER_OPTION( 2 , -1 );
										Trg_Slot  = 1;
										Trg_Slot2 = 0;
										Find = TRUE;
									}
								}
								else {
									for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
										if ( Transfer_Enable_Check( i ) ) {
											Trg_Slot = 0;
											Trg_Slot2 = 0;
											for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
												if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) > 0 ) {
													if      ( ( Trg_Slot <= 0 ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) )
														Trg_Slot = j + 1;
													else if ( ( Trg_Slot2 <= 0 ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) )
														Trg_Slot2 = j + 1;
												}
											}
											if ( Trg_Slot > 0 || Trg_Slot2 > 0 ) {
												Trg_Station = i;
												i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
												Find = TRUE;
												break;
											}
										}
									}
								}
								if ( Find ) {
									//-------------------------------------------------------------------------------
									if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
										if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) ) ) {
											i = Trg_Slot;
											Trg_Slot = Trg_Slot2;
											Trg_Slot2 = i;
										}
										else if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) ) ) {
											i = Trg_Slot;
											Trg_Slot = Trg_Slot2;
											Trg_Slot2 = i;
										}
									}
									else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) {
										if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) ) ) {
											*rcode = 1051;
											return SYS_ERROR;
										}
									}
									else if ( _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
										if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot2 ) ) ) {
											*rcode = 1052;
											return SYS_ERROR;
										}
									}
									//------------------------------------------------------------------------------------
									// 2005.10.11
									//------------------------------------------------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_BM_SINGLE_ONLY() == 2 ) {
										if      ( BM_WAIT[ Trg_Station ] == 2 ) {
											*rcode = 1053;
											return SYS_ERROR;
										}
										else if ( BM_WAIT[ Trg_Station ] == 3 ) {
											//-------------------------------------------------------------------------------
											if ( EQUIP_WAIT_PROCESS( TR_CHECKING_SIDE , Trg_Station ) != SYS_SUCCESS ) {
												*rcode = 1054;
												return SYS_ABORTED;
											}
											//------------------------------------------------------------------------------------
											BM_WAIT[ Trg_Station ] = 0;
										}
										else {
											if ( EQUIP_RUN_WAITING_BM( TR_CHECKING_SIDE , Trg_Station , BUFFER_RUN_WAITING_FOR_FM , -1 , -1 , FALSE ) == SYS_ABORTED ) {
												*rcode = 1055;
												return SYS_ABORTED;
											}
											BM_WAIT[ Trg_Station ] = 0;
										}
									}
									//-----------------------------------------------------------------------------------
									// 2006.02.14
									//-------------------------------------------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_USE( Trg_Station ) == 1 ) {
										if ( !WAIT_SECONDS( _i_SCH_PRM_GET_CLSOUT_DELAY( Trg_Station ) ) ) {
											*rcode = 1056;
											return SYS_ABORTED;
										}
									}
									//-------------------------------------------------------------------------------
									if ( _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( Trg_Station ) ) {
										if ( EQUIP_CLEAR_CHAMBER( Trg_Station , Trg_Slot , Trg_Slot2 ) == SYS_ABORTED ) {
											*rcode = 1057;
											return SYS_ABORTED;
										}
									}
									if ( Trg_Slot2 > 0 ) i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot2 ) + CM1;
									if ( Trg_Slot > 0 ) i = _i_SCH_IO_GET_MODULE_SOURCE( Trg_Station , Trg_Slot ) + CM1;
									if ( _i_SCH_EQ_PICK_FROM_FMBM( 0 , TR_CHECKING_SIDE , Trg_Station , Trg_Slot , Trg_Slot2 , TRUE , i , 0 ) == SYS_ABORTED ) {
										*rcode = 1058;
										return SYS_ABORTED;
									}
									_i_SCH_IO_MTL_SAVE();
									//
									if ( EQUIP_PLACE_AND_PICK_WITH_FOR_TR_FALIC( IC , Trg_Slot , Trg_Slot2 , FM_IC_WAFER_SOURCE() + CM1 , -1 , FAL_RUN_MODE_PLACE_MDL_PICK ) == SYS_ABORTED ) {
										*rcode = 1059;
										return SYS_ABORTED;
									}
									//--------------------------------------------------------
									BM_Place_Count = 1; // 2006.02.24
									//--------------------------------------------------------
									continue;
								}
							}
							//---------------------------------------------------------------------------
							break;
						} // while
					} // ( BM_ALL_RUN != 0 )
				} // Get_Prm_MODULE_BUFFER_SWITCH_MODE
				//==================================================================================================
				if ( BM_Place_Count == 1 ) continue;
				//==================================================================================================
				break;
				//==================================================================================================
			}
			for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
				if ( Transfer_Enable_Check( i + CM1 ) && _i_SCH_PRM_GET_CLSOUT_MESSAGE_USE( i + CM1 ) ) {
					if ( EQUIP_CLEAR_CHAMBER( i + CM1 , 0 , 0 ) == SYS_ABORTED ) {
						*rcode = 1060;
						return SYS_ABORTED;
					}
				}
			}
		}
	}
	return SYS_SUCCESS;
}
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
int Transfer_User_LL_Code( BOOL SDMMode , int Signal , int oppis , int opplc , int oppls , int *runfinger ) {
	int	 i , j;
	int  Error = 0;
	int Src_Station;
	int Trg_Station;
	int Src_Slot;
	int Trg_Slot;
	int Finger;
	int Mode;
	int Result;
	int tmatm; // 2006.02.23

	if ( SDMMode ) { // 2002.05.28
		Mode        = TR_MOVE;
		Src_Station = Signal;
		Src_Slot    = oppis;
		Trg_Station = opplc;
		Trg_Slot    = oppls;
		Finger      = 2;
	}
	else {
		EnterCriticalSection( &CR_MAIN );

		if ( BUTTON_GET_TR_CONTROL() != CTC_IDLE ) {
			MessageBeep(MB_ICONHAND);
			LeaveCriticalSection( &CR_MAIN );
			return SYS_ABORTED;
		}

		_i_SCH_SYSTEM_FLAG_RESET( TR_CHECKING_SIDE );

		BUTTON_SET_TR_CONTROL( CTC_DISABLE );
		BUTTON_HANDLER_REMOTE( "START TR" );

		Mode        = Signal;
		Src_Station = Transfer_IO_Reading( "CTC.TR_SRC_STATION" , &Address_TR_SRC_STATION ) + CM1;
		Src_Slot    = Transfer_IO_Reading( "CTC.TR_SRC_SLOT" , &Address_TR_SRC_SLOT );
		Trg_Station = Transfer_IO_Reading( "CTC.TR_TRG_STATION" , &Address_TR_TRG_STATION ) + CM1;
		Trg_Slot    = Transfer_IO_Reading( "CTC.TR_TRG_SLOT" , &Address_TR_TRG_SLOT );
		Finger      = Transfer_IO_Reading( "CTC.TR_FINGER" , &Address_TR_FINGER );
	}

	//=======================================================================================================================
	// 2006.02.23
	//=======================================================================================================================
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_PICK ) ) {
		if ( ( Src_Station >= CM1 ) && ( Src_Station < PM1 ) ) {
			tmatm = 0;
		}
		else if ( Src_Station >= PM1 ) {
			tmatm = -1;
			/*
			// 2006.04.06
			if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Src_Station ) >= 2 ) { // 2006.03.23
				if ( Transfer_Enable_Check( TM + ( Finger / 3 ) ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Src_Station , G_PICK ) ) {
						tmatm = ( Finger / 3 );
						Finger = Finger % 3;
					}
				}
			}
			if ( tmatm == -1 ) {
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					if ( Transfer_Enable_Check( TM + i ) ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Src_Station , G_PICK ) ) {
							tmatm = i;
							break;
						}
					}
				}
			}
			if ( tmatm == -1 ) Error = 101;
			*/
			if ( Finger != 2 ) {
				if ( Transfer_Enable_Check( TM + ( Finger / 3 ) ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Src_Station , G_PICK ) ) {
						tmatm = ( Finger / 3 );
						Finger = Finger % 3;
					}
				}
			}
			else {
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					if ( Transfer_Enable_Check( TM + i ) ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Src_Station , G_PICK ) ) {
							tmatm = i;
							break;
						}
					}
				}
			}
			if ( tmatm == -1 ) Error = 101;
		}
	}
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_PLACE ) ) {
		if ( ( Trg_Station >= CM1 ) && ( Trg_Station < PM1 ) ) {
			tmatm = 0;
		}
		else if ( Trg_Station >= PM1 ) {
			tmatm = -1;
			/*
			// 2006.04.06
			if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Trg_Station ) >= 2 ) { // 2006.03.23
				if ( Transfer_Enable_Check( TM + ( Finger / 3 ) ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Trg_Station , G_PLACE ) ) {
						tmatm = ( Finger / 3 );
						Finger = Finger % 3;
					}
				}
			}
			if ( tmatm == -1 ) {
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					if ( Transfer_Enable_Check( TM + i ) ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Trg_Station , G_PLACE ) ) {
							tmatm = i;
							break;
						}
					}
				}
			}
			if ( tmatm == -1 ) Error = 102;
			*/
			if ( Finger != 2 ) {
				if ( Transfer_Enable_Check( TM + ( Finger / 3 ) ) ) {
					if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Trg_Station , G_PLACE ) ) {
						tmatm = ( Finger / 3 );
						Finger = Finger % 3;
					}
				}
			}
			else {
				for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
					if ( Transfer_Enable_Check( TM + i ) ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Trg_Station , G_PLACE ) ) {
							tmatm = i;
							break;
						}
					}
				}
			}
			if ( tmatm == -1 ) Error = 102;
		}
	}
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_MOVE ) ) {
		if ( ( Src_Station >= CM1 ) && ( Src_Station < PM1 ) ) {
			if ( ( Trg_Station >= CM1 ) && ( Trg_Station < PM1 ) ) {
				tmatm = 0;
			}
			else if ( Trg_Station >= PM1 ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , Trg_Station , G_PLACE ) ) {
					tmatm = 0;
				}
				else {
					Error = 111;
				}
			}
		}
		else if ( Src_Station >= PM1 ) {
			if ( ( Trg_Station >= CM1 ) && ( Trg_Station < PM1 ) ) {
				if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( 0 , Src_Station , G_PICK ) ) {
					tmatm = 0;
				}
				else {
					Error = 111;
				}
			}
			else if ( Trg_Station >= PM1 ) {
				tmatm = -1;
				/*
				//====================================================================
				// 2006.03.23
				//====================================================================
				if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Src_Station ) >= 2 ) {
					if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , Trg_Station ) >= 2 ) {
						if ( Transfer_Enable_Check( TM + ( Finger / 3 ) ) ) {
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Src_Station , G_PICK ) ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Trg_Station , G_PLACE ) ) {
									tmatm = ( Finger / 3 );
									Finger = Finger % 3;
								}
							}
						}
					}
				}
				//====================================================================
				if ( tmatm == -1 ) {
					for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
						if ( Transfer_Enable_Check( TM + i ) ) {
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Src_Station , G_PICK ) ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Trg_Station , G_PLACE ) ) {
									tmatm = i;
									break;
								}
							}
						}
					}
				}
				if ( tmatm == -1 ) Error = 111;
				*/
				if ( Finger != 2 ) {
					if ( Transfer_Enable_Check( TM + ( Finger / 3 ) ) ) {
						if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Src_Station , G_PICK ) ) {
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( ( Finger / 3 ) , Trg_Station , G_PLACE ) ) {
								tmatm = ( Finger / 3 );
								Finger = Finger % 3;
							}
						}
					}
				}
				else {
					for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
						if ( Transfer_Enable_Check( TM + i ) ) {
							if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Src_Station , G_PICK ) ) {
								if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( i , Trg_Station , G_PLACE ) ) {
									tmatm = i;
									break;
								}
							}
						}
					}
				}
				if ( tmatm == -1 ) Error = 111;
			}
		}
	}
	//=============================================================================================================================
	//=============================================================================================================================
	//=============================================================================================================================
	//=======================================================================================================================
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_PICK ) ) {
		if ( Finger == 2 ) { // 2003.10.23
			Finger = 0;
			Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
			if ( Finger < 0 ) {
				Finger = 1;
				Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
				if ( Finger < 0 ) {
					Error = 101;
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Src_Station , Finger , -1 ) ) {
						Error = 101;
					}
				}
			}
			else {
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Src_Station , Finger , -1 ) ) {
					Finger = 1;
					Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
					if ( Finger < 0 ) {
						Error = 101;
					}
					else {
						if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Src_Station , Finger , -1 ) ) {
							Error = 101;
						}
					}
				}
			}
		}
		else {
			Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
			if ( Finger < 0 ) {
				Error = 101;
			}
			else { // 2003.10.17
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Src_Station , Finger , -1 ) ) {
					Error = 101;
				}
			}
		}
	}
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_PLACE ) ) {
		if ( Finger == 2 ) { // 2003.10.23
			Finger = 0;
			Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( tmatm , Finger );
			if ( Finger < 0 ) {
				Finger = 1;
				Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( tmatm , Finger );
				if ( Finger < 0 ) {
					Error = 102;
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Trg_Station , Finger , -1 ) ) {
						Error = 102;
					}
				}
			}
			else {
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Trg_Station , Finger , -1 ) ) {
					Finger = 1;
					Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( tmatm , Finger );
					if ( Finger < 0 ) {
						Error = 102;
					}
					else {
						if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Trg_Station , Finger , -1 ) ) {
							Error = 102;
						}
					}
				}
			}
		}
		else {
			Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( tmatm , Finger );
			if ( Finger < 0 ) {
				Error = 102;
			}
			else { // 2003.10.17
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( Mode , tmatm , Trg_Station , Finger , -1 ) ) {
					Error = 102;
				}
			}
		}
	}
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_MOVE ) ) {
		if ( Finger == 2 ) { // 2003.10.23
			Finger = 0;
			Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
			if ( Finger < 0 ) {
				Finger = 1;
				Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
				if ( Finger < 0 ) {
					Error = 103;
				}
				else { // 2003.10.17
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tmatm , Src_Station , Finger , -1 ) ) {
						Error = 103;
					}
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tmatm , Trg_Station , Finger , Src_Station ) ) {
						Error = 103;
					}
				}
			}
			else { // 2003.10.17
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tmatm , Src_Station , Finger , -1 ) ) {
					Finger = 1;
					Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
					if ( Finger < 0 ) {
						Error = 103;
					}
					else { // 2003.10.17
						if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tmatm , Src_Station , Finger , -1 ) ) {
							Error = 103;
						}
						if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tmatm , Trg_Station , Finger , Src_Station ) ) {
							Error = 103;
						}
					}
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tmatm , Trg_Station , Finger , Src_Station ) ) {
						Finger = 1;
						Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
						if ( Finger < 0 ) {
							Error = 103;
						}
						else { // 2003.10.17
							if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tmatm , Src_Station , Finger , -1 ) ) {
								Error = 103;
							}
							if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tmatm , Trg_Station , Finger , Src_Station ) ) {
								Error = 103;
							}
						}
					}
				}
			}
		}
		else {
			Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( tmatm , Finger );
			if ( Finger < 0 ) {
				Error = 103;
			}
			else { // 2003.10.17
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , tmatm , Src_Station , Finger , -1 ) ) {
					Error = 103;
				}
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , tmatm , Trg_Station , Finger , Src_Station ) ) {
					Error = 103;
				}
			}
		}
	}
	//=============================================================================================================================
	//=============================================================================================================================
	//=============================================================================================================================
	if ( ( Error == ERROR_NONE ) && ( ( Mode == TR_PICK ) || ( Mode == TR_MOVE ) ) ) {
		if ( ( Src_Station < CM1 ) || ( Src_Station >= AL ) ) {
			Error = 115;
		}
		else if ( Src_Station < PM1 ) {
			if ( tmatm != 0 ) { // 2006.02.23
				//===================================================
				Error = 111;
				//===================================================
			}
			else {
				if ( !Transfer_Enable_Check( Src_Station ) ) {
					Error = 111;
				}
				else if ( !Transfer_Enable_Check( TM + tmatm ) ) { // 2003.10.09
					Error = 130;
				}
				else { // 2004.01.30
					switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( Src_Station ) ) {
					case 4 :
					case 6 :
						if ( WAFER_STATUS_IN_TM( tmatm , TA_STATION ) > 0 ) {
							Error = 111;
						}
						if ( WAFER_STATUS_IN_TM( tmatm , TB_STATION ) > 0 ) {
							Error = 111;
						}
						break;
					}
				}
			}
		}
		else if ( Src_Station >= PM1 ) {
			if ( !Transfer_Enable_Check( Src_Station ) ) {
				Error = 111;
			}
			else if ( !Transfer_Enable_Check( TM + tmatm ) ) { // 2003.10.09
				Error = 130;
			}
		}
		if ( Error == ERROR_NONE ) {
			if ( Src_Station < PM1 ) {
				if ( ( Src_Slot <= 0 ) || ( Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) {
					Error = 112;
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( Src_Station , Src_Slot ) != CWM_PRESENT ) {
						Error = 113;
					}
				}
			}
			else {
				if ( _i_SCH_IO_GET_MODULE_STATUS( Src_Station , 1 ) <= 0 ) {
					Error = 114;
				}
			}
		}
	}
	//
	if ( ( Error == ERROR_NONE ) && ( ( Mode == TR_PLACE ) || ( Mode == TR_MOVE ) ) ) {
		if ( ( Trg_Station < CM1 ) || ( Trg_Station >= AL ) ) {
			Error = 125;
		}
		else if ( !Transfer_Enable_Check( Trg_Station ) ) {
			Error = 121;
		}
		else if ( !Transfer_Enable_Check( TM + tmatm ) ) { // 2003.10.09
			Error = 130;
		}
		if ( Error == ERROR_NONE ) {
			if ( Trg_Station < PM1 ) {
				if ( tmatm != 0 ) { // 2006.02.23
					//===================================================
					Error = 125;
					//===================================================
				}
				else {
					if ( ( Trg_Slot <= 0 ) || ( Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) {
						Error = 122;
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , Trg_Slot ) != CWM_ABSENT ) {
							Error = 123;
						}
					}
					//------------------------------------------------------------------------
					// Append 2002.02.28
					//------------------------------------------------------------------------
					if ( WAFER_STATUS_IN_TM( tmatm , TA_STATION ) > 0 ) {
						if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM( tmatm , TA_STATION ) + CM1 , Trg_Station ) ) Error = 121;
					}
					if ( WAFER_STATUS_IN_TM( tmatm , TB_STATION ) > 0 ) {
						if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM( tmatm , TB_STATION ) + CM1 , Trg_Station ) ) Error = 121;
					}
					if ( Error == ERROR_NONE ) { // 2004.01.30
						switch( _i_SCH_PRM_GET_INTERLOCK_CM_ROBOT_MULTI_DENY_FOR_ARM( Trg_Station ) ) {
						case 5 :
						case 6 :
							if ( Mode == TR_PLACE ) {
								if ( ( WAFER_STATUS_IN_TM( tmatm , TA_STATION ) > 0 ) && ( WAFER_STATUS_IN_TM( tmatm , TB_STATION ) > 0 ) ) {
									Error = 121;
								}
							}
							else if ( Mode == TR_MOVE ) {
								if ( WAFER_STATUS_IN_TM( tmatm , TA_STATION ) > 0 ) {
									Error = 121;
								}
								if ( WAFER_STATUS_IN_TM( tmatm , TB_STATION ) > 0 ) {
									Error = 121;
								}
							}
							break;
						}
					}
				}
			}
			else {
				if ( _i_SCH_IO_GET_MODULE_STATUS( Trg_Station , 1 ) > 0 ) {
					Error = 124;
				}
				//------------------------------------------------------------------------
				// Append 2002.05.20
				//------------------------------------------------------------------------
				if ( WAFER_STATUS_IN_TM( tmatm , TA_STATION ) > 0 ) {
					if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM( tmatm , TA_STATION ) + CM1 , Trg_Station ) ) Error = 121;
				}
				if ( WAFER_STATUS_IN_TM( tmatm , TB_STATION ) > 0 ) {
					if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM( tmatm , TB_STATION ) + CM1 , Trg_Station ) ) Error = 121;
				}
			}
		}
	}
	//=============================================================================================================================
	//=============================================================================================================================
	//=============================================================================================================================
	if ( ( Error == ERROR_NONE ) && ( Mode == TR_CLEAR ) ) {
		if ( !Transfer_Enable_Check( TM ) ) { // 2003.10.09
			Error = 130;
		}
		else {
			j = 0;
			if ( ( !Transfer_Enable_Check( IC ) && ( _i_SCH_IO_GET_MODULE_STATUS( IC , 1 ) > 0 ) ) ||
				 ( !Transfer_Enable_Check( AL ) && ( _i_SCH_IO_GET_MODULE_STATUS( AL , 1 ) > 0 ) ) ) {
				Error = 140;
			}
			else if ( ( Transfer_Enable_Check( IC ) && ( _i_SCH_IO_GET_MODULE_STATUS( IC , 1 ) > 0 ) ) ||
				 ( Transfer_Enable_Check( AL ) && ( _i_SCH_IO_GET_MODULE_STATUS( AL , 1 ) > 0 ) ) ) {
				j++;
				//=================
				//Finger = 2;
				//_dWRITE_DIGITAL( Address_TR_FINGER , Finger , &CommStatus );
				//=================
				if ( Finger == 0 ) {
					if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TA_STATION ) || Transfer_ROBOT_TR_FINGER_HAS_WAFER( 0 , TA_STATION ) ) {
						Error = 133;
					}
				}
				else if ( Finger == 1 ) {
					if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( 0 , TB_STATION ) || Transfer_ROBOT_TR_FINGER_HAS_WAFER( 0 , TB_STATION ) ) {
						Error = 134;
					}
				}
				else if ( Finger == 2 ) {
					if ( Transfer_ROBOT_GET_FINGER_FREE_COUNT( 0 ) <= 0 ) {
						Error = 135;
					}
				}
			}
			if ( ( Error == ERROR_NONE ) && ( j == 0 ) ) {
				for ( i = PM1 ; i < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; i++ ) {
					if ( Transfer_Enable_Check( i ) && _i_SCH_IO_GET_MODULE_STATUS( i , 1 ) > 0 ) j++;
				}
				if ( Finger == 0 ) {
					for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2006.03.27
						if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
							if ( Transfer_ROBOT_TR_FINGER_HAS_WAFER( i , TA_STATION ) ) {
								if ( !Transfer_Enable_Check( TM + i ) ) {
									Error = 133;
									break;
								}
								else if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TA_STATION ) ) {
									Error = 133;
									break;
								}
								else {
									j++;
								}
							}
						}
					}
				}
				else if ( Finger == 1 ) {
					for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2006.03.27
						if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
							if ( Transfer_ROBOT_TR_FINGER_HAS_WAFER( i , TB_STATION ) ) {
								if ( !Transfer_Enable_Check( TM + i ) ) {
									Error = 134;
									break;
								}
								else if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TB_STATION ) ) {
									Error = 134;
									break;
								}
								else {
									j++;
								}
							}
						}
					}
				}
				else if ( Finger == 2 ) {
					for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) { // 2006.03.27
						if ( _i_SCH_PRM_GET_MODULE_MODE( i + TM ) ) {
							if ( Transfer_ROBOT_TR_FINGER_HAS_WAFER( i , TA_STATION ) ) {
								if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TA_STATION ) ) {
									if ( !Transfer_Enable_Check( TM + i ) ) {
										Error = 133;
										break;
									}
									j++;
								}
							}
							if ( Transfer_ROBOT_TR_FINGER_HAS_WAFER( i , TB_STATION ) ) {
								if ( _i_SCH_ROBOT_GET_FINGER_HW_USABLE( i , TB_STATION ) ) {
									if ( !Transfer_Enable_Check( TM + i ) ) {
										Error = 134;
										break;
									}
									j++;
								}
							}
						}
					}
				}
				if ( ( Error == ERROR_NONE ) && ( j == 0 ) ) Error = 136;
			}
		}
	}
	else if ( Error == ERROR_NONE ) {
		if ( ( Transfer_Enable_Check( AL ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( AL , 1 ) > 0 ) ) {
			Error = 131;
		}
		if ( ( Transfer_Enable_Check( IC ) ) && ( _i_SCH_IO_GET_MODULE_STATUS( IC , 1 ) > 0 ) ) {
			Error = 132;
		}
	}
	//=============================================================================================================================
	//=============================================================================================================================
	//=============================================================================================================================
	if ( Error != ERROR_NONE ) {
		ERROR_HANDLER( Error , "" );
		if ( !SDMMode ) { // 2002.05.30
			BUTTON_SET_TR_CONTROL( CTC_IDLE );
			BUTTON_HANDLER_REMOTE( "STOP TR" );
			LeaveCriticalSection( &CR_MAIN );
		}
		return SYS_ABORTED;
	}

	if ( !SDMMode ) { // 2002.05.28
		BUTTON_SET_TR_CONTROL( CTC_RUNNING );
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			SYSTEM_CANCEL_DISABLE_SET( i , FALSE );
		}
	}
	*runfinger = Finger; // 2002.05.28
	Result = Transfer_User_Running_LL_Code( Mode , tmatm , Finger , Src_Station , Src_Slot , Trg_Station , Trg_Slot );
	_i_SCH_IO_MTL_SAVE();

	if ( !SDMMode ) { // 2002.05.30
		BUTTON_SET_TR_CONTROL( CTC_IDLE );
		BUTTON_HANDLER_REMOTE( "STOP TR" );
		LeaveCriticalSection( &CR_MAIN );
	}

	if ( Result == SYS_ABORTED ) {
		ERROR_HANDLER( ERROR_TRANSFER_STOP_BY_ABORT , "" );
		return SYS_ABORTED;
	}
	if ( Result == SYS_ERROR   ) {
		ERROR_HANDLER( ERROR_TRANSFER_STOP_BY_ERROR , "" );
		return SYS_ERROR;
	}
	return SYS_SUCCESS;
}
//===================================================================================================
//===================================================================================================
//===================================================================================================
int Transfer_Main_Checking_FEM( int Mode , int *Use , int *Finger , int *Src_Station , int *Src_Slot , int *Src_Slot2 , int *Trg_Station , int *Trg_Slot , int *Trg_Slot2 , int *BM_Station , int *BM_Slot , int *BM_Slot2 , int *BM_PSlot , int *BM_PSlot2 , int *st , int *et , int *DoArmMX ) {
	int	CommStatus , i , j , p , a;

	*DoArmMX = 0;

	if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
		*Src_Slot2 = 0;
		*Trg_Slot2 = 0;
		BM_Slot2[0]  = 0;
	}

	if ( Mode == TR_PICK ) {
		if  ( ( *Src_Station < CM1 ) || ( *Src_Station == AL ) || ( *Src_Station == IC ) || ( *Src_Station >= TM ) ) {
			return 116;
		}
		else if ( *Src_Station < PM1 ) {
			if ( !Transfer_Enable_Check( *Src_Station ) ) return 111;
		}
		else if ( *Src_Station >= BM1 ) {
			if ( !Transfer_Enable_Check( *Src_Station ) ) return 111;
		}
		else {
			if ( !Transfer_Enable_Check( *Src_Station ) ) return 111;
		}
		//
//		if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
//		if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21

		if ( *Src_Station < PM1 ) { // C:(D)2001.08.15
			//
			if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
			//
			*Use = 0;
			*Finger = ROBOT_TR_FM_GET_FINGER_FOR_USE( 0 , *Finger % 3 );
			if ( *Finger < 0 ) return 104;
			//==================================================================================
			if ( *Finger == 2 ) {
//				if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Src_Slot >= *Src_Slot2 ) ) return 112; // 2005.10.17
				if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112; // 2005.10.17
				if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , *Src_Slot ) || _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , *Src_Slot2 ) ) return 104;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) != CWM_PRESENT ) return 113;
			}
			else {
				if (
					( ( *Finger == 0 ) && _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , *Src_Slot ) ) ||
					( ( *Finger == 1 ) && _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , *Src_Slot ) )
					) {
					return 104;
				}
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) ) {
					}
					else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) ) {
						*Src_Slot = *Src_Slot2;
						*Src_Slot2 = 0;
					}
					else {
						return 112;
					}
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
				}
				else {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
					_dWRITE_DIGITAL( Address_TR_SRC_SLOT2 , 0 , &CommStatus );
					*Src_Slot2 = 0;
				}
			}
			//==================================================================================
			if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
				if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
					for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
						if ( WAFER_STATUS_IN_FM( 0 , 2 + a ) > 0 ) return 108;
						if ( ( (*Src_Slot) + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 108;
						switch( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , (*Src_Slot) + a + 1 ) ) {
						case CWM_PRESENT :
							*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
							break;
						case CWM_ABSENT :
							*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
							break;
						default : return 108;
						}
					}
				}
			}
			//==================================================================================
		}
		else if ( *Src_Station >= BM1 ) {
			if ( *Finger >= 3 ) { // FEM-BM // C:(D)2001.08.15
				//
				if (
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) ) {
					*Use = 0;
					*Finger = ROBOT_TR_FM_GET_FINGER_FOR_USE( 0 , *Finger % 3 );
					if ( *Finger < 0 ) return 104;
					if ( _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) == 1 ) {
						*Src_Slot = 1;
						*Src_Slot2 = 0;
					}
					if ( *Finger == 2 ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) || ( *Src_Slot >= *Src_Slot2 ) ) return 117;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 118;
					}
					else {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
						_dWRITE_DIGITAL( Address_TR_SRC_SLOT2 , 0 , &CommStatus );
						*Src_Slot2 = 0;
					}
					//==================================================================================
					if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
						if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
							for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
								if ( WAFER_STATUS_IN_FM( 0 , 2 + a ) > 0 ) return 108;
								if ( ( (*Src_Slot) + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) return 108;
								*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
							}
						}
					}
					//==================================================================================
				}
				else {
					if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station ) == BUFFER_IN_S_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station ) == BUFFER_OUT_S_MODE ) ) {
						return 104;
					}
					if ( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) <= 0 ) {
						*Use = 0;
						*Finger = ROBOT_TR_FM_GET_FINGER_FOR_USE( 0 , *Finger % 3 );
						if ( *Finger < 0 ) return 104;
						//==================================================================================
						if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) { // 2003.01.25
							 return 104;
						}
						//==================================================================================
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							if      ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) ) {
								if ( *Finger == 2 ) return 104;
								*Src_Slot = 2;
								*Src_Slot2 = 0;
							}
							else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) ) {
								if ( *Finger == 2 ) return 104;
								*Src_Slot = 1;
								*Src_Slot2 = 0;
							}
							else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) ) {
								if ( *Finger != 2 ) return 104;
								*Src_Slot = 1;
								*Src_Slot2 = 2;
							}
						}
						else {
							if ( _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) == 1 ) {
								*Src_Slot = 1;
								*Src_Slot2 = 0;
							}
						}
						if ( *Finger == 2 ) {
							if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) || ( *Src_Slot >= *Src_Slot2 ) ) return 117;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 118;
						}
						else {
							if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
							if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
								_dWRITE_DIGITAL( Address_TR_SRC_SLOT2 , 0 , &CommStatus );
								*Src_Slot2 = 0;
							}
						}
					}
					else {
						*Use = 0;
						*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) - 1 , *Finger % 3 );
						if ( *Finger < 0 ) return 101;
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
							if ( *Src_Slot  > 0 ) *Src_Slot = 1;
							if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
							if ( *Src_Slot  > 0 ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
							}
							else {
								if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) return 114;
							}
							if ( *Src_Slot2 > 0 ) {
								if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) return 114;
							}
							else {
								if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) return 114;
							}
						}
						else {
							if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
								*Src_Slot = 1;
								*Src_Slot2 = 0;
							}
							//
							if ( *Src_Slot <= 0 ) return 112;
							//
							*Src_Slot2 = 0;
							if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
						}
					}
					//--------------------------------------------------------------------------------------
// ??????????
					if ( *Finger == 0 ) { // 2003.10.21
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) return 108;
							if ( ( (*Src_Slot) + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) return 108;
							*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
						}
					}
					//--------------------------------------------------------------------------------------
				}
			}
			else { // LL-BM //C:(D)2001.08.23
				*Use = 1;
				if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == *Src_Station ) return 101; // 2002.09.12
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
				}
				else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == *Src_Station ) return 101; // 2002.09.12
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
				}
				else {
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
				}
				if ( *Finger < 0 ) return 101;
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
					if ( *Src_Slot  > 0 ) *Src_Slot = 1;
					if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
					if ( *Src_Slot  > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) return 114;
					}
					if ( *Src_Slot2 > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) return 114;
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) return 114;
					}
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
						*Src_Slot = 1;
						*Src_Slot2 = 0;
					}
					//
					if ( *Src_Slot <= 0 ) return 112;
					//
					*Src_Slot2 = 0;
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
				}
			}
		}
		else { //C:(D)2001.08.23
			*Use = 1;
			if ( *Finger == 2 ) { // 2003.10.17
				//-----------------------------------------------------------------------------------------------------
				*Finger = 0;
				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
				//-----------------------------------------------------------------------------------------------------
				if ( *Finger < 0 ) {
					*Finger = 1;
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
					if ( *Finger < 0 ) return 101;
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station , *Finger , -1 ) ) return 101; // 2003.10.17
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station , *Finger , -1 ) ) {
						*Finger = 1;
						*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
						if ( *Finger < 0 ) return 101;
						if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station , *Finger , -1 ) ) return 101; // 2003.10.17
					}
				}
			}
			else {
				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger );
				//-----------------------------------------------------------------------------------------------------
				if ( *Finger < 0 ) return 101; // 2003.08.11(bug)
				//-----------------------------------------------------------------------------------------------------
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station , *Finger , -1 ) ) return 101; // 2003.10.17
			}
			//-----------------------------------------------------------------------------------------------------
//			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) { // 2003.10.17
//				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger ); // 2003.10.17
//			} // 2003.10.17
//			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) { // 2003.10.17
//				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger ); // 2003.10.17
//			} // 2003.10.17
//			else { // 2003.10.17
//				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger ); // 2003.10.17
//			} // 2003.10.17
//			if ( *Finger < 0 ) return 101; // 2003.08.11(bug) // 2003.10.17
			//-----------------------------------------------------------------------------------------------------
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
				if ( *Src_Slot  > 0 ) *Src_Slot = 1;
				if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
				if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) ) {
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
				}
				else if ( *Src_Slot > 0 ) {
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) ) return 114;
				}
				else if ( *Src_Slot2 > 0 ) {
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
				}
				else {
					return 114;
				}
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
					*Src_Slot = 1;
					*Src_Slot2 = 0;
				}
				//
				if ( *Src_Slot <= 0 ) return 111;
				//
				*Src_Slot2 = 0;
				if ( *Src_Slot > 0 ) *Src_Slot = 1;
				if ( *Src_Slot <= 0 ) return 114;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
			}
		}
	}
	//----------------------------------------------------------------------------------------------------------------------------
	if ( Mode == TR_PLACE ) {
		if      ( ( *Trg_Station < CM1 ) || ( *Trg_Station == AL ) || ( *Trg_Station == IC ) || ( *Trg_Station >= TM ) ) {
			return 126;
		}
		else if ( !Transfer_Enable_Check( *Trg_Station ) ) {
			return 121;
		}
		//
//		if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
//		if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21
		//
		if ( *Trg_Station < PM1 ) { // C:(D)2001.08.15
			//
			if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21
			//
			*Use = 0;
			*Finger = ROBOT_TR_FM_PUT_FINGER_FOR_USE( 0 , *Finger % 3 );
			if ( *Finger < 0 ) return 105;
			//==================================================================================
//			if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) ) { // 2003.01.25 // 2003.04.29
//				 return 105;
//			}
			//==================================================================================
			if ( *Finger == 2 ) {
//				if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Trg_Slot >= *Trg_Slot2 ) ) return 122; // 2005.10.17
				if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122; // 2005.10.17
				if ( _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , *Trg_Slot ) || _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , *Trg_Slot2 ) ) return 105;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) != CWM_ABSENT ) return 123;
			}
			else {
				if (
					( ( *Finger == 0 ) && _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TA_STATION , *Trg_Slot ) ) ||
					( ( *Finger == 1 ) && _SCH_SUB_FM_ROBOT_CHECK_INTERLOCK( TB_STATION , *Trg_Slot ) )
					) {
					return 105;
				}
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
						*Trg_Slot = *Trg_Slot2;
						*Trg_Slot2 = 0;
					}
					else if ( ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
					}
					else {
						return 122;
					}
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
				}
				else {
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
					_dWRITE_DIGITAL( Address_TR_TRG_SLOT2 , 0 , &CommStatus );
					*Trg_Slot2 = 0;
				}
			}
			//------------------------------------------------------------------------
			// Append 2002.02.28
			//------------------------------------------------------------------------
			if ( ( *Finger == 0 ) || ( *Finger == 2 ) ) {
				if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( WAFER_SOURCE_IN_FM( 0 , TA_STATION ) + CM1 , *Trg_Station ) ) return 121;
			}
			if ( ( *Finger == 1 ) || ( *Finger == 2 ) ) {
				if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( WAFER_SOURCE_IN_FM( 0 , TB_STATION ) + CM1 , *Trg_Station ) ) return 121;
			}
			//==================================================================================
			if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
				if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
					for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
						if ( ( (*Trg_Slot) + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 109;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , (*Trg_Slot) + a + 1 ) != CWM_ABSENT ) return 109;
						*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
					}
				}
			}
			//==================================================================================
		}
		else if ( *Trg_Station >= BM1 ) {
			if ( *Finger >= 3 ) { // FEM-BM // C:(D)2001.08.15
				//
				if (
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
					( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) ) {
					*Use = 0;
					*Finger = ROBOT_TR_FM_PUT_FINGER_FOR_USE( 0 , *Finger % 3 );
					if ( *Finger < 0 ) return 105;
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
						*Trg_Slot = 1;
						*Trg_Slot2 = 0;
					}
					if ( *Finger == 2 ) {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) || ( *Trg_Slot >= *Trg_Slot2 ) ) return 127;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0  ) return 128;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 128;
					}
					else {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
						_dWRITE_DIGITAL( Address_TR_TRG_SLOT2 , 0 , &CommStatus );
						*Trg_Slot2 = 0;
					}
					//==================================================================================
					if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
						if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
							for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
								if ( ( (*Trg_Slot) + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) return 109;
								if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , (*Trg_Slot) + a + 1 ) > 0 ) return 109;
								*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
							}
						}
					}
					//==================================================================================
				}
				else {
					if ( ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station ) == BUFFER_IN_S_MODE ) || ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station ) == BUFFER_OUT_S_MODE ) ) {
						return 104;
					}
					if ( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) <= 0 ) {
						*Use = 0;
						*Finger = ROBOT_TR_FM_PUT_FINGER_FOR_USE( 0 , *Finger % 3 );
						if ( *Finger < 0 ) return 105;
						//==================================================================================
						if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) { // 2003.01.25
							 return 105;
						}
						//==================================================================================
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							if      ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
								if ( *Finger == 2 ) return 104;
								*Trg_Slot = 2;
								*Trg_Slot2 = 0;
							}
							else if ( ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
								if ( *Finger == 2 ) return 104;
								*Trg_Slot = 1;
								*Trg_Slot2 = 0;
							}
							else if ( ( *Trg_Slot > 0 ) && ( *Trg_Slot2 > 0 ) ) {
								if ( *Finger != 2 ) return 104;
								*Trg_Slot = 1;
								*Trg_Slot2 = 2;
							}
						}
						else {
							if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
								*Trg_Slot = 1;
								*Trg_Slot2 = 0;
							}
						}
						if ( *Finger == 2 ) {
							if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) || ( *Trg_Slot >= *Trg_Slot2 ) ) return 127;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0  ) return 128;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 128;
						}
						else {
							if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
							if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) {
								_dWRITE_DIGITAL( Address_TR_TRG_SLOT2 , 0 , &CommStatus );
								*Trg_Slot2 = 0;
							}
						}
					}
					else {
						*Use = 0;
						*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) - 1 , *Finger % 3 );
						if ( *Finger < 0 ) return 102;
						//==================================================================================
						if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) { // 2003.01.25
							 return 102;
						}
						//==================================================================================
						if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
							if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
							if ( WAFER_STATUS_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) - 1 , *Finger ) <= 0 ) {
								if ( *Trg_Slot > 0 ) return 122;
							}
							else {
								if ( *Trg_Slot <= 0 ) return 122;
							}
							if ( WAFER_STATUS_IN_TM2( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) - 1 , *Finger ) <= 0 ) {
								if ( *Trg_Slot2 > 0 ) return 122;
							}
							else {
								if ( *Trg_Slot2 <= 0 ) return 122;
							}
							//
							if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
							if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 128;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 128;
						}
						else {
							if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
								*Trg_Slot = 1;
								*Trg_Slot2 = 0;
							}
							//
							if ( *Trg_Slot <= 0 ) return 112;
							if ( WAFER_STATUS_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) - 1 , *Finger ) <= 0 ) return 112;
							//
							*Trg_Slot2 = 0;
							if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
						}
					}
					//----------------------------------------------------------------------------------------------
// ??????????
					if ( *Finger == 0 ) {
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( ( (*Trg_Slot) + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) return 109;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , (*Trg_Slot) + a + 1 ) > 0 ) return 109;
							*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
						}
					}
				}
			}
			else { // LL-BM //C:(D)2001.08.23
				*Use = 1;
				if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == *Trg_Station ) return 101; // 2002.09.12
					*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
				}
				else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
					if ( _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() == *Trg_Station ) return 101; // 2002.09.12
					*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
				}
				else {
					*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
				}
				if ( *Finger < 0 ) return 102;
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
					if ( WAFER_STATUS_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) <= 0 ) {
						if ( *Trg_Slot > 0 ) return 122;
					}
					else {
						if ( *Trg_Slot <= 0 ) return 122;
					}
					if ( WAFER_STATUS_IN_TM2( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) <= 0 ) {
						if ( *Trg_Slot2 > 0 ) return 122;
					}
					else {
						if ( *Trg_Slot2 <= 0 ) return 122;
					}
					//
					if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
					if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 128;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 128;
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
						*Trg_Slot = 1;
						*Trg_Slot2 = 0;
					}
					//
					if ( *Trg_Slot <= 0 ) return 122;
					if ( WAFER_STATUS_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) <= 0 ) return 122;
					//
					*Trg_Slot2 = 0;
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
				}
			}
		}
		else { //C:(D)2001.08.23
			*Use = 1;
			if ( *Finger == 2 ) { // 2003.10.17
				//-----------------------------------------------------------------------------------------------------
				*Finger = 0;
				*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
				//-----------------------------------------------------------------------------------------------------
				if ( *Finger < 0 ) {
					*Finger = 1;
					*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
					if ( *Finger < 0 ) return 102;
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station , *Finger , -1 ) ) return 102; // 2003.10.17
				}
				else {
					if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station , *Finger , -1 ) ) {
						*Finger = 1;
						*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
						if ( *Finger < 0 ) return 102;
						if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station , *Finger , -1 ) ) return 102; // 2003.10.17
					}
				}
			}
			else {
				*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger );
				//-----------------------------------------------------------------------------------------------------
				if ( *Finger < 0 ) return 102;
				//-----------------------------------------------------------------------------------------------------
				if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station , *Finger , -1 ) ) return 102; // 2003.10.17
			}
			//-----------------------------------------------------------------------------------------------------
//			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) { // 2003.10.17
//				*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ); // 2003.10.17
//			} // 2003.10.17
//			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) { // 2003.10.17
//				*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ); // 2003.10.17
//			} // 2003.10.17
//			else { // 2003.10.17
//				*Finger = Transfer_ROBOT_PUT_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ); // 2003.10.17
//			} // 2003.10.17
//			if ( *Finger < 0 ) return 102; // 2003.10.17
			//-----------------------------------------------------------------------------------------------------
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
				if ( WAFER_STATUS_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) <= 0 ) {
					if ( *Trg_Slot > 0 ) return 122;
				}
				else {
					if ( *Trg_Slot <= 0 ) return 122;
				}
				if ( WAFER_STATUS_IN_TM2( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) <= 0 ) {
					if ( *Trg_Slot2 > 0 ) return 122;
				}
				else {
					if ( *Trg_Slot2 <= 0 ) return 122;
				}
				//
				if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
				if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 128;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 128;
				//------------------------------------------------------------------------
				// Append 2002.05.20
				//------------------------------------------------------------------------
				if ( *Trg_Slot  > 0 ) {
					if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) + CM1 , *Trg_Station ) ) return 121;
				}
				if ( *Trg_Slot2 > 0 ) {
					if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM2( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) + CM1 , *Trg_Station ) ) return 121;
				}
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
					*Trg_Slot = 1;
					*Trg_Slot2 = 0;
				}
				//
				if ( *Trg_Slot <= 0 ) return 112;
				if ( WAFER_STATUS_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) <= 0 ) return 112;
				//
				*Trg_Slot = 1;
				*Trg_Slot2 = 0;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
				//------------------------------------------------------------------------
				// Append 2002.05.20
				//------------------------------------------------------------------------
				if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( WAFER_SOURCE_IN_TM( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Finger ) + CM1 , *Trg_Station ) ) return 121;
			}
		}
	}
	//----------------------------------------------------------------------------------------------------------------------------
	if ( Mode == TR_MOVE ) {
		if ( ( *Src_Station < CM1 ) || ( *Src_Station == AL ) || ( *Src_Station == IC ) || ( *Src_Station >= TM ) ) {
			return 116;
		}
		else if ( *Src_Station < PM1 ) {
			if ( !Transfer_Enable_Check( *Src_Station ) ) return 111;
		}
		else if ( *Src_Station >= BM1 ) {
			if ( !Transfer_Enable_Check( *Src_Station ) ) return 111;
		}
		else {
			if ( !Transfer_Enable_Check( *Src_Station ) ) return 111;
		}
		if ( ( *Trg_Station < CM1 ) || ( *Trg_Station == AL ) || ( *Trg_Station == IC ) || ( *Trg_Station >= TM ) ) {
			return 126;
		}
		else if ( !Transfer_Enable_Check( *Trg_Station ) ) return 121;
		//
//		if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
//		if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21
		//
		if      ( ( ( *Src_Station < PM1 ) || ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ) && ( ( *Trg_Station < PM1 ) || ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ) ) {
	//c-f-c // C:(D)2001.08.15
			//
			if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
			if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21
			//
			*Use = 0;
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				*Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );
				if ( *Finger < 0 ) return 107;
				//==================================================================================
				if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) { // 2003.03.31
					 return 107;
				}
				//==================================================================================
				if      ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
					if ( *Finger == 1 ) return 107;
					*Finger = 0;
				}
				else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Finger == 0 ) return 107;
					*Finger = 1;
					*Src_Slot = *Src_Slot2;
					*Trg_Slot = *Trg_Slot2;
					*Src_Slot2 = 0;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Src_Slot == *Src_Slot2 ) return 112;
					if ( *Trg_Slot == *Trg_Slot2 ) return 122;
					if ( *Finger != 2 ) return 107;
					*Finger = 2;
				}
				else {
					return 112;
				}
				if ( *Src_Slot > 0 ) {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
					}
					else {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
					}
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
					}
					else {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
					}
				}
				if ( *Src_Slot2 > 0 ) {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 113;
					}
					else {
						if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) != CWM_PRESENT ) return 113;
					}
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 123;
					}
					else {
						if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) != CWM_ABSENT ) return 123;
					}
				}
			}
			else {
				*Finger = ROBOT_TR_FM_GET_FINGER_FOR_USE( 0 , *Finger % 3 );
				if ( *Finger < 0 ) return 106;
				//==================================================================================
				if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
					if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( WAFER_STATUS_IN_FM( 0 , 2 + a ) > 0 ) return 108;
							if ( ( (*Src_Slot) + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 108;
							switch( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , (*Src_Slot) + a + 1 ) ) {
							case CWM_PRESENT :	*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );	break;
							case CWM_ABSENT :	*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );	break;
							default : return 108;
							}
							//
							if ( ( (*Trg_Slot) + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 109;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , (*Trg_Slot) + a + 1 ) != CWM_ABSENT ) return 109;
						}
					}
				}
				//==================================================================================
				if ( *Finger == 2 ) {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
						if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 113;
					}
					else {
//						if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Src_Slot >= *Src_Slot2 ) ) return 112; // 2005.10.17
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112; // 2005.10.17
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) != CWM_PRESENT ) return 113;
					}
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
						if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 123;
					}
					else {
//						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Trg_Slot >= *Trg_Slot2 ) ) return 122; // 2005.10.17
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122; // 2005.10.17
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) != CWM_ABSENT ) return 123;
					}
				}
				else {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
					}
					else {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
					}
					_dWRITE_DIGITAL( Address_TR_SRC_SLOT2 , 0 , &CommStatus );
					*Src_Slot2 = 0;
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
					}
					else {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
					}
					_dWRITE_DIGITAL( Address_TR_TRG_SLOT2 , 0 , &CommStatus );
					*Trg_Slot2 = 0;
				}
			}
			//------------------------------------------------------------------------
			// Append 2002.02.28
			//------------------------------------------------------------------------
			if ( ( *Finger == 0 ) || ( *Finger == 2 ) ) {
				if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( *Src_Station , *Trg_Station ) ) return 121;
			}
			if ( ( *Finger == 1 ) || ( *Finger == 2 ) ) {
				if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( *Src_Station , *Trg_Station ) ) return 121;
			}
		}
		else if ( ( ( *Src_Station < PM1 ) || ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ) && ( *Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
	//c-f-b-t-p //C:(D)2001.08.23
			//
			if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
			//
			*Use = 2;
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
				}
				else {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
				}
				if ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) < 0 ) return 107;
				if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station ) < 0 ) return 107;
				i = _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) + BM1;
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( i ) ) return 151;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
				*Trg_Slot = 1;
				*Trg_Slot2 = 0;
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
				}
				else {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
				}
				if ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) < 0 ) return 107;
				//==================================================================================
				if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.03.31 // 2003.04.29 // 2003.06.18
					if ( ( Transfer_FM_ARM_ALL(0) && ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) != 1 ) ) ) { // 2008.02.01
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( WAFER_STATUS_IN_FM( 0 , 2 + a ) > 0 ) return 108;
							if ( ( ( *Src_Slot + a + 1 ) < 1 ) || ( ( *Src_Slot + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
							switch ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , ( *Src_Slot + a + 1 ) ) ) {
							case CWM_ABSENT :
							case CWM_PRESENT :
								*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
								break;
							default :
								return 108;
								break;
							}
						}
					}
				}
				//==================================================================================
				if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station ) < 0 ) return 107;
				i = _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) + BM1;
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( i ) ) return 151;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
				*Trg_Slot = 1;
				*Trg_Slot2 = 0;
			}
			else {
				for ( p = 0 ; p < MAX_TM_CHAMBER_DEPTH ; p++ ) BM_Station[p] = -1;
				//
				if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) , 1 ) <= 0 ) {
						BM_Station[0] = _SCH_COMMON_TRANSFER_OPTION( 2 , -1 );
						BM_Slot[0]  = 1;
					}
				}
				else {
					if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.12.21
						//-------------------------------------------------------------------
						// 2004.08.16
						//-------------------------------------------------------------------
						i = Transfer_GET_SUPPLY_BM_AT_SWITCH( *Src_Station - CM1 );
						j = Transfer_GET_SUPPLY_SLOT_FOR_BM_AT_SWITCH( i , *Src_Slot , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( *Src_Station - CM1 ) % 2 );
						if ( j == -1 ) return SYS_ERROR;
						//
						BM_Station[0] = i;
						BM_Slot[0] = j;
						//-------------------------------------------------------------------
					}
					else {
						for ( p = 0 ; p <= _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) ; p++ ) {
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
									if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) {
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
												BM_Station[p] = i;
												BM_Slot[p] = j + 1;
												i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
												j = 9999999;
											}
										}
									}
								}
							}
							if ( BM_Station[p] < 0 ) return 151;
						}
					}
				}
				//
				if ( BM_Station[0] < 0 ) return 151;
				//
				*Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );
				if ( *Finger < 0 ) return 107;
				//
				for ( p = 0 ; p <= _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) ; p++ ) {
					if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( p , *Trg_Station ) < 0 ) return 107;
					if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
						BM_Slot2[p] = BM_Slot[p] + 1;
						if ( ( BM_Slot[p] % 2 ) != 1 ) return 151;
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[p] , BM_Slot[p] ) > 0 ) return 151;
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[p] , BM_Slot2[p] ) > 0 ) return 151;
						if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
							if ( *Finger == 1 ) return 107;
							*Finger = 0;
							*Src_Slot2 = 0;
							BM_Slot[p] = 2;
							BM_Slot2[p] = 0;
							BM_PSlot[p] = 0;
							BM_PSlot2[p] = 2;
							*Trg_Slot = 0;
							*Trg_Slot2 = 2;
						}
						else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
							if ( *Finger == 1 ) return 107;
							*Finger = 0;
							*Src_Slot2 = 0;
							BM_Slot[p] = 1;
							BM_Slot2[p] = 0;
							BM_PSlot[p] = 1;
							BM_PSlot2[p] = 0;
							*Trg_Slot = 1;
							*Trg_Slot2 = 0;
						}
						else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
							if ( *Finger == 0 ) return 107;
							*Finger = 1;
							*Src_Slot = 0;
							BM_Slot[p] = 0;
							BM_Slot2[p] = 2;
							BM_PSlot[p] = 0;
							BM_PSlot2[p] = 2;
							*Trg_Slot = 0;
							*Trg_Slot2 = 2;
						}
						else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
							if ( *Finger == 0 ) return 107;
							*Finger = 1;
							*Src_Slot = 0;
							BM_Slot[p] = 0;
							BM_Slot2[p] = 1;
							BM_PSlot[p] = 1;
							BM_PSlot2[p] = 0;
							*Trg_Slot = 1;
							*Trg_Slot2 = 0;
						}
						else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 > 0 ) ) {
							BM_Slot[p] = 1;
							BM_Slot2[p] = 2;
							BM_PSlot[p] = 1;
							BM_PSlot2[p] = 2;
							*Trg_Slot = 1;
							*Trg_Slot2 = 2;
						}
						else {
							return 112;
						}
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
							*Trg_Slot = 1;
							*Trg_Slot2 = 0;
						}
						//
						BM_Slot2[p] = 0;
						*Src_Slot2 = 0;
						*Trg_Slot2 = 0;
						//
						if ( *Trg_Slot > 0 ) *Trg_Slot = 1;
						BM_PSlot[p] = BM_Slot[p];
						BM_PSlot2[p] = 0;
					}
				}
				if ( *Src_Slot == *Src_Slot2 ) return 112;
				if ( *Src_Slot > 0 ) {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
				}
				if ( *Src_Slot2 > 0 ) {
					if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) != CWM_PRESENT ) return 113;
				}
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 124;
				}
				else {
					if ( *Trg_Slot <= 0 ) return 124;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 124;
				}
				//------------------------------------------------------------------------
				// Append 2002.05.20
				//------------------------------------------------------------------------
				if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( *Src_Station , *Trg_Station ) ) return 121;
				//------------------------------------------------------------------------
				// 2003.10.21
				//------------------------------------------------------------------------
// ??????????
				for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
					if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) return 108;
					if ( ( ( *Src_Slot + a + 1 ) < 1 ) || ( ( *Src_Slot + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					switch ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , ( *Src_Slot + a + 1 ) ) ) {
					case CWM_ABSENT :
					case CWM_PRESENT :
						*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
						break;
					default :
						return 108;
						break;
					}
				}
				//------------------------------------------------------------------------
			}
		}
		else if ( ( ( *Src_Station < PM1 ) || ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ) && ( *Trg_Station < TM ) ) {
	//c-f-b // C:(D)2001.08.15
			//
			if ( FM_AL_HAS_A_WAFER( TR_CHECKING_SIDE ) ) return 131; // 2016.07.21
			//
			*Use = 0;
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				*Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );
				if ( *Finger < 0 ) return 107;
				//==================================================================================
				if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) { // 2003.03.31
					 return 107;
				}
				//==================================================================================
				if      ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Finger == 1 ) return 107;
					*Finger = 0;
					*Trg_Slot = 2;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
					if ( *Finger == 1 ) return 107;
					*Finger = 0;
					*Trg_Slot = 1;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Finger == 0 ) return 107;
					*Finger = 1;
					*Src_Slot = *Src_Slot2;
					*Src_Slot2 = 0;
					*Trg_Slot = 2;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
					if ( *Finger == 0 ) return 107;
					*Finger = 1;
					*Src_Slot = *Src_Slot2;
					*Src_Slot2 = 0;
					*Trg_Slot = 1;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Src_Slot == *Src_Slot2 ) return 112;
					if ( *Finger != 2 ) return 107;
					*Finger = 2;
					*Trg_Slot = 1;
					*Trg_Slot2 = 2;
				}
				else {
					return 112;
				}
				if ( *Src_Slot > 0 ) {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
					}
					else {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
					}
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 127;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
				}
				if ( *Src_Slot2 > 0 ) {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 113;
					}
					else {
						if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) != CWM_PRESENT ) return 113;
					}
					if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 127;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 128;
				}
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
					*Trg_Slot = 1;
					*Trg_Slot2 = 0;
				}
				//
				*Finger = ROBOT_TR_FM_GET_FINGER_FOR_USE( 0 , *Finger % 3 );
				if ( *Finger < 0 ) return 106;
				//==================================================================================
				if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
					if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( WAFER_STATUS_IN_FM( 0 , 2 + a ) > 0 ) return 108;
							if ( ( (*Src_Slot) + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 108;
							switch( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , (*Src_Slot) + a + 1 ) ) {
							case CWM_PRESENT :	*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );	break;
							case CWM_ABSENT :	*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );	break;
							default : return 108;
							}
							//
							if ( ( (*Trg_Slot) + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) return 109;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , (*Trg_Slot) + a + 1 ) > 0 ) return 109;
						}
					}
				}
				//==================================================================================
				if ( *Finger == 2 ) {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
						if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 113;
					}
					else {
//						if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Src_Slot >= *Src_Slot2 ) ) return 112; // 2005.10.17
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Src_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112; // 2005.10.17
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) != CWM_PRESENT ) return 113;
					}
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) || ( *Trg_Slot >= *Trg_Slot2 ) ) return 127;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 128;
					if (
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) &&
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_6_INLIGN_PM2_V1 )
						) { // 2003.04.28
						if ( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) > 0 ) return 151;
					}
				}
				else {
					if ( *Src_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 113;
					}
					else {
						if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) != CWM_PRESENT ) return 113;
					}
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
					if (
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) &&
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_6_INLIGN_PM2_V1 )
						) { // 2003.04.28
						for ( p = 0 ; p < _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) ; p++ ) {
							BM_Station[p] = -1;
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
									if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) {
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
												BM_Station[p] = i;
												BM_Slot[p] = j + 1;
												i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
												j = 9999999;
											}
										}
									}
								}
							}
							if ( BM_Station[p] < 0 ) return 151;
							*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger % 3);
							if ( *Finger < 0 ) return 103;
						}
					}
					_dWRITE_DIGITAL( Address_TR_SRC_SLOT2 , 0 , &CommStatus );
					*Src_Slot2 = 0;
					_dWRITE_DIGITAL( Address_TR_TRG_SLOT2 , 0 , &CommStatus );
					*Trg_Slot2 = 0;
				}
			}
		}
		else if ( ( *Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) && ( ( *Trg_Station < PM1 ) || ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ) ) {
	//p-t-b-f-c //C:(D)2001.08.23
			//
			if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21
			//
			*Use = 3;
			if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) {
				if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
				}
				else {
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
				}
				if ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) < 0 ) return 107;
				if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station ) < 0 ) return 107;
				i = _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) + BM1;
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( i ) ) return 151;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
				*Src_Slot = 1;
				*Src_Slot2 = 0;
			}
			else if ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 ) {
				if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
				}
				else {
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
				}
				if ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) < 0 ) return 107;
				//==================================================================================
				if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.03.31 // 2003.04.29 // 2003.06.18
					if ( ( Transfer_FM_ARM_ALL(0) && ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( ROBOT_TR_FM_PICK_FINGER( 0 , -1 ) != 1 ) ) ) { // 2008.02.01
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) { // 2003.10.21
							if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) return 108;
							if ( ( ( *Trg_Slot + a + 1 ) < 1 ) || ( ( *Trg_Slot + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , ( *Trg_Slot + a + 1 ) ) != CWM_ABSENT ) return 108;
							*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
						}
					}
				}
				//==================================================================================
				if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station ) < 0 ) return 107;
				i = _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) + BM1;
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( i ) ) return 151;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
				*Src_Slot = 1;
				*Src_Slot2 = 0;
			}
			else {
				for ( p = 0 ; p < MAX_TM_CHAMBER_DEPTH ; p++ ) BM_Station[p] = -1;
				//
				if ( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) > 0 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( _SCH_COMMON_TRANSFER_OPTION( 2 , -1 ) , 1 ) <= 0 ) {
						BM_Station[0] = _SCH_COMMON_TRANSFER_OPTION( 2 , -1 );
						BM_Slot[0]  = 1;
					}
				}
				else {
					if ( _SCH_COMMON_TRANSFER_OPTION( 1 , -1 ) != 0 ) { // 2006.12.21
						//-------------------------------------------------------------------
						// 2004.08.16
						//-------------------------------------------------------------------
						i = Transfer_GET_CLEAR_BM_AT_SWITCH( *Trg_Station - CM1 );
						j = Transfer_GET_CLEAR_SLOT_FOR_BM_AT_SWITCH( i , *Trg_Slot , _i_SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( *Trg_Station - CM1 ) % 2 );
						if ( j == -1 ) return SYS_ERROR;
						//
						BM_Station[0] = i;
						BM_Slot[0] = j;
						//-------------------------------------------------------------------
					}
					else {
						for ( p = 0 ; p <= _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) ; p++ ) {
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
									if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
												BM_Station[p] = i;
												BM_Slot[p] = j + 1;
												i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
												j = 9999999;
											}
										}
									}
								}
							}
							if ( BM_Station[p] < 0 ) return 151;
						}
					}
				}
				//
				if ( BM_Station[0] < 0 ) return 151;
				//
				*Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );
				if ( *Finger < 0 ) return 107;
				//
				for ( p = 0 ; p <= _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) ; p++ ) {
					if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( p , *Trg_Station ) < 0 ) return 107;
					if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
						BM_Slot2[p] = BM_Slot[p] + 1;
						if ( ( BM_Slot[p] % 2 ) != 1 ) return 151;
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[p] , BM_Slot[p] ) > 0 ) return 151;
						if ( _i_SCH_IO_GET_MODULE_STATUS( BM_Station[p] , BM_Slot2[p] ) > 0 ) return 151;
						if      ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
							if ( *Finger == 0 ) return 107;
							*Finger = 1;
							*Src_Slot = 1;
							*Src_Slot2 = 0;
							BM_PSlot[p] = 1;
							BM_PSlot2[p] = 0;
							BM_Slot[p] = 0;
							BM_Slot2[p] = 1;
							*Trg_Slot = 0;
						}
						else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
							if ( *Finger == 1 ) return 107;
							*Finger = 0;
							*Src_Slot = 1;
							*Src_Slot2 = 0;
							BM_PSlot[p] = 1;
							BM_PSlot2[p] = 0;
							BM_Slot[p] = 1;
							BM_Slot2[p] = 0;
							*Trg_Slot2 = 0;
						}
						else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
							if ( *Finger == 0 ) return 107;
							*Finger = 1;
							*Src_Slot = 0;
							*Src_Slot2 = 2;
							BM_PSlot[p] = 0;
							BM_PSlot2[p] = 2;
							BM_Slot[p] = 0;
							BM_Slot2[p] = 2;
							*Trg_Slot = 0;
						}
						else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
							if ( *Finger == 1 ) return 107;
							*Finger = 0;
							*Src_Slot = 0;
							*Src_Slot2 = 2;
							BM_PSlot[p] = 0;
							BM_PSlot2[p] = 2;
							BM_Slot[p] = 2;
							BM_Slot2[p] = 0;
							*Trg_Slot2 = 0;
						}
						else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 > 0 ) ) {
							*Src_Slot = 1;
							*Src_Slot2 = 2;
							BM_PSlot[p] = 1;
							BM_PSlot2[p] = 2;
							BM_Slot[p] = 1;
							BM_Slot2[p] = 2;
						}
						else {
							return 112;
						}
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
							*Src_Slot = 1;
							*Src_Slot2 = 0;
						}
						//
						BM_Slot2[p] = 0;
						*Src_Slot2 = 0;
						*Trg_Slot2 = 0;
						//
						if ( *Src_Slot > 0 ) *Src_Slot = 1;
						BM_PSlot[p] = BM_Slot[p];
						BM_PSlot2[p] = 0;
					}
				}
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) ) {
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
					}
					else if ( *Src_Slot > 0 ) {
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) ) return 114;
					}
					else if ( *Src_Slot2 > 0 ) {
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
					}
					else {
						return 114;
					}
				}
				else {
					if ( *Src_Slot <= 0 ) return 114;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
				}
				if ( *Trg_Slot == *Trg_Slot2 ) return 122;
				if ( *Trg_Slot > 0 ) {
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
				}
				if ( *Trg_Slot2 > 0 ) {
					if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) != CWM_ABSENT ) return 123;
				}
				//------------------------------------------------------------------------
				// Append 2002.02.28
				//------------------------------------------------------------------------
				if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( _i_SCH_IO_GET_MODULE_SOURCE( *Src_Station , 1 ) + CM1 , *Trg_Station ) ) return 121;
				//------------------------------------------------------------------------
				// Append 2002.05.20
				//------------------------------------------------------------------------
				if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( *Trg_Station , *Src_Station ) ) return 121;
				//------------------------------------------------------------------------
				// 2003.10.21
				//------------------------------------------------------------------------
// ??????????
				for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
					if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) return 108;
					if ( ( ( *Trg_Slot + a + 1 ) < 1 ) || ( ( *Trg_Slot + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 112;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , ( *Trg_Slot + a + 1 ) ) != CWM_ABSENT ) return 108;
					*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
				}
				//------------------------------------------------------------------------
			}
		}
		else if ( ( *Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) && ( *Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
	//p-t-p //C:(D)2001.08.23
			*Use = 1;
			//
			*st = _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station );
			*et = _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station );
			//
			if (
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 )
				 ) {
				if ( *st != *et ) return 151; // 2003.04.21
			}
			//----------------------------------------------------------------------------------------------------------------
			// 2004.08.19
			//----------------------------------------------------------------------------------------------------------------
			if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( Finger , *st , *et , *Src_Station , *Trg_Station ) ) return 103; // 2004.08.19
			//----------------------------------------------------------------------------------------------------------------
			//----------------------------------------------------------------------------------------------------------------
			if ( *st > *et ) {
				for ( p = *st ; p > *et ; p-- ) {
					BM_Station[p] = -1;
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
								for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
										BM_Station[p] = i;
										BM_Slot[p] = j + 1;
										i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
										j = 9999999;
									}
								}
							}
						}
					}
					if ( BM_Station[p] < 0 ) return 151;
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
					if ( *Finger < 0 ) return 103;
				}
			}
			else if ( *st < *et ) {
				for ( p = *et ; p > *st ; p-- ) {
					BM_Station[p] = -1;
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) {
								for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
										BM_Station[p] = i;
										BM_Slot[p] = j + 1;
										i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
										j = 9999999;
									}
								}
							}
						}
					}
					if ( BM_Station[p] < 0 ) return 151;
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
					if ( *Finger < 0 ) return 103;
				}
			}
			else {
				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( *st , *Finger );
				if ( *Finger < 0 ) return 103;
			}
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
				if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
				if ( ( *Src_Slot2 > 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
				if ( ( *Src_Slot2 <= 0 ) && ( *Trg_Slot2 > 0 ) ) return 112;
				if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
				if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
				//
				if ( *Src_Slot  > 0 ) *Src_Slot = 1;
				if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
				if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) ) {
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
				}
				else if ( *Src_Slot > 0 ) {
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) ) return 114;
				}
				else if ( *Src_Slot2 > 0 ) {
					if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
				}
				else {
					return 114;
				}
				if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
				if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 124;
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
					*Src_Slot = 1;
					*Src_Slot2 = 0;
				}
				//
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
					*Trg_Slot = 1;
					*Trg_Slot2 = 0;
				}
				//
				if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 121;
				if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 111;
				if ( ( *Src_Slot <= 0 ) || ( *Trg_Slot <= 0 ) ) return 111;
				//
				*Src_Slot2 = 0;
				*Trg_Slot2 = 0;
				if ( *Src_Slot > 0 ) *Src_Slot = 1;
				if ( *Src_Slot <= 0 ) return 114;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
				if ( *Trg_Slot <= 0 ) return 124;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 124;
			}
			//------------------------------------------------------------------------
			// Append 2002.05.20
			//------------------------------------------------------------------------
			if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( _i_SCH_IO_GET_MODULE_SOURCE( *Src_Station , 1 ) + CM1 , *Trg_Station ) ) return 121;
			//------------------------------------------------------------------------------
//			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , *st , *Src_Station , *Finger , -1 ) ) return 103; // 2003.10.16 // 2004.08.19
//			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , *et , *Trg_Station , *Finger , -1 ) ) return 103; // 2003.10.16  // 2004.08.19
			//------------------------------------------------------------------------------
		}
		else if ( ( *Src_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) && ( *Trg_Station < TM ) ) {
	//p-t-b //C:(D)2001.08.23
			*Use = 1;
			*st = _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station );
			*et = _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station );
			if (
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 )
				 ) {
				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger ); // 2003.04.21
				if ( *Finger < 0 ) return 103; // 2003.04.21
//				if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Src_Station ) < 0 ) return 107; // 2003.04.21
				if ( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) != _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) ) return 151;
				for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ; j++ ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , j + 1 ) <= 0 ) {
						break;
					}
				}
				if ( j == _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) return 151;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
				*Src_Slot = 1;
				*Src_Slot2 = 0;
				//
				*Trg_Slot2 = 0; // 2005.10.18
			}
			else {
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.19
				//----------------------------------------------------------------------------------------------------------------
				if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( Finger , *st , *et , *Src_Station , *Trg_Station ) ) return 103; // 2004.08.19
				//----------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( *et , *Trg_Station ) != BUFFER_IN_S_MODE && _i_SCH_PRM_GET_MODULE_BUFFER_MODE( *et , *Trg_Station ) != BUFFER_OUT_S_MODE ) {
					if ( ( *st + 1 ) == *et ) *et = *st;
				}
				if ( *st > *et ) {
					for ( p = *st ; p > *et ; p-- ) {
						BM_Station[p] = -1;
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
									for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
										if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
											BM_Station[p] = i;
											BM_Slot[p] = j + 1;
											i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
											j = 9999999;
										}
									}
								}
							}
						}
						if ( BM_Station[p] < 0 ) return 151;
						*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
						if ( *Finger < 0 ) return 103;
					}
				}
				else if ( *st < *et ) {
					for ( p = *et ; p > *st ; p-- ) {
						BM_Station[p] = -1;
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) {
									for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
										if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
											BM_Station[p] = i;
											BM_Slot[p] = j + 1;
											i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
											j = 9999999;
										}
									}
								}
							}
						}
						if ( BM_Station[p] < 0 ) return 151;
						*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
						if ( *Finger < 0 ) return 103;
					}
				}
				else {
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( *st , *Finger );
					if ( *Finger < 0 ) return 103;
				}
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
					if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
					if ( ( *Src_Slot2 > 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
					if ( ( *Src_Slot2 <= 0 ) && ( *Trg_Slot2 > 0 ) ) return 112;
					if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
					if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
					//
					if ( *Src_Slot  > 0 ) *Src_Slot = 1;
					if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
					if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) ) {
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
					}
					else if ( *Src_Slot > 0 ) {
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) ) return 114;
					}
					else if ( *Src_Slot2 > 0 ) {
						if ( ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) || ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) ) return 114;
					}
					else {
						return 114;
					}
					if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
					if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 128;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 128;
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
						*Src_Slot = 1;
						*Src_Slot2 = 0;
					}
					//
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
						*Trg_Slot = 1;
						*Trg_Slot2 = 0;
					}
					//
					if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
					if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
					if ( ( *Src_Slot <= 0 ) || ( *Trg_Slot <= 0 ) ) return 112;
					//
					*Src_Slot2 = 0;
					*Trg_Slot2 = 0;
					if ( *Src_Slot > 0 ) *Src_Slot = 1;
					if ( *Src_Slot <= 0 ) return 114;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
					if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 128;
				}
			}
			//------------------------------------------------------------------------------
//			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , *st , *Src_Station , *Finger , -1 ) ) return 103; // 2003.10.16 // 2004.08.19
//			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , *et , *Trg_Station , *Finger , -1 ) ) return 103; // 2003.10.16 // 2004.08.19
			//------------------------------------------------------------------------------
		}
		else if ( ( *Src_Station < TM ) && ( ( *Trg_Station < PM1 ) || ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) ) ) {
	//b-f-c // C:(D)2001.08.15
			//
			if ( FM_IC_HAS_A_WAFER( TR_CHECKING_SIDE , &j ) ) return 132; // 2016.07.21
			//
			*Use = 0;
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				*Finger = ROBOT_TR_FM_PICK_FINGER( 0 , -1 );
				if ( *Finger < 0 ) return 107;
				//==================================================================================
				if ( ( *Finger != 0 ) && ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) ) { // 2003.03.31
					 return 107;
				}
				//==================================================================================
				if      ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Finger == 0 ) return 107;
					*Finger = 1;
					*Src_Slot = 1;
					*Src_Slot2 = 0;
					*Trg_Slot = *Trg_Slot2;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 <= 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
					if ( *Finger == 1 ) return 107;
					*Finger = 0;
					*Src_Slot = 1;
					*Src_Slot2 = 0;
				}
				else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Finger == 0 ) return 107;
					*Finger = 1;
					*Src_Slot = 2;
					*Src_Slot2 = 0;
					*Trg_Slot = *Trg_Slot2;
					*Trg_Slot2 = 0;
				}
				else if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 <= 0 ) ) {
					if ( *Finger == 1 ) return 107;
					*Finger = 0;
					*Src_Slot = 2;
					*Src_Slot2 = 0;
				}
				else if ( ( *Src_Slot > 0 ) && ( *Src_Slot2 > 0 ) && ( *Trg_Slot > 0 ) && ( *Trg_Slot2 > 0 ) ) {
					if ( *Trg_Slot == *Trg_Slot2 ) return 122;
					if ( *Finger != 2 ) return 107;
					*Finger = 2;
					*Src_Slot = 1;
					*Src_Slot2 = 2;
				}
				else {
					return 112;
				}
				if ( *Src_Slot > 0 ) {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 117;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
					}
					else {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
					}
				}
				if ( *Src_Slot2 > 0 ) {
					if ( ( *Src_Slot2 < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) ) return 117;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 118;
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
						if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 123;
					}
					else {
						if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) != CWM_ABSENT ) return 123;
					}
				}
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
					*Src_Slot = 1;
					*Src_Slot2 = 0;
				}
				//
				*Finger = ROBOT_TR_FM_GET_FINGER_FOR_USE( 0 , *Finger % 3 );
				if ( *Finger < 0 ) return 106;
				//==================================================================================
				if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) > 0 ) { // 2003.04.29
					if ( ( Transfer_FM_ARM_ALL(0) && ( *Finger != 0 ) ) || ( !Transfer_FM_ARM_ALL(0) && ( *Finger != 1 ) ) ) { // 2008.02.01
						for ( a = 0 ; a < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ; a++ ) {
							if ( WAFER_STATUS_IN_FM( 0 ,2 + a ) > 0 ) return 108;
							//
							if ( ( (*Src_Slot) + a + 1 ) > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) return 108;
							*DoArmMX = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 );
							//
							if ( ( (*Trg_Slot) + a + 1 ) > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return 109;
							if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , (*Trg_Slot) + a + 1 ) != CWM_ABSENT ) return 109;
						}
					}
				}
				//==================================================================================
				if ( *Finger == 2 ) {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) || ( *Src_Slot >= *Src_Slot2 ) ) return 117;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot2 ) <= 0 ) return 118;
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
						if ( ( *Trg_Slot2 < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) > 0 ) return 123;
					}
					else {
//						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Trg_Slot >= *Trg_Slot2 ) ) return 122; // 2005.10.17
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot2 < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) || ( *Trg_Slot2 > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122; // 2005.10.17
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot2 ) != CWM_ABSENT ) return 123;
					}
					if (
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) &&
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_6_INLIGN_PM2_V1 )
						) { // 2003.04.28
						if ( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) > 0 ) return 151;
					}
				}
				else {
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 118;
					if ( *Trg_Station == _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() ) { // 2003.09.03
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Trg_Station ) ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 123;
					}
					else {
						if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) ) return 122;
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) != CWM_ABSENT ) return 123;
					}
					if (
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) &&
						( Transfer_RUN_ALG_STYLE_GET() != RUN_ALGORITHM_6_INLIGN_PM2_V1 )
						) { // 2003.04.28
						for ( p = 0 ; p < _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) ; p++ ) {
							BM_Station[p] = -1;
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
									if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
										for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
											if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
												BM_Station[p] = i;
												BM_Slot[p] = j + 1;
												i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
												j = 9999999;
											}
										}
									}
								}
							}
							if ( BM_Station[p] < 0 ) return 151;
							*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger % 3 );
							if ( *Finger < 0 ) return 103;
						}
					}
					_dWRITE_DIGITAL( Address_TR_SRC_SLOT2 , 0 , &CommStatus );
					*Src_Slot2 = 0;
					_dWRITE_DIGITAL( Address_TR_TRG_SLOT2 , 0 , &CommStatus );
					*Trg_Slot2 = 0;
				}
			}
			//------------------------------------------------------------------------
			// Append 2002.02.28
			//------------------------------------------------------------------------
			if ( !Get_RunPrm_CHAMBER_SLOT_OFFSET_POSSIBLE_CHECK( _i_SCH_IO_GET_MODULE_SOURCE( *Src_Station , *Src_Slot ) + CM1 , *Trg_Station ) ) return 121;
		}
		else if ( ( *Src_Station < TM ) && ( *Trg_Station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
	//b-t-p //C:(D)2001.08.23
			*Use = 1;
			*st = _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station );
			*et = _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station );
			if (
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 )
				 ) {
				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) , *Finger ); // 2003.04.21
				if ( *Finger < 0 ) return 103; // 2003.04.21
//				if ( Transfer_ROBOT_GET_TR_FAST_PICK_FINGER( _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) , *Trg_Station ) < 0 ) return 107; // 2003.04.21
				if ( _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station ) != _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station ) ) return 151;
				//-------------------------------------------------------------------------
				// 2003.04.21
				//-------------------------------------------------------------------------
				if ( *Src_Slot <= 0 ) return 151;
				if ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE( *Src_Station ) ) return 151;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 151;
				//
				*Src_Slot2 = 0;
				//
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
				*Trg_Slot = 1;
				*Trg_Slot2 = 0;
			}
			else {
				//----------------------------------------------------------------------------------------------------------------
				// 2004.08.19
				//----------------------------------------------------------------------------------------------------------------
				if ( !Transfer_Enable_InterlockFinger_CheckOnly_for_Move_FEM( Finger , *st , *et , *Src_Station , *Trg_Station ) ) return 103; // 2004.08.19
				//----------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( *st , *Src_Station ) != BUFFER_IN_S_MODE && _i_SCH_PRM_GET_MODULE_BUFFER_MODE( *st , *Src_Station ) != BUFFER_OUT_S_MODE ) {
					if ( ( *et + 1 ) == *st ) *st = *et;
				}
				if ( *st > *et ) {
					for ( p = *st ; p > *et ; p-- ) {
						BM_Station[p] = -1;
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
									for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
										if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
											BM_Station[p] = i;
											BM_Slot[p] = j + 1;
											i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
											j = 9999999;
										}
									}
								}
							}
						}
						if ( BM_Station[p] < 0 ) return 151;
						*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
						if ( *Finger < 0 ) return 103;
					}
				}
				else if ( *st < *et ) {
					for ( p = *et ; p > *st ; p-- ) {
						BM_Station[p] = -1;
						for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
							if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
								if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) {
									for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
										if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
											BM_Station[p] = i;
											BM_Slot[p] = j + 1;
											i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
											j = 9999999;
										}
									}
								}
							}
						}
						if ( BM_Station[p] < 0 ) return 151;
						*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
						if ( *Finger < 0 ) return 103;
					}
				}
				else {
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( *st , *Finger );
					if ( *Finger < 0 ) return 103;
				}
				if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
					if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
					if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
					if ( ( *Src_Slot2 > 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
					if ( ( *Src_Slot2 <= 0 ) && ( *Trg_Slot2 > 0 ) ) return 112;
					if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
					if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
					//
					if ( *Src_Slot  > 0 ) *Src_Slot = 1;
					if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
					if ( *Src_Slot > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) return 114;
					}
					if ( *Src_Slot2 > 0 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) return 114;
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) return 114;
					}
					if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
					if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 124;
				}
				else {
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
						*Src_Slot = 1;
						*Src_Slot2 = 0;
					}
					//
					if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
						*Trg_Slot = 1;
						*Trg_Slot2 = 0;
					}
					//
					if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
					if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
					if ( ( *Src_Slot <= 0 ) || ( *Trg_Slot <= 0 ) ) return 112;
					//
					*Src_Slot2 = 0;
					*Trg_Slot2 = 0;
					if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
					if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
				}
				//------------------------------------------------------------------------
				// Append 2002.05.20
				//------------------------------------------------------------------------
				if ( !Get_RunPrm_CHAMBER_INTERLOCK_POSSIBLE_CHECK( _i_SCH_IO_GET_MODULE_SOURCE( *Src_Station , *Src_Slot ) + CM1 , *Trg_Station ) ) return 121;
			}
			//------------------------------------------------------------------------------
//			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PICK , *st , *Src_Station , *Finger , -1 ) ) return 103; // 2003.10.16 // 2004.08.19
//			if ( !Transfer_Enable_InterlockFinger_Check_for_Sub( TR_PLACE , *et , *Trg_Station , *Finger , -1 ) ) return 103; // 2003.10.16 // 2004.08.19
			//------------------------------------------------------------------------------
		}
		else if ( ( *Src_Station < TM ) && ( *Trg_Station < TM ) ) {
	//b-t-b //C:(D)2001.08.23
			*Use = 1;
			if (
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_4_INLIGN_EXPRESS_V1 ) ||
				 ( Transfer_RUN_ALG_STYLE_GET() == RUN_ALGORITHM_6_INLIGN_PM2_V1 )
				 ) {
				return 151;
			}
			*st = _i_SCH_PRM_GET_MODULE_GROUP( *Src_Station );
			*et = _i_SCH_PRM_GET_MODULE_GROUP( *Trg_Station );
			if ( ( *st + 1 ) == *et ) *et = *st;
			if ( *st > *et ) {
				for ( p = *st ; p > *et ; p-- ) {
					BM_Station[p] = -1;
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_OUT_MODE ) {
								for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
										BM_Station[p] = i;
										BM_Slot[p] = j + 1;
										i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
										j = 9999999;
									}
								}
							}
						}
					}
					if ( BM_Station[p] < 0 ) return 151;
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
					if ( *Finger < 0 ) return 103;
				}
			}
			else if ( *st < *et ) {
				for ( p = *et ; p > *st ; p-- ) {
					BM_Station[p] = -1;
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						if ( Transfer_Enable_Check( i ) && ( _i_SCH_PRM_GET_MODULE_GROUP( i ) == p ) ) {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( p , i ) == BUFFER_IN_MODE ) {
								for ( j = 0 ; j < _i_SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
									if ( _i_SCH_IO_GET_MODULE_STATUS( i , j + 1 ) <= 0 ) {
										BM_Station[p] = i;
										BM_Slot[p] = j + 1;
										i = ( BM1 + MAX_BM_CHAMBER_DEPTH );
										j = 9999999;
									}
								}
							}
						}
					}
					if ( BM_Station[p] < 0 ) return 151;
					*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( p , *Finger );
					if ( *Finger < 0 ) return 103;
				}
			}
			else {
				*Finger = Transfer_ROBOT_GET_TR_FINGER_FOR_USE( *st , *Finger );
				if ( *Finger < 0 ) return 103;
			}
			if ( _i_SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) {
				if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
				if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
				if ( ( *Src_Slot2 > 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
				if ( ( *Src_Slot2 <= 0 ) && ( *Trg_Slot2 > 0 ) ) return 112;
				if ( ( *Src_Slot <= 0 ) && ( *Src_Slot2 <= 0 ) ) return 112;
				if ( ( *Trg_Slot <= 0 ) && ( *Trg_Slot2 <= 0 ) ) return 122;
				//
				if ( *Src_Slot  > 0 ) *Src_Slot = 1;
				if ( *Src_Slot2 > 0 ) *Src_Slot2 = 2;
				if ( *Src_Slot > 0 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) <= 0 ) return 114;
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 1 ) > 0 ) return 114;
				}
				if ( *Src_Slot2 > 0 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) <= 0 ) return 114;
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , 2 ) > 0 ) return 114;
				}
				if ( *Trg_Slot  > 0 ) *Trg_Slot = 1;
				if ( *Trg_Slot2 > 0 ) *Trg_Slot2 = 2;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 2 ) > 0 ) return 124;
			}
			else {
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) == 1 ) {
					*Src_Slot = 1;
					*Src_Slot2 = 0;
				}
				//
				if ( _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) == 1 ) {
					*Trg_Slot = 1;
					*Trg_Slot2 = 0;
				}
				//
				if ( ( *Src_Slot > 0 ) && ( *Trg_Slot <= 0 ) ) return 122;
				if ( ( *Src_Slot <= 0 ) && ( *Trg_Slot > 0 ) ) return 112;
				if ( ( *Src_Slot <= 0 ) || ( *Trg_Slot <= 0 ) ) return 112;
				//
				*Src_Slot2 = 0;
				*Trg_Slot2 = 0;
				if ( ( *Src_Slot < 1 ) || ( *Src_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Src_Station) ) ) return 117;
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Src_Station , *Src_Slot ) <= 0 ) return 114;
				if ( ( *Trg_Slot < 1 ) || ( *Trg_Slot > _i_SCH_PRM_GET_MODULE_SIZE(*Trg_Station) ) ) return 127;
//				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , 1 ) > 0 ) return 124; // 2003.05.19
				if ( _i_SCH_IO_GET_MODULE_STATUS( *Trg_Station , *Trg_Slot ) > 0 ) return 124; // 2003.05.19
			}
		}
	}
	return ERROR_NONE;
}
//===================================================================================================
int Transfer_User_FEM_Code( BOOL SDMMode , int Signal , int oppis , int opplc , int oppls , int *runfinger ) {
	int Mode;
	int Src_Station;
	int Trg_Station;
	int Src_Slot;
	int Src_Slot2;
	int Trg_Slot;
	int Trg_Slot2;
	int Finger;
	int Use;
	int BM_Station[MAX_TM_CHAMBER_DEPTH];
	int BM_Slot[MAX_TM_CHAMBER_DEPTH];
	int BM_Slot2[MAX_TM_CHAMBER_DEPTH];
	int BM_PSlot[MAX_TM_CHAMBER_DEPTH];
	int BM_PSlot2[MAX_TM_CHAMBER_DEPTH];
	int st , et;
	int Result;
	int	i;
	int DoArmMX; // 2003.04.29
	int rcode; // 2006.10.11

	if ( SDMMode ) { // 2002.05.28
		Mode		= TR_MOVE;
		Src_Station = Signal;
		Src_Slot    = oppis;
		Src_Slot2   = 0;
		Trg_Station = opplc;
		Trg_Slot    = oppls;
		Trg_Slot2   = 0;
		Finger      = 2;
	}
	else {
		EnterCriticalSection( &CR_MAIN );

		if ( BUTTON_GET_TR_CONTROL() != CTC_IDLE ) {
			MessageBeep(MB_ICONHAND);
			LeaveCriticalSection( &CR_MAIN );
			return SYS_ABORTED;
		}

		_i_SCH_SYSTEM_FLAG_RESET( TR_CHECKING_SIDE );
		BUTTON_SET_TR_CONTROL( CTC_DISABLE );
		BUTTON_HANDLER_REMOTE( "START TR" );

		Mode		= Signal;
		Src_Station = Transfer_IO_Reading( "CTC.TR_SRC_STATION" , &Address_TR_SRC_STATION ) + CM1;
		Src_Slot    = Transfer_IO_Reading( "CTC.TR_SRC_SLOT" , &Address_TR_SRC_SLOT );
		Src_Slot2   = Transfer_IO_Reading( "CTC.TR_SRC_SLOT2" , &Address_TR_SRC_SLOT2 );
		Trg_Station = Transfer_IO_Reading( "CTC.TR_TRG_STATION" , &Address_TR_TRG_STATION ) + CM1;
		Trg_Slot    = Transfer_IO_Reading( "CTC.TR_TRG_SLOT" , &Address_TR_TRG_SLOT );
		Trg_Slot2   = Transfer_IO_Reading( "CTC.TR_TRG_SLOT2" , &Address_TR_TRG_SLOT2 );
		Finger      = Transfer_IO_Reading( "CTC.TR_FINGER" , &Address_TR_FINGER );
	}

	Result = Transfer_Main_Checking_FEM( Mode , &Use , &Finger , &Src_Station , &Src_Slot , &Src_Slot2 , &Trg_Station , &Trg_Slot , &Trg_Slot2 , BM_Station , BM_Slot , BM_Slot2 , BM_PSlot , BM_PSlot2 , &st , &et , &DoArmMX );

	if ( Result != ERROR_NONE ) {
		ERROR_HANDLER( Result , "" );
		if ( !SDMMode ) { // 2002.05.30
			BUTTON_SET_TR_CONTROL( CTC_IDLE );
			BUTTON_HANDLER_REMOTE( "STOP TR" );
			LeaveCriticalSection( &CR_MAIN );
		}
		return SYS_ABORTED;
	}

	if ( !SDMMode ) { // 2002.05.28
		BUTTON_SET_TR_CONTROL( CTC_RUNNING );
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			SYSTEM_CANCEL_DISABLE_SET( i , FALSE );
		}
	}
	*runfinger = Finger; // 2002.05.28

	rcode = 0;
	Result = Transfer_User_Running_FEM_Code( Mode , Use , Finger , Src_Station , Src_Slot , Src_Slot2 , Trg_Station , Trg_Slot , Trg_Slot2 , BM_Station , BM_Slot , BM_Slot2 , BM_PSlot , BM_PSlot2 , st , et , DoArmMX , &rcode );

	_i_SCH_IO_MTL_SAVE();

	if ( !SDMMode ) { // 2002.05.30
		BUTTON_SET_TR_CONTROL( CTC_IDLE );
		BUTTON_HANDLER_REMOTE( "STOP TR" );
		LeaveCriticalSection( &CR_MAIN );
	}

	if ( Result == SYS_ABORTED ) {
		ERROR_HANDLER( ERROR_TRANSFER_STOP_BY_ABORT , "" );
		if ( rcode != 0 ) printf( "TRANSFER ABORT-RESULT is %d\n" , rcode ); // 2006.10.11
		return SYS_ABORTED;
	}
	if ( Result == SYS_ERROR   ) {
		ERROR_HANDLER( ERROR_TRANSFER_STOP_BY_ERROR , "" );
		if ( rcode != 0 ) printf( "TRANSFER ERROR-RESULT is %d\n" , rcode ); // 2006.10.11
		return SYS_ERROR;
	}
	return SYS_SUCCESS;
}
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
void Transfer_User_Code( int Signal ) {
	int Result;
	int Finger;
	//----------------------------------
	IO_TR_STATUS_SET( 0 , SYS_RUNNING );
	//----------------------------------
	_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 2 );
	if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
		Result = Transfer_User_FEM_Code( FALSE , Signal % 1000 , 0 , 0 , 0 , &Finger );
	}
	else {
		Result = Transfer_User_LL_Code( FALSE , Signal % 1000 , 0 , 0 , 0 , &Finger );
	}

	if ( ( Signal / 1000 ) > 0 ) {
		switch ( Signal % 1000 ) {
		case TR_PICK		:	FA_RESULT_MESSAGE( 4 , FA_TRANSFER_PICK , Result );		break;
		case TR_PLACE		:	FA_RESULT_MESSAGE( 4 , FA_TRANSFER_PLACE , Result );	break;
		case TR_MOVE		:	FA_RESULT_MESSAGE( 4 , FA_TRANSFER_MOVE , Result );		break;
		case TR_CLEAR		:	FA_RESULT_MESSAGE( 4 , FA_TRANSFER_CLEAR , Result );	break;
		}
	}
	_i_SCH_SYSTEM_USING_SET( TR_CHECKING_SIDE , 0 );
	//----------------------------------
	switch( Result ) {
	case SYS_SUCCESS : IO_TR_STATUS_SET( 0 , SYS_SUCCESS ); break;
	case SYS_ERROR   : IO_TR_STATUS_SET( 0 , SYS_ERROR ); break;
	default          : IO_TR_STATUS_SET( 0 , SYS_ABORTED ); break;
	}
	//----------------------------------
	_endthread();
}
//===================================================================================================
