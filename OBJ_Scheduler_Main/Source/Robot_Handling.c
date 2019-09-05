#include "default.h"

#include "EQ_Enum.h"

#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "sch_prm_FBTPM.h"
#include "Robot_Handling.h"

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int ROBOT_TM_LAST_ACTION_MODE[MAX_TM_CHAMBER_DEPTH];
int ROBOT_TM_LAST_ACTION_ARM[MAX_TM_CHAMBER_DEPTH];
int ROBOT_TM_LAST_ACTION_CH[MAX_TM_CHAMBER_DEPTH];
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void  ROBOT_LAST_ACTION_CONTROL( int tmside , int mode , int arm , int ch ) {
	int i;
	if ( tmside == -1 ) {
		for ( i = 0 ; i < MAX_TM_CHAMBER_DEPTH ; i++ ) {
			ROBOT_TM_LAST_ACTION_MODE[i] = mode;
			ROBOT_TM_LAST_ACTION_ARM[i] = arm;
			ROBOT_TM_LAST_ACTION_CH[i] = ch;
		}
	}
	else {
		ROBOT_TM_LAST_ACTION_MODE[tmside] = mode;
		ROBOT_TM_LAST_ACTION_ARM[tmside] = arm;
		ROBOT_TM_LAST_ACTION_CH[tmside] = ch;
	}
}

int  _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( int pickarm ) {
	//
	if ( ( pickarm / 100 ) <= 0 ) {
		switch( pickarm ) {
		case TA_STATION :	return TB_STATION; break;
		case TB_STATION :	return TA_STATION; break;
		case TC_STATION :	return TD_STATION; break;
		case TD_STATION :	return TC_STATION; break;
		default :			return pickarm+1; break;
		}
	}
	else {
		return( ( pickarm / 100 ) - 1 );
	}
	//
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
BOOL   ROBOT_GO_TARGET_COMPLETED( double Current , double Target , double Range ) {
	if ( Current > Target ) {
		if ( ( Current - Target ) <= Range ) return TRUE;
	}
	else {
		if ( ( Target - Current ) <= Range ) return TRUE;
	}
	return FALSE;
}
//-----------------------------------------------------------------------
double ROBOT_Get_FAST_FINGER_SUB( double n1 , double n2 ) {
	double distx;
	distx = n1 - n2;
	if ( distx < 0 ) return 360 + distx;
	else             return distx;
}
//-----------------------------------------------------------------------
int ROBOT_Get_FAST_FINGER_AT_STATION( int TMATM , int arm1 , int arm2 , int i , double curpos , double curmv , BOOL *same ) {
	double home , trgposA , trgposB , mvposA , mvposB , rngA , rngB , distA , distB , distx , dist2 , distA2 , distB2;

	*same = FALSE;
	
	if ( i <= 0 ) return arm1;

	mvposA  = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm1 , i , RB_ANIM_MOVE );
	mvposB  = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm2 , i , RB_ANIM_MOVE );

	if ( ( ( mvposA > mvposB ) ? ( mvposA - mvposB ) : ( mvposB - mvposA ) ) > 0.1 ) {
		distA = ( mvposA > curmv ) ? ( mvposA - curmv ) : ( curmv - mvposA );
		distB = ( mvposB > curmv ) ? ( mvposB - curmv ) : ( curmv - mvposB );
	}
	else {
		trgposA = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm1 , i , RB_ANIM_ROTATE );
		trgposB = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm2 , i , RB_ANIM_ROTATE );
		rngA	= _i_SCH_PRM_GET_RB_RNG( TMATM , arm1 , RB_ANIM_ROTATE );
		rngB	= _i_SCH_PRM_GET_RB_RNG( TMATM , arm2 , RB_ANIM_ROTATE );
		home	= _i_SCH_PRM_GET_RB_HOME_POSITION( TMATM );

		if ( ROBOT_GO_TARGET_COMPLETED( curpos , trgposA , rngA ) ) {
			if ( ROBOT_GO_TARGET_COMPLETED( curpos , trgposB , rngB ) ) *same = TRUE; // 2013.04.26
			return arm1;
		}
		if ( ROBOT_GO_TARGET_COMPLETED( curpos , trgposB , rngB ) ) return arm2;

		switch ( _i_SCH_PRM_GET_RB_ROTATION_STYLE( TMATM ) ) {
			case ROTATION_STYLE_MINIMUM :
				if ( trgposA > curpos ) distx = trgposA - curpos;
				else                    distx = curpos - trgposA;
				if ( distx > 180 )		distA = 360 - distx;
				else					distA = distx;
				if ( trgposB > curpos ) distx = trgposB - curpos;
				else                    distx = curpos - trgposB;
				if ( distx > 180 )		distB = 360 - distx;
				else					distB = distx;
				break;

			case ROTATION_STYLE_CW :
				distA = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposA );
				distB = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposB );
				break;

			case ROTATION_STYLE_CCW :
				distA = ROBOT_Get_FAST_FINGER_SUB( trgposA , curpos );
				distB = ROBOT_Get_FAST_FINGER_SUB( trgposB , curpos );
				break;

			case ROTATION_STYLE_HOME_CW :
				dist2  = ROBOT_Get_FAST_FINGER_SUB( home , curpos );
				distA2 = ROBOT_Get_FAST_FINGER_SUB( home , trgposA );
				distB2 = ROBOT_Get_FAST_FINGER_SUB( home , trgposB );
				if ( distA2 > dist2 ) distA = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposA );
				else                  distA = ROBOT_Get_FAST_FINGER_SUB( trgposA , curpos );
				if ( distB2 > dist2 ) distB = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposB );
				else                  distB = ROBOT_Get_FAST_FINGER_SUB( trgposB , curpos );
				break;

			case ROTATION_STYLE_HOME_CCW :
				dist2  = ROBOT_Get_FAST_FINGER_SUB( curpos , home );
				distA2 = ROBOT_Get_FAST_FINGER_SUB( trgposA , home );
				distB2 = ROBOT_Get_FAST_FINGER_SUB( trgposB , home );
				if ( distA2 > dist2 ) distA = ROBOT_Get_FAST_FINGER_SUB( trgposA , curpos );
				else                  distA = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposA );
				if ( distB2 > dist2 ) distB = ROBOT_Get_FAST_FINGER_SUB( trgposB , curpos );
				else                  distB = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposB );
				break;
		}
	}
	if ( distA <= distB ) return arm1;
	else                  return arm2;
}
//-----------------------------------------------------------------------
int ROBOT_Get_FAST_FINGER_AT_STATION_PLACE( int TMATM , int arm1 , int arm2 , int i , double curpos , double curmv ) {
	double home , trgposA , trgposB , mvposA , mvposB , rngA , rngB , distA , distB , distx , dist2 , distA2 , distB2;

	if ( i <= 0 ) return -1;

	mvposA  = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm1 , i , RB_ANIM_MOVE );
	mvposB  = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm2 , i , RB_ANIM_MOVE );

	if ( ( ( mvposA > mvposB ) ? ( mvposA - mvposB ) : ( mvposB - mvposA ) ) > 0.1 ) {
		distA = ( mvposA > curmv ) ? ( mvposA - curmv ) : ( curmv - mvposA );
		distB = ( mvposB > curmv ) ? ( mvposB - curmv ) : ( curmv - mvposB );
	}
	else {
		trgposA = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm1 , i , RB_ANIM_ROTATE );
		trgposB = _i_SCH_PRM_GET_RB_POSITION( TMATM , arm2 , i , RB_ANIM_ROTATE );
		rngA	= _i_SCH_PRM_GET_RB_RNG( TMATM , arm1 , RB_ANIM_ROTATE );
		rngB	= _i_SCH_PRM_GET_RB_RNG( TMATM , arm2 , RB_ANIM_ROTATE );
		home	= _i_SCH_PRM_GET_RB_HOME_POSITION( TMATM );

		if ( ROBOT_GO_TARGET_COMPLETED( curpos , trgposA , rngA ) ) {
			if ( ROBOT_GO_TARGET_COMPLETED( curpos , trgposB , rngB ) ) {
				return -1;
			}
			return arm1;
		}
		if ( ROBOT_GO_TARGET_COMPLETED( curpos , trgposB , rngB ) ) return arm2;

		switch ( _i_SCH_PRM_GET_RB_ROTATION_STYLE( TMATM ) ) {
			case ROTATION_STYLE_MINIMUM :
				if ( trgposA > curpos ) distx = trgposA - curpos;
				else                    distx = curpos - trgposA;
				if ( distx > 180 )		distA = 360 - distx;
				else					distA = distx;
				if ( trgposB > curpos ) distx = trgposB - curpos;
				else                    distx = curpos - trgposB;
				if ( distx > 180 )		distB = 360 - distx;
				else					distB = distx;
				break;

			case ROTATION_STYLE_CW :
				distA = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposA );
				distB = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposB );
				break;

			case ROTATION_STYLE_CCW :
				distA = ROBOT_Get_FAST_FINGER_SUB( trgposA , curpos );
				distB = ROBOT_Get_FAST_FINGER_SUB( trgposB , curpos );
				break;

			case ROTATION_STYLE_HOME_CW :
				dist2  = ROBOT_Get_FAST_FINGER_SUB( home , curpos );
				distA2 = ROBOT_Get_FAST_FINGER_SUB( home , trgposA );
				distB2 = ROBOT_Get_FAST_FINGER_SUB( home , trgposB );
				if ( distA2 > dist2 ) distA = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposA );
				else                  distA = ROBOT_Get_FAST_FINGER_SUB( trgposA , curpos );
				if ( distB2 > dist2 ) distB = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposB );
				else                  distB = ROBOT_Get_FAST_FINGER_SUB( trgposB , curpos );
				break;

			case ROTATION_STYLE_HOME_CCW :
				dist2  = ROBOT_Get_FAST_FINGER_SUB( curpos , home );
				distA2 = ROBOT_Get_FAST_FINGER_SUB( trgposA , home );
				distB2 = ROBOT_Get_FAST_FINGER_SUB( trgposB , home );
				if ( distA2 > dist2 ) distA = ROBOT_Get_FAST_FINGER_SUB( trgposA , curpos );
				else                  distA = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposA );
				if ( distB2 > dist2 ) distB = ROBOT_Get_FAST_FINGER_SUB( trgposB , curpos );
				else                  distB = ROBOT_Get_FAST_FINGER_SUB( curpos , trgposB );
				break;
		}
	}
	if ( ( ( distA > distB ) ? ( distA - distB ) : ( distB - distA ) ) > 1 ) {
		if ( distA <= distB ) return arm1;
		else                  return arm2;
	}
	return -1;
}
//-----------------------------------------------------------------------
int _i_SCH_ROBOT_GET_FINGER_FREE_COUNT( int TMATM ) {
	int i , c = 0;
	for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
		//
		if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
		//
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , i ) ) {
			if ( _i_SCH_MODULE_Get_TM_WAFER(TMATM,i) <= 0 ) c++;
		}
	}
	return c;
}
//
BOOL _i_SCH_ROBOT_GET_FINGER_HW_USABLE( int TMATM , int arm ) {
	return _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , arm );
}
//
int _i_SCH_ROBOT_GET_FAST_PICK_FINGER( int TMATM , int Station ) {
	int i , selarm = -1 , j;
	BOOL same;
	//
	for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
		//
		if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
		//
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , i ) ) {
			//
			if ( _i_SCH_MODULE_Get_TM_WAFER(TMATM,i) <= 0 ) {
				if ( selarm == -1 ) {
					selarm = i;
				}
				else {
					selarm = ROBOT_Get_FAST_FINGER_AT_STATION( TMATM , selarm , i , Station , ROBOT_GET_ROTATE_POSITION(TMATM) , ROBOT_GET_MOVE_POSITION(TMATM) , &same );
					//
					if ( same ) { // 2013.04.26
						//
						if      ( ROBOT_TM_LAST_ACTION_ARM[TMATM] != -1 ) {
							//
							j = (int) _i_SCH_PRM_GET_RB_HOME_POSITION( TMATM );
							//
							if ( ( j % 10 ) == 1 ) {
								selarm = _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( ROBOT_TM_LAST_ACTION_ARM[TMATM] );
							}
							//
						}
						//
					}
					//
				}
			}
			//
		}
	}
	return selarm;
}
//
int _i_SCH_ROBOT_GET_FAST_PLACE_FINGER( int TMATM , int Station ) {
	int i , selbuf , selarm = -1;
	//
	for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
		//
		if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
		//
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , i ) ) {
			//
			if ( _i_SCH_MODULE_Get_TM_WAFER(TMATM,i) > 0 ) {
				if ( selarm == -1 ) {
					selarm = i;
				}
				else {
					selbuf = ROBOT_Get_FAST_FINGER_AT_STATION_PLACE( TMATM , selarm , i , Station , ROBOT_GET_ROTATE_POSITION( TMATM ) , ROBOT_GET_MOVE_POSITION( TMATM ) );
					if ( selbuf != -1 ) selarm = selbuf;
				}
			}
			//
		}
	}
	return selarm;
}
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void ROBOT_ARM_SET_CONFIGURATION( int TMATM ) {
	int i;
	for ( i = 0 ; i < MAX_FINGER_TM ; i++ ) {
		//
		if ( i >= _i_SCH_PRM_GET_DFIX_MAX_FINGER_TM() ) break;
		//
		if ( _i_SCH_PRM_GET_RB_FINGER_ARM_STYLE( TMATM , i ) || _i_SCH_PRM_GET_RB_FINGER_ARM_DISPLAY( TMATM , i ) ) {
			ROBOT_SET_ARM_DISPLAY_STYLE( TMATM , i , 1 );
		}
		else {
			ROBOT_SET_ARM_DISPLAY_STYLE( TMATM , i , 0 );
		}
	}
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
BOOL _i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( int arm ) {
	return _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( arm / 100 , arm % 100 );
}
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void ROBOT_FM_ARM_SET_CONFIGURATION() {
	int i , j;
	for ( i = 0 ; i < 2 ; i++ ) {
		for ( j = 0 ; j < MAX_FINGER_FM ; j++ ) {
			if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( i , j ) || _i_SCH_PRM_GET_RB_FM_FINGER_ARM_DISPLAY( i , j ) ) {
				ROBOT_FM_SET_ARM_DISPLAY_STYLE( i , j , 1 );
			}
			else {
				ROBOT_FM_SET_ARM_DISPLAY_STYLE( i , j , 0 );
			}
		}
	}
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
