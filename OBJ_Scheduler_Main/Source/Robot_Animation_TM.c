#include "default.h"

#include "EQ_Enum.h"

#include "System_Tag.h"
#include "User_Parameter.h"
#include "Robot_Animation.h"
#include "FA_Handling.h"
#include "sch_CassetteResult.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "Equip_Handling.h"
#include "Robot_Handling.h"
#include "sch_estimate.h"

#include "INF_sch_CommonUser.h"

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
/*

											R1							R2
PICK

	RB_STARTING								Wait->RB_STARTING			Wait->RB_STARTING
	RB_RETRACTING							"							"
	RB_ROTATING								"							"

	RB_DOWNING								"							"
	RB_EXTENDING							"							"
	RB_UPPING								"							"


	<RB_UPDATING>							"							"

	RB_RETRACTING2							WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING
	RB_LASTING								"							"
	RB_PICKDONE								X							X

											R1							R2
PLACE

	RB_STARTING								Wait->RB_STARTING			Wait->RB_STARTING
	RB_RETRACTING							"							"
	RB_ROTATING								"							"

	RB_UPPING								"							"
	RB_EXTENDING							"							"
	RB_DOWNING								"							"


	<RB_UPDATING>							"							"

	RB_RETRACTING2							WR(P)->Wait->RB_STARTING	WR(P)->Wait->RB_STARTING
	RB_LASTING								"							"
	RB_PLACEDONE							X							X


SWITCH										R1							R2

	RB_STARTING			RB_STARTING			Wait->RB_STARTING			Wait->RB_STARTING
	RB_RETRACTING		RB_RETRACTING		"							"
	RB_ROTATING			RB_ROTATING			"							"

	RB_DOWNING			RB_DOWNING			"							"
	RB_EXTENDING		RB_EXTENDING		"							"
	RB_UPPING			RB_UPPING			"							"

	<RB_UPDATING>		<RB_UPDATING>		"							"

	RB_RETRACTING2		RB_RETRACTING2		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING

	RB_LASTING
	RB_PICKDONE

	==================

	RB_STARTING			RB_STARTING_W		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING
	RB_RETRACTING		RB_RETRACTING_W		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING
	RB_ROTATING			RB_ROTATING_W		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING

	RB_UPPING			RB_UPPING_W			WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING
	RB_EXTENDING		RB_EXTENDING_W		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING
	RB_DOWNING			RB_DOWNING_W		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING


	<RB_UPDATING>		<RB_UPDATING_W>		WR(G)->Wait->RB_STARTING	WR(G)->Wait->RB_STARTING

	RB_RETRACTING2		RB_RETRACTING2_W	WR(G,P)->Wait->RB_STARTING	WR(P)->Wait->RB_STARTING_W

	RB_LASTING			RB_LASTING			WR(G,P)->Wait->RB_STARTING	WR(P)->Wait->RB_STARTING_W
	RB_PLACEDONE		RB_SWITCHDONE		X							X

*/


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
CONTROL_ROBOT_OBJECT	ROBOT_CONTROL[MAX_TM_CHAMBER_DEPTH];
MODE_ROBOT_OBJECT		ROBOT_MODE[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_STATION[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_TM_STATION[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_TM_STATION_W[MAX_TM_CHAMBER_DEPTH]; // 2013.01.11
int						ROBOT_SLOT[2][MAX_TM_CHAMBER_DEPTH];
int						ROBOT_DEPTH[2][MAX_TM_CHAMBER_DEPTH];
int						ROBOT_ANX1_WSTS[2][MAX_TM_CHAMBER_DEPTH]; // 2015.12.15
//
int						ROBOT_SYNCH_CHECK[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_SEPERATE[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_MAINTINF[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_ANIMUSE[MAX_TM_CHAMBER_DEPTH];

int						ROBOT_TEMP_STATION[MAX_TM_CHAMBER_DEPTH];
int						ROBOT_TEMP_DEPTH[MAX_TM_CHAMBER_DEPTH];

int						ROBOT_UPDATING[MAX_TM_CHAMBER_DEPTH];

//-------------------------------------------
//----- New Append 2016.02.18 Begin
//-------------------------------------------
//
int						ROBOT_SYNCH_IGNORING[MAX_TM_CHAMBER_DEPTH]; // 2016.02.18
//
//-------------------------------------------
//----- New Append 2016.02.18 End
//-------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int ROBOT_ANIM_SUB_INDEX( int tms , int arm ) {
//	if ( ROBOT_SEPERATE[tms] ) return ROBOT_TM_STATION[tms]; // 2018.09.11
	if ( ROBOT_SEPERATE[tms] ) return arm; // 2018.09.11
	return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int ROBOT_ANIMATION_TM_RUN_HOMING( int tms , int runtype , BOOL End ) {
	//
	if ( runtype == 1 ) {
		if ( ( ROBOT_MODE[tms] != RB_MODE_UP ) && ( ROBOT_MODE[tms] != RB_MODE_DOWN ) ) {
			ROBOT_SET_EXTEND_POSITION( tms , 0 , 0 );
			ROBOT_SET_EXTEND_POSITION( tms , 1 , 0 );
			ROBOT_SET_ROTATE_POSITION( tms , _i_SCH_PRM_GET_RB_HOME_POSITION(tms) );
			ROBOT_SET_UPDOWN_POSITION( tms , 0 );
		}
		else if ( ROBOT_MODE[tms] == RB_MODE_UP ) {
			ROBOT_SET_UPDOWN_POSITION( tms , 100 );
		}
		else if ( ROBOT_MODE[tms] == RB_MODE_DOWN ) {
			ROBOT_SET_UPDOWN_POSITION( tms , 0 );
		}
	}
	//
	return 0;
}
//
int ROBOT_ANIMATION_TM_RUN_EXTENDING( int tms , int arm , int ch , int runtype , BOOL End , int mode , BOOL synch , int synchid , int synchid2 ) {
	int ss;
	double Result;
	//
	if ( ( runtype == 1 ) || ( runtype == -99 ) ) {
		if ( End ) {
			//
			//-------------------------------------------
			//----- New Append 2016.02.18 Begin
			//-------------------------------------------
			//
			if ( ROBOT_SYNCH_CHECK[tms] > 0 ) {
				//
				if ( ROBOT_SYNCH_IGNORING[tms] == 0 ) {
					//
					if ( ROBOT_GET_SYNCH_STATUS( tms ) == ROBOT_SYNCH_IGNORE ) {
						ROBOT_SYNCH_IGNORING[tms] = 1;
					}
					else {
						ROBOT_SYNCH_IGNORING[tms] = 2;
					}
					//
				}
				//
			}
			//
			//-------------------------------------------
			//----- New Append 2016.02.18 End
			//-------------------------------------------
			//
			if ( ROBOT_SYNCH_IGNORING[tms] != 1 ) { // 2016.02.18
				//
				ROBOT_SET_EXTEND_POSITION( tms , ROBOT_ANIM_SUB_INDEX( tms , arm ) , _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) );
				//
			}
			//
			return 0;
		}
		else {
			if ( synch && ( runtype != -99 ) ) {
				//
				if ( ROBOT_SYNCH_CHECK[tms] ) {
					//
					ss = ROBOT_GET_SYNCH_STATUS( tms );
					//
					if ( ss != ROBOT_SYNCH_SUCCESS ) {
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_SYNCH_IGNORING[tms] = 1;
							//
							return -1;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) return 1;
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) return 2;
						//
						if ( ss < synchid ) return -1;
						//
						if ( ( synchid2 > ROBOT_SYNCH_SUCCESS ) && ( ss > ROBOT_SYNCH_SUCCESS ) ) { // 2013.02.13
							if ( ss < synchid2 ) return -1;
						}
						//
					}
					//
				}
			}
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_GET_EXTEND_POSITION( tms , ROBOT_ANIM_SUB_INDEX( tms , arm ) ) ,
											_i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) ,
											_i_SCH_PRM_GET_RB_RNG( tms , arm , mode ) , &Result ) ) {
				//
				Result = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode );
				//
				ROBOT_SET_EXTEND_POSITION( tms , ROBOT_ANIM_SUB_INDEX( tms , arm ) , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_SET_EXTEND_POSITION( tms , ROBOT_ANIM_SUB_INDEX( tms , arm ) , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
	//
}

int ROBOT_ANIMATION_TM_RUN_ROTATING( int tms , int arm , int ch , int runtype , BOOL End , int synchid , int synchid2 ) {
	BOOL a , b;
	int Rb_Rotate , ss;
	double Result , Result2 , Buffer2 , Buffer3 , BMode;
	//
	if ( runtype == 1 ) {
		if ( End ) {
			//
			//-------------------------------------------
			//----- New Append 2016.02.18 Begin
			//-------------------------------------------
			//
			if ( ROBOT_SYNCH_CHECK[tms] > 0 ) {
				//
				if ( ROBOT_SYNCH_IGNORING[tms] == 0 ) {
					//
					if ( ROBOT_GET_SYNCH_STATUS( tms ) == ROBOT_SYNCH_IGNORE ) {
						ROBOT_SYNCH_IGNORING[tms] = 1;
					}
					else {
						ROBOT_SYNCH_IGNORING[tms] = 2;
					}
					//
				}
				//
			}
			//
			//-------------------------------------------
			//----- New Append 2016.02.18 End
			//-------------------------------------------
			//
			if ( ROBOT_SYNCH_IGNORING[tms] != 1 ) { // 2016.02.18
				//
				a = TRUE;
				ROBOT_SET_ROTATE_POSITION( tms , _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) );
				b = TRUE;
				ROBOT_SET_MOVE_POSITION( tms , _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_MOVE ) );
				//
			}
			//
			return 0;
		}
		else {
			//
			if ( ROBOT_SYNCH_CHECK[tms] ) {
				//
				ss = ROBOT_GET_SYNCH_STATUS( tms );
				//
				if ( ss != ROBOT_SYNCH_SUCCESS ) {
					//
					//-------------------------------------------
					//----- New Append 2016.02.18 Begin
					//-------------------------------------------
					if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
						//
						ROBOT_SYNCH_IGNORING[tms] = 1;
						//
						return -1;
					}
					//
					//-------------------------------------------
					//----- New Append 2016.02.18 End
					//-------------------------------------------
					//
					if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) return 1;
					if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) return 2;
					//
					if ( ss < synchid ) return -1;
					//
					if ( ( synchid2 > ROBOT_SYNCH_SUCCESS ) && ( ss > ROBOT_SYNCH_SUCCESS ) ) { // 2013.02.13
						if ( ss < synchid2 ) return -1;
					}
					//
				}
				//
			}
			//
			a = FALSE;
			b = FALSE;
			Result = ROBOT_GET_ROTATE_POSITION( tms );
			a = ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( Result ,
					_i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) ,
					_i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE ) );
			Result2 = ROBOT_GET_MOVE_POSITION( tms );
			b = ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( Result2 ,
					_i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_MOVE ) ,
					_i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_MOVE ) );
			if ( !a ) {
				Rb_Rotate = _i_SCH_PRM_GET_RB_ROTATION_STYLE(tms);
				if ( Rb_Rotate == ROTATION_STYLE_HOME_CW ) {
					Buffer2 = _i_SCH_PRM_GET_RB_HOME_POSITION(tms) - Result;
					if ( Buffer2 < 0 ) Buffer2 += 360;
					Buffer3 = _i_SCH_PRM_GET_RB_HOME_POSITION(tms) - _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE );
					if ( Buffer3 < 0 ) Buffer3 += 360;
					if ( Buffer3 > Buffer2 ) Rb_Rotate = ROTATION_STYLE_CW;
					else                     Rb_Rotate = ROTATION_STYLE_CCW;
				}
				else if ( Rb_Rotate == ROTATION_STYLE_HOME_CCW ) {
					Buffer2 = Result - _i_SCH_PRM_GET_RB_HOME_POSITION(tms);
					if ( Buffer2 < 0 ) Buffer2 += 360;
					Buffer3 = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) - _i_SCH_PRM_GET_RB_HOME_POSITION(tms);
					if ( Buffer3 < 0 ) Buffer3 += 360;
					if ( Buffer3 > Buffer2 ) Rb_Rotate = ROTATION_STYLE_CCW;
					else                     Rb_Rotate = ROTATION_STYLE_CW;
				}
				if ( Rb_Rotate == ROTATION_STYLE_MINIMUM ) {
					if ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) > Result ) Buffer2 = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) - Result;
					else                                                                          Buffer2 = Result - _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE );
					if ( Buffer2 >= 180 ) {
						if ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) > Result ) BMode = -_i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE );
						else                                                                          BMode =  _i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE );
					}
					else {
						if ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) > Result ) BMode =  _i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE );
						else                                                                          BMode = -_i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE );
					}
					Result = Result + BMode;
				}
				else if ( Rb_Rotate == ROTATION_STYLE_CCW ) {
					Result = Result + _i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE );
				}
				else {
					Result = Result - _i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE );
				}
				if      ( Result >= 360 ) Result = Result - 360;
				else if ( Result <    0 ) Result = Result + 360;
				a = ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( Result ,
						_i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) ,
						_i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_ROTATE ) );
				if ( a ) Result  = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE );
				ROBOT_SET_ROTATE_POSITION( tms , Result );
			}
			else {
				ROBOT_SET_ROTATE_POSITION( tms , _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_ROTATE ) );
			}
			if ( !b ) {
				b = ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	Result2 ,
												_i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_MOVE ) ,
												_i_SCH_PRM_GET_RB_RNG( tms , arm , RB_ANIM_MOVE ) , &Result2 );
				if ( b ) Result2 = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_MOVE );
				ROBOT_SET_MOVE_POSITION( tms , Result2 );
			}
			else {
				ROBOT_SET_MOVE_POSITION( tms , _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_MOVE ) );
			}
			//
			if ( a && b ) {
				return 0;
			}
			else {
				return -1;
			}
			//
		}
	}
	else {
		return 0;
	}
}



/*
//
//
// 2018.04.30
//
//

int ROBOT_ANIMATION_TM_RUN_UPDOWNING( int tms , int arm , int ch , int runtype , BOOL End , int mode ) {
	double Result;
	//
	if ( runtype == 1 ) {
		//
		//-------------------------------------------
		//----- New Append 2016.02.18 Begin
		//-------------------------------------------
		if ( ROBOT_SYNCH_IGNORING[tms] == 1 ) return 0;
		//-------------------------------------------
		//----- New Append 2016.02.18 End
		//-------------------------------------------
		//
		if ( End ) {
			//
			ROBOT_SET_UPDOWN_POSITION( tms , _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) );
			//
			return 0;
			//
		}
		else {
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_GET_UPDOWN_POSITION( tms ) ,
											_i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) ,
											_i_SCH_PRM_GET_RB_RNG( tms , arm , mode ) , &Result ) ) {
				//
				Result = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode );
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
}

int ROBOT_ANIMATION_TM_RUN_UPDOWNING_HALF( int tms , int arm , int ch , int runtype , BOOL End , int mode , int mode2 ) { // 2014.11.04
	double Result;
	double data; // 2014.11.04
	if ( runtype == 1 ) {
		//
		//-------------------------------------------
		//----- New Append 2016.02.18 Begin
		//-------------------------------------------
		if ( ROBOT_SYNCH_IGNORING[tms] == 1 ) return 0;
		//-------------------------------------------
		//----- New Append 2016.02.18 End
		//-------------------------------------------
		//
		data = ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) + _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode2 ) ) / 2;
		//
		if ( End ) {
			//
			ROBOT_SET_UPDOWN_POSITION( tms , data );
			//
			return 0;
			//
		}
		else {
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_GET_UPDOWN_POSITION( tms ) ,
											data ,
											_i_SCH_PRM_GET_RB_RNG( tms , arm , mode ) , &Result ) ) {
				//
				Result = data;
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
}
//
*/
//
//
// 2018.04.30
//
//

extern int ROBOT_AUTO_MULTI_UPDOWN; // 2018.04.30 // 0:x 1:FM/TM 2:FM 3:TM


int ROBOT_ANIMATION_TM_RUN_UPDOWNING( int tms , int arm , int ch , int runtype , BOOL End , int mode ) {
	double Result;
	double target , gap;
	int slot;

	if ( ( ROBOT_AUTO_MULTI_UPDOWN == 1 ) || ( ROBOT_AUTO_MULTI_UPDOWN == 3 ) ) { // 2018.04.30
		//
		gap = ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_UP ) - _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_DOWN ) );
		//
		if ( ROBOT_STATION[tms] < PM1 ) {
			if ( ROBOT_SLOT[0][tms] > 0 ) {
				slot = ROBOT_SLOT[0][tms];
			}
			else if ( ROBOT_SLOT[1][tms] > 0 ) {
				slot = ROBOT_SLOT[1][tms];
			}
			else {
				slot = 1;
			}
		}
		else {
			if ( ROBOT_DEPTH[0][tms] > 0 ) {
				slot = ROBOT_DEPTH[0][tms];
			}
			else if ( ROBOT_DEPTH[1][tms] > 0 ) {
				slot = ROBOT_DEPTH[1][tms];
			}
			else {
				slot = 1;
			}
		}
		//
		target = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) + ( (double) ( slot - 1 ) * gap );
		//
	}
	else {
		target = _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode );
	}
	//
	if ( runtype == 1 ) {
		//
		//-------------------------------------------
		//----- New Append 2016.02.18 Begin
		//-------------------------------------------
		if ( ROBOT_SYNCH_IGNORING[tms] == 1 ) return 0;
		//-------------------------------------------
		//----- New Append 2016.02.18 End
		//-------------------------------------------
		//
		if ( End ) {
			//
			ROBOT_SET_UPDOWN_POSITION( tms , target );
			//
			return 0;
			//
		}
		else {
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_GET_UPDOWN_POSITION( tms ) ,
											target ,
											_i_SCH_PRM_GET_RB_RNG( tms , arm , mode ) , &Result ) ) {
				//
				Result = target;
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
}

int ROBOT_ANIMATION_TM_RUN_UPDOWNING_HALF( int tms , int arm , int ch , int runtype , BOOL End , int mode , int mode2 ) { // 2014.11.04
	double Result;
	double target , gap;
	int slot;
	//
	target = ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode ) + _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , mode2 ) ) / 2;
	//
	if ( ( ROBOT_AUTO_MULTI_UPDOWN == 1 ) || ( ROBOT_AUTO_MULTI_UPDOWN == 3 ) ) { // 2018.04.30
		//
		gap = ( _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_UP ) - _i_SCH_PRM_GET_RB_POSITION( tms , arm , ch , RB_ANIM_DOWN ) );
		//
		if ( ROBOT_STATION[tms] < PM1 ) {
			if ( ROBOT_SLOT[0][tms] > 0 ) {
				slot = ROBOT_SLOT[0][tms];
			}
			else if ( ROBOT_SLOT[1][tms] > 0 ) {
				slot = ROBOT_SLOT[1][tms];
			}
			else {
				slot = 1;
			}
		}
		else {
			if ( ROBOT_DEPTH[0][tms] > 0 ) {
				slot = ROBOT_DEPTH[0][tms];
			}
			else if ( ROBOT_DEPTH[1][tms] > 0 ) {
				slot = ROBOT_DEPTH[1][tms];
			}
			else {
				slot = 1;
			}
		}
		//
		target = target + ( (double) ( slot - 1 ) * gap );
		//
	}
	//
	if ( runtype == 1 ) {
		//
		//-------------------------------------------
		//----- New Append 2016.02.18 Begin
		//-------------------------------------------
		if ( ROBOT_SYNCH_IGNORING[tms] == 1 ) return 0;
		//-------------------------------------------
		//----- New Append 2016.02.18 End
		//-------------------------------------------
		if ( End ) {
			//
			ROBOT_SET_UPDOWN_POSITION( tms , target );
			//
			return 0;
			//
		}
		else {
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_GET_UPDOWN_POSITION( tms ) ,
											target ,
											_i_SCH_PRM_GET_RB_RNG( tms , arm , mode ) , &Result ) ) {
				//
				Result = target;
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_SET_UPDOWN_POSITION( tms , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
}

int ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( int tms , int runtype , BOOL End , BOOL *update ) {
	int ss , wsrc , wsts , lp , dualsepchk , g , gs , gl;
	int wsre; // 2018.09.05
	int Res;
	int arm , pickplace;
	//
	*update = FALSE;
	//
	switch( ROBOT_CONTROL[tms] ) {
		case RB_HOMING :
			//
			if ( ROBOT_ANIMATION_TM_RUN_HOMING( tms , runtype , End ) == 0 ) ROBOT_CONTROL[tms]	= RB_HOMEDONE;
			//
			break;
			//
		case RB_STARTING :
			ROBOT_CONTROL[tms]	= RB_RETRACTING;
			//
		case RB_RETRACTING :
			//
			Res = ROBOT_ANIMATION_TM_RUN_EXTENDING( tms , ROBOT_TM_STATION[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_RETRACT , FALSE , 0 , 0 );
			//
			if ( Res == 0 ) {
				ROBOT_CONTROL[tms] = RB_ROTATING;
			}
			//
			if ( ROBOT_CONTROL[tms] != RB_ROTATING ) break;
			//
			if ( runtype == -99 ) break; // 2008.12.12
			//
		case RB_ROTATING :
			//
			Res = ROBOT_ANIMATION_TM_RUN_ROTATING( tms , ROBOT_TM_STATION[tms] , ROBOT_STATION[tms] , runtype , End , ROBOT_SYNCH_ROTATE_START , ( ROBOT_MODE[tms] != RB_MODE_PICKPLACE ) ? ROBOT_SYNCH_ROTATE_START_W : 0 );
			//
			if ( Res == 0 ) {
				if	    ( ROBOT_MODE[tms] == RB_MODE_EXTPIC    ) ROBOT_CONTROL[tms] = RB_EXTENDING;
				else if ( ROBOT_MODE[tms] == RB_MODE_EXTPLC    ) ROBOT_CONTROL[tms] = RB_EXTENDING;
				else if ( ROBOT_MODE[tms] == RB_MODE_EXTEND    ) ROBOT_CONTROL[tms] = RB_EXTENDING;
				else if	( ROBOT_MODE[tms] == RB_MODE_PICK      ) ROBOT_CONTROL[tms] = RB_DOWNING;
				else if	( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ROBOT_CONTROL[tms] = RB_DOWNING; // 2013.01.11
				else if ( ROBOT_MODE[tms] == RB_MODE_PLACE     ) ROBOT_CONTROL[tms] = RB_UPPING;
				else if ( ROBOT_MODE[tms] == RB_MODE_ROTATE    ) ROBOT_CONTROL[tms] = RB_ROTATEDONE;
				else										     ROBOT_CONTROL[tms] = RB_LASTING;
			}
			else if ( Res == 1 ) { // Rev1
				ROBOT_CONTROL[tms] = RB_RESETTING2;
			}
			else if ( Res == 2 ) { // Rev2
				ROBOT_CONTROL[tms] = RB_RESETTING2;
			}
			//
			break;
			//
		case RB_STARTING_W : // 2013.01.11
			ROBOT_CONTROL[tms]	= RB_RETRACTING_W;
			//
		case RB_RETRACTING_W :
			//
			Res = ROBOT_ANIMATION_TM_RUN_EXTENDING( tms , ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_RETRACT , FALSE , 0 , 0 );
			//
			if ( Res == 0 ) {
				ROBOT_CONTROL[tms] = RB_ROTATING_W;
			}
			else if ( Res == 1 ) { // Rev1
				ROBOT_CONTROL[tms] = RB_RESETTING1;
			}
			else if ( Res == 2 ) { // Rev2
				ROBOT_CONTROL[tms] = RB_RESETTING1;
			}
			//
			if ( ROBOT_CONTROL[tms] != RB_ROTATING_W ) break;
			//
			if ( runtype == -99 ) break;
			//
		case RB_ROTATING_W :
			//
			Res = ROBOT_ANIMATION_TM_RUN_ROTATING( tms , ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , ROBOT_SYNCH_ROTATE_START_W , 0 );
			//
			if ( Res == 0 ) {
				ROBOT_CONTROL[tms] = RB_UPPING_W;
			}
			else if ( Res == 1 ) { // Rev1
				ROBOT_CONTROL[tms] = RB_RESETTING1;
			}
			else if ( Res == 2 ) { // Rev2
				ROBOT_CONTROL[tms] = RB_RESETTING1;
			}
			//
			break;
			//
		case RB_DOWNING :
		case RB_DOWNING_W :
			//
//			Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_DOWNING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_DOWN ); // 2014.11.04
			//
			if ( ROBOT_CONTROL[tms] == RB_DOWNING ) { // 2014.11.04
				if ( ROBOT_MODE[tms] == RB_MODE_PLACE ) {
					Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING_HALF( tms , ( ROBOT_CONTROL[tms] == RB_DOWNING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_DOWN , RB_ANIM_UP );
				}
				else {
					Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_DOWNING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_DOWN );
				}
			}
			else {
				Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING_HALF( tms , ( ROBOT_CONTROL[tms] == RB_DOWNING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_DOWN , RB_ANIM_UP );
			}
			//
			if ( Res == 0 ) {
				if ( ROBOT_CONTROL[tms] == RB_DOWNING ) {
					if	    ( ROBOT_MODE[tms] == RB_MODE_PICK      ) ROBOT_CONTROL[tms] = RB_EXTENDING;
					else if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ROBOT_CONTROL[tms] = RB_EXTENDING; // 2013.01.11
					else if ( ROBOT_MODE[tms] == RB_MODE_PLACE     ) ROBOT_CONTROL[tms] = RB_UPDATING;
					else										     ROBOT_CONTROL[tms] = RB_LASTING;
				}
				else {
					ROBOT_CONTROL[tms] = RB_UPDATING_W;
				}
			}
			//
			break;
			//
		case RB_EXTENDING :
		case RB_EXTENDING_W :
			//
			Res = ROBOT_ANIMATION_TM_RUN_EXTENDING( tms , ( ROBOT_CONTROL[tms] == RB_EXTENDING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_EXTEND , TRUE , ( ROBOT_CONTROL[tms] == RB_EXTENDING ) ? ROBOT_SYNCH_EXTEND_START : ROBOT_SYNCH_EXTEND_START_W , ( ROBOT_MODE[tms] != RB_MODE_PICKPLACE ) ? ROBOT_SYNCH_EXTEND_START_W : 0 );
			//
			if ( ROBOT_CONTROL[tms] == RB_EXTENDING ) {
				if ( Res == 0 ) {
					if ( runtype == 3 ) { // 2015.12.16
						if	    ( ROBOT_MODE[tms] == RB_MODE_PICK      ) ROBOT_CONTROL[tms] = RB_UPPING;
						else if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ROBOT_CONTROL[tms] = RB_UPPING; // 2013.01.11
						else if ( ROBOT_MODE[tms] == RB_MODE_PLACE     ) ROBOT_CONTROL[tms] = RB_DOWNING;
						else if	( ROBOT_MODE[tms] == RB_MODE_EXTPIC    ) ROBOT_CONTROL[tms] = RB_UPDATING; // 2015.12.16
						else if ( ROBOT_MODE[tms] == RB_MODE_EXTPLC    ) ROBOT_CONTROL[tms] = RB_UPDATING; // 2015.12.16
						else										     ROBOT_CONTROL[tms] = RB_LASTING;
					}
					else {
						if	    ( ROBOT_MODE[tms] == RB_MODE_PICK      ) ROBOT_CONTROL[tms] = RB_UPPING;
						else if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ROBOT_CONTROL[tms] = RB_UPPING; // 2013.01.11
						else if ( ROBOT_MODE[tms] == RB_MODE_PLACE     ) ROBOT_CONTROL[tms] = RB_DOWNING;
						else										     ROBOT_CONTROL[tms] = RB_LASTING;
					}
				}
				else if ( Res == 1 ) { // Rev1
					ROBOT_CONTROL[tms] = RB_RESETTING2;
				}
				else if ( Res == 2 ) { // Rev2
					ROBOT_CONTROL[tms] = RB_RESETTING2;
				}
			}
			else {
				if ( Res == 0 ) {
					ROBOT_CONTROL[tms] = RB_DOWNING_W;
				}
				else if ( Res == 1 ) { // Rev1
					ROBOT_CONTROL[tms] = RB_RESETTING1;
				}
				else if ( Res == 2 ) { // Rev2
					ROBOT_CONTROL[tms] = RB_RESETTING1;
				}
			}
			//
			break;
			//
		case RB_UPPING :
		case RB_UPPING_W :
			//
//			Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_UPPING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_UP ); // 2014.11.04
			//
			if ( ROBOT_CONTROL[tms] == RB_UPPING ) { // 2014.11.04
				if	    ( ( ROBOT_MODE[tms] == RB_MODE_PICK ) || ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ) {
					Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING_HALF( tms , ( ROBOT_CONTROL[tms] == RB_UPPING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_UP , RB_ANIM_DOWN );
				}
				else {
					Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_UPPING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_UP );
				}
			}
			else {
				Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_UPPING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_UP );
			}
			//
			if ( Res == 0 ) {
				if ( ROBOT_CONTROL[tms] == RB_UPPING ) {
					if	    ( ROBOT_MODE[tms] == RB_MODE_PICK      ) ROBOT_CONTROL[tms] = RB_UPDATING;
					else if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ROBOT_CONTROL[tms] = RB_UPDATING; // 2013.01.11
					else if ( ROBOT_MODE[tms] == RB_MODE_PLACE     ) ROBOT_CONTROL[tms] = RB_EXTENDING;
					else									    	 ROBOT_CONTROL[tms] = RB_LASTING;
				}
				else {
					ROBOT_CONTROL[tms] = RB_EXTENDING_W;
				}
			}
			//
			break;
			//
		case RB_UPPING2 : // 2014.11.04
		case RB_UPPING2_W :
			//
			Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_UPPING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_UP );
			//
			if ( Res == 0 ) {
				if ( ROBOT_CONTROL[tms] == RB_UPPING2 ) {
					ROBOT_CONTROL[tms] = RB_RETRACTING2;
				}
				else {
					ROBOT_CONTROL[tms] = RB_RETRACTING2_W;
				}
			}
			//
			break;
			//
		case RB_DOWNING2 : // 2014.11.04
		case RB_DOWNING2_W :
			//
			Res = ROBOT_ANIMATION_TM_RUN_UPDOWNING( tms , ( ROBOT_CONTROL[tms] == RB_DOWNING ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_DOWN );
			//
			if ( Res == 0 ) {
				if ( ROBOT_CONTROL[tms] == RB_DOWNING2 ) {
					ROBOT_CONTROL[tms] = RB_RETRACTING2;
				}
				else {
					ROBOT_CONTROL[tms] = RB_RETRACTING2_W;
				}
			}
			//
			break;
			//=============================================================================================================================================================
			//=============================================================================================================================================================
			//=============================================================================================================================================================
			//=============================================================================================================================================================
			//=============================================================================================================================================================
		case RB_UPDATING :
		case RB_UPDATING_W :
			if ( runtype == 1 ) {
				if ( !End ) {
					//
					if ( ROBOT_CONTROL[tms] == RB_UPDATING ) { // 2013.01.11
						//
						if ( ( ROBOT_MODE[tms] != RB_MODE_PICK_UPDATE ) && ( ROBOT_MODE[tms] != RB_MODE_PLACE_UPDATE ) ) {
							if ( ROBOT_SYNCH_CHECK[tms] ) {
								//
								ss = ROBOT_GET_SYNCH_STATUS( tms );
								//
								if ( ss != ROBOT_SYNCH_SUCCESS ) {
									//
									//-------------------------------------------
									//----- New Append 2016.02.18 Begin
									//-------------------------------------------
									//
									if ( ss == ROBOT_SYNCH_IGNORE ) break; // 2016.02.18
									//
									//-------------------------------------------
									//----- New Append 2016.02.18 End
									//-------------------------------------------
									//
									if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
										ROBOT_CONTROL[tms] = RB_RESETTING2;
										break;
									}
									//
									if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
										ROBOT_CONTROL[tms] = RB_RESETTING2;
										break;
									}
									//
									if ( ss < ROBOT_SYNCH_UPDATE ) break;
									//
									if ( ( ROBOT_MODE[tms] != RB_MODE_PICKPLACE ) ) { // 2013.02.13
										//
										if ( ss > ROBOT_SYNCH_SUCCESS ) {
											//
											if ( ss < ROBOT_SYNCH_UPDATE_W ) break;
											//
										}
										//
									}
									//
								}
								//
							}
						}
						//
//						ROBOT_UPDATING[tms] = 1; // 2008.12.10 // 2018.09.27
						//
					}
					else if ( ROBOT_CONTROL[tms] == RB_UPDATING_W ) { // 2013.01.11
						//
						if ( ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ) {
							//
							if ( ROBOT_SYNCH_CHECK[tms] ) {
								//
								ss = ROBOT_GET_SYNCH_STATUS( tms );
								//
								if ( ss != ROBOT_SYNCH_SUCCESS ) {
									//
									//-------------------------------------------
									//----- New Append 2016.02.18 Begin
									//-------------------------------------------
									//
									if ( ss == ROBOT_SYNCH_IGNORE ) break; // 2016.02.18
									//
									//-------------------------------------------
									//----- New Append 2016.02.18 End
									//-------------------------------------------
									//
									if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
										ROBOT_CONTROL[tms] = RB_RESETTING1;
										break;
									}
									//
									if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
										ROBOT_CONTROL[tms] = RB_RESETTING1;
										break;
									}
									//
									if ( ss < ROBOT_SYNCH_UPDATE_W ) break;
									//
								}
								//
							}
						}
						else {
							break;
						}
						//
//						ROBOT_UPDATING[tms] = 2; // 2013.01.11 // 2018.09.27
						//
					}
					//
				}
			}
			//
			pickplace = 0;
			//
			if ( ROBOT_CONTROL[tms] == RB_UPDATING ) { // 2013.01.11
				//
				arm = ROBOT_TM_STATION[tms];
				//
//				if ( ( ROBOT_MODE[tms] == RB_MODE_PICK ) || ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) || ( ROBOT_MODE[tms] == RB_MODE_PICK_UPDATE ) ) {  // 2015.12.16
				if ( ( ROBOT_MODE[tms] == RB_MODE_PICK ) || ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) || ( ROBOT_MODE[tms] == RB_MODE_EXTPIC ) || ( ROBOT_MODE[tms] == RB_MODE_PICK_UPDATE ) ) {  // 2015.12.16
					pickplace = 1;
				}
//				else if ( ( ROBOT_MODE[tms] == RB_MODE_PLACE ) || ( ROBOT_MODE[tms] == RB_MODE_PLACE_UPDATE ) ) {  // 2015.12.16
				else if ( ( ROBOT_MODE[tms] == RB_MODE_PLACE ) || ( ROBOT_MODE[tms] == RB_MODE_EXTPLC ) || ( ROBOT_MODE[tms] == RB_MODE_PLACE_UPDATE ) ) {  // 2015.12.16
					pickplace = 2;
				}
				//
				ROBOT_UPDATING[tms] = 1; // 2018.09.27
				//
			}
			else if ( ROBOT_CONTROL[tms] == RB_UPDATING_W ) {
				if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) { 
					pickplace = 2;
					arm = ROBOT_TM_STATION_W[tms];
					//
					ROBOT_UPDATING[tms] = 2; // 2018.09.27
					//
				}
			}
			//
			if ( pickplace == 1 ) { 
				if ( ROBOT_STATION[tms] < PM1 ) {
					for ( lp = 0 ; lp < 2 ; lp++ ) {
						if ( ROBOT_SLOT[lp][tms] > 0 ) {
							//
							if ( runtype == 1 ) { // 2015.12.15
								switch( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] ) ) {
								case CWM_UNKNOWN :
								case CWM_ABSENT :
									wsts = 0;
									break;
								default :
									wsts = ROBOT_SLOT[lp][tms];
								}
							}
							else { // 2015.12.15
								wsts = ROBOT_ANX1_WSTS[lp][tms];
							}
							//
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] );
							//
//							if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , ROBOT_STATION[tms] , wsts , tms , arm );
							}
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , ROBOT_STATION[tms] - 1 , wsts , -1 , wsre ); // 2007.08.10
							}
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
//								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , ROBOT_STATION[tms] - 1 + CM1 , ROBOT_SLOT[lp][tms] , tms ); // 2015.05.28
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , ROBOT_STATION[tms] - 1 + CM1 , ROBOT_SLOT[lp][tms] , tms ); // 2015.05.28
							}
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								_i_SCH_IO_SET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , CWM_ABSENT );
							}
						}
					}
					if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_STATION[tms] , tms + 1 , arm , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] ); /* 2007.03.14 */
					}
				}
				else if ( ( ROBOT_STATION[tms] >= BM1 ) && ( ROBOT_STATION[tms] < TM ) ) {
					for ( lp = 0 ; lp < 2 ; lp++ ) {
						if ( ROBOT_DEPTH[lp][tms] > 0 ) {
							//
							if ( runtype == 1 ) { // 2015.12.15
								//-----------------------------
								IO_WAFER_DATA_GET_TO_BM( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , &wsrc , &wsts , &wsre ); // 2007.08.10
								//-----------------------------
							}
							else { // 2015.12.15
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] );
								wsts = ROBOT_ANX1_WSTS[lp][tms];
							}
							//
//							if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , wsrc + CM1 , wsts , tms , arm );
//								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms ); // 2013.04.29 // 2015.05.28
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms ); // 2013.04.29 // 2015.05.28
							}
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							/*
							// 2013.04.29
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms );
							}
							*/
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_BM( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , -1 , 0 , -1 , -1 ); // 2007.07.25
							}
						}
					}
					if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_STATION[tms] , tms + 1 , arm , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] ); /* 2007.03.14 */
					}
				}
				else {
					for ( lp = 0 ; lp < 2 ; lp++ ) {
						if ( ROBOT_DEPTH[lp][tms] > 0 ) {
							//=========================================================================
							// 2007.10.29
							//=========================================================================
							if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2007.10.29
								dualsepchk = TRUE;
								if ( lp == 0 ) {
									ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
									ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
								}
								else {
									if ( ( ( ROBOT_STATION[tms] - PM1 ) % 2 ) == 0 ) {
										ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms] + 1;
									}
									else {
										ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
									}
									ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms] - 1;
								}
							}
							else {
								dualsepchk = FALSE;
								ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
								ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
							}
							//
//_IO_CIM_PRINTF( "1.[runtype=%d][lp=%d][ST,DP=%d,%d][TST,TDP=%d,%d]\n" , runtype , lp , ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] );
							//
							if ( runtype == 1 ) { // 2015.12.15
								wsts = _i_SCH_IO_GET_MODULE_STATUS( ROBOT_TEMP_STATION[tms] , ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ? 1 : ROBOT_TEMP_DEPTH[tms] );
							}
							else { // 2015.12.15
								wsts = ROBOT_ANX1_WSTS[lp][tms];
							}
							//
//_IO_CIM_PRINTF( "2.[runtype=%d][wsts=%d][arm=%d]\n" , runtype , wsts , arm );
							//=========================================================================
							if ( wsts > 0 ) { // 2015.12.15
								//=========================================================================
								if ( lp == 0 ) {
									wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ? 1 : ROBOT_TEMP_DEPTH[tms] ); /* 2007.05.02 */
									wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ? 1 : ROBOT_TEMP_DEPTH[tms] );
								}
								else {
									if ( dualsepchk ) { // 2007.10.29
										wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 );
										wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 );
									}
									else {
										if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) ) { /* 2007.03.20 */
											wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ); /* 2007.03.20 */
											wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] );
										}
										else {
											wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 );
											wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 );
										}
									}
								}
//_IO_CIM_PRINTF( "3.[runtype=%d][wsrc=%d][wsts=%d]\n" , runtype , wsrc , wsts );
								//=========================================================================
//								if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18

//_IO_CIM_PRINTF( "4.[ST,SL=%d,%d][SST,SSL=%d,%d][Res=%d][ResT=%d]\n" , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , _i_SCH_IO_GET_MODULE_RESULT( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) , WAFER_RESULT_IN_TM( tms , arm , lp ) );


									SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , tms , arm );

//_IO_CIM_PRINTF( "5.[ST,SL=%d,%d][SST,SSL=%d,%d][Res=%d][ResT=%d]\n" , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , _i_SCH_IO_GET_MODULE_RESULT( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) , WAFER_RESULT_IN_TM( tms , arm , lp ) );

									//=========================================================================
									// 2013.04.29
									if ( lp == 0 ) {
//										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms ); // 2015.05.27
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms ); // 2015.05.27
//_IO_CIM_PRINTF( "6.[ST,SL=%d,%d][SST,SSL=%d,%d][Res=%d][ResT=%d]\n" , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , _i_SCH_IO_GET_MODULE_RESULT( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) , WAFER_RESULT_IN_TM( tms , arm , lp ) );
									}
									else {
										if ( ( ROBOT_DEPTH[0][tms] > 0 ) && ( wsrc == ( MAX_CASSETTE_SIDE + 1 ) ) ) { /* 2007.04.25 */
											/* SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , wsts , tms ); */
//_IO_CIM_PRINTF( "7.[ST,SL=%d,%d][SST,SSL=%d,%d][Res=%d][ResT=%d]\n" , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , _i_SCH_IO_GET_MODULE_RESULT( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) , WAFER_RESULT_IN_TM( tms , arm , lp ) );
										}
										else {
//											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms ); // 2015.05.27
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms ); // 2015.05.27
//_IO_CIM_PRINTF( "8.[ST,SL=%d,%d][SST,SSL=%d,%d][Res=%d][ResT=%d]\n" , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , _i_SCH_IO_GET_MODULE_RESULT( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) , WAFER_RESULT_IN_TM( tms , arm , lp ) );
										}
									}
									//=========================================================================

								}
								//=========================================================================
								if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
									IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , wsrc , wsts , -1 , wsre ); // 2007.08.10
								}
								/*
								// 2013.04.29
								if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
									//=========================================================================
									if ( lp == 0 ) {
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms );
									}
									else {
										if ( ( ROBOT_DEPTH[0][tms] > 0 ) && ( wsrc == ( MAX_CASSETTE_SIDE + 1 ) ) ) { // 2007.04.25
											// SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER() , wsts , tms );
										}
										else {
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms );
										}
									}
									//=========================================================================
								}
								*/
							}
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
							}
						}
					}
					if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_STATION[tms] , tms + 1 , arm , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] ); /* 2007.03.14 */
					}
				}
				//
				if ( ROBOT_ANIMUSE[tms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( tms , MACRO_PICK , FALSE , runtype , ROBOT_STATION[tms] , arm , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] ); // 2009.07.16
				//
			}
			else if ( pickplace == 2 ) {
				if ( ROBOT_STATION[tms] < PM1 ) {
					for ( lp = 0 ; lp < 2 ; lp++ ) {
						if ( ROBOT_SLOT[lp][tms] > 0 ) {
							//
							//===========================================================
							if ( lp == 0 ) {
								wsrc = WAFER_SOURCE_IN_TM( tms , arm );
								wsre = WAFER_SOURCE_IN_TM_E( tms , arm );
							}
							else {
								wsrc = WAFER_SOURCE_IN_TM2( tms , arm );
								wsre = WAFER_SOURCE_IN_TM2_E( tms , arm );
							}
							//===========================================================
							//
							if ( runtype == 1 ) { // 2015.12.15
								if ( lp == 0 ) {
									wsts = WAFER_STATUS_IN_TM( tms , arm );
								}
								else {
									wsts = WAFER_STATUS_IN_TM2( tms , arm );
								}
							}
							else { // 2015.12.15
								wsts = ROBOT_ANX1_WSTS[lp][tms];
							}
							//

// 2015.12.15
							//===========================================================
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , lp , ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , tms , arm ); // 2015.06.10
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
							}
							//===========================================================
							if ( wsts > 0 ) {
								if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
									_i_SCH_IO_SET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , CWM_PRESENT );
								}
							}
							//===========================================================
//							if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2019.01.11
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2008.02.26 // 2019.01.11
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , wsrc + CM1 , wsts , tms );
							}
							//===========================================================
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
							//===========================================================

/*
// 2015.12.15
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								//===========================================================
								if ( lp == 0 ) {
									wsrc = WAFER_SOURCE_IN_TM( tms , arm ); // 2008.02.26
									wsts = WAFER_STATUS_IN_TM( tms , arm );
								}
								else {
									wsrc = WAFER_SOURCE_IN_TM2( tms , arm ); // 2008.02.26
									wsts = WAFER_STATUS_IN_TM2( tms , arm );
								}
								//===========================================================
//								if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
//									SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , 0 , ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , tms , arm ); // 2015.06.10
//									SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
									SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , lp , ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , tms , arm ); // 2015.06.10
									SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
								}
								//===========================================================
								if ( wsts > 0 ) {
									_i_SCH_IO_SET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , CWM_PRESENT );
									//===========================================================
									if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , wsrc + CM1 , wsts , tms );
									}
									//===========================================================
								}
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 ); // 2007.08.10
								//===========================================================
							}
*/
						}
					}
					if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_STATION[tms] , tms + 1 , arm , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] ); /* 2007.03.14 */
					}
				}
				else if ( ( ROBOT_STATION[tms] >= BM1 ) && ( ROBOT_STATION[tms] < TM ) ) {
					for ( lp = 0 ; lp < 2 ; lp++ ) {
						if ( ROBOT_DEPTH[lp][tms] > 0 ) {
							//
							//===========================================================
							if ( lp == 0 ) {
								wsrc = WAFER_SOURCE_IN_TM( tms , arm );
								wsre = WAFER_SOURCE_IN_TM_E( tms , arm );
							}
							else {
								wsrc = WAFER_SOURCE_IN_TM2( tms , arm );
								wsre = WAFER_SOURCE_IN_TM2_E( tms , arm );
							}
							//===========================================================
							//
							if ( runtype == 1 ) { // 2015.12.15
								if ( lp == 0 ) {
									wsts = WAFER_STATUS_IN_TM( tms , arm );
								}
								else {
									wsts = WAFER_STATUS_IN_TM2( tms , arm );
								}
							}
							else { // 2015.12.15
								wsts = ROBOT_ANX1_WSTS[lp][tms];
							}
							//
							//-----------------------------
//							if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
//								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , wsrc + CM1 , wsts , -1 ); // 2013.04.29
							}
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_BM( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , wsrc , wsts , -1 , wsre );
							}
							/*
							// 2013.04.29
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , wsrc + CM1 , wsts , -1 );
							}
							*/
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
						}
					}
					if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_STATION[tms] , tms + 1 , arm , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] ); /* 2007.03.14 */
					}
				}
				else {
					for ( lp = 0 ; lp < 2 ; lp++ ) {
						if ( ROBOT_DEPTH[lp][tms] > 0 ) {
							//=========================================================================
							// 2007.10.29
							//=========================================================================
							if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2007.10.29
								dualsepchk = TRUE;
								if ( lp == 0 ) {
									ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
									ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
								}
								else {
									if ( ( ( ROBOT_STATION[tms] - PM1 ) % 2 ) == 0 ) {
										ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms] + 1;
									}
									else {
										ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
									}
									ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms] - 1;
								}
							}
							else {
								dualsepchk = FALSE;
								ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
								ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
							}
							//=========================================================================
							//
							if ( lp == 0 ) {
								wsrc = WAFER_SOURCE_IN_TM( tms , arm );
								wsre = WAFER_SOURCE_IN_TM_E( tms , arm );
							}
							else {
								wsrc = WAFER_SOURCE_IN_TM2( tms , arm );
								wsre = WAFER_SOURCE_IN_TM2_E( tms , arm );
							}
							//===========================================================
							//
							if ( runtype == 1 ) { // 2015.12.15
								if ( lp == 0 ) {
									wsts = WAFER_STATUS_IN_TM( tms , arm );
								}
								else {
									wsts = WAFER_STATUS_IN_TM2( tms , arm );
								}
							}
							else { // 2015.12.15
								wsts = ROBOT_ANX1_WSTS[lp][tms];
							}
							//
							//-----------------------------
//							if ( ROBOT_MAINTINF[tms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
//								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms , arm ); // 2015.06.10
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , -1 ); // 2013.04.29
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								//============================================================================================
								if ( lp == 0 ) {
									_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] , wsrc ); /* 2007.05.02 */
									_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] , wsre );
								}
								else {
									if ( dualsepchk ) { // 2007.10.29
										_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 , wsrc );
										_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 , wsre );
									}
									else {
//										if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , 2 ) ) { /* 2007.03.20 */ // 2013.04.29
										if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) ) { /* 2007.03.20 */ // 2013.04.29
//											_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 2 , wsrc ); /* 2007.03.20 */ // 2013.04.29
											_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc ); /* 2007.03.20 */ // 2013.04.29
											_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsre );
										}
										else {
											_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 , wsrc );
											_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 , wsre );
										}
									}
								}
								//============================================================================================
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , -1 , wsts , -1 , -1 ); // 2007.08.10
								//-----------------------------
							}
							//-----------------------------
							/*
							// 2013.04.29
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , -1 );
							}
							*/
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
						}
					}
					if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
						_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_STATION[tms] , tms + 1 , arm , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] ); /* 2007.03.14 */
					}
				}
				//
				if ( ROBOT_ANIMUSE[tms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( tms , MACRO_PLACE , FALSE , runtype , ROBOT_STATION[tms] , arm , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] ); // 2009.07.16
				//
			}
			//
			if ( ROBOT_CONTROL[tms] == RB_UPDATING ) { // 2013.01.11
				//
				if ( ( ROBOT_MODE[tms] == RB_MODE_PICK_UPDATE ) || ( ROBOT_MODE[tms] == RB_MODE_PLACE_UPDATE ) ) {
					ROBOT_CONTROL[tms] = RB_LASTING;
				}
				else if ( ( ROBOT_MODE[tms] == RB_MODE_EXTPIC ) || ( ROBOT_MODE[tms] == RB_MODE_EXTPLC ) ) { // 2015.12.16
					ROBOT_CONTROL[tms] = RB_LASTING;
				}
				else {
//					ROBOT_CONTROL[tms] = RB_RETRACTING2; // 2014.11.04
					//
					if ( ROBOT_MODE[tms] == RB_MODE_PICK ) { // 2014.11.04 
						ROBOT_CONTROL[tms] = RB_UPPING2;
					}
					else {
						ROBOT_CONTROL[tms] = RB_DOWNING2;
					}
				}
				//
			}
			else if ( ROBOT_CONTROL[tms] == RB_UPDATING_W ) { // 2013.01.11
				//
				if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) { 
					//
//					ROBOT_CONTROL[tms] = RB_RETRACTING2_W; // 2014.11.04
					//
					ROBOT_CONTROL[tms] = RB_DOWNING2_W; // 2014.11.04
					//
				}
				//
			}
			//
			*update = TRUE;
			break;
			//=============================================================================================================================================================
			//=============================================================================================================================================================
			//=============================================================================================================================================================
			//=============================================================================================================================================================
			//=============================================================================================================================================================
		case RB_RETRACTING2 :
		case RB_RETRACTING2_W :
			//
			Res = ROBOT_ANIMATION_TM_RUN_EXTENDING( tms , ( ROBOT_CONTROL[tms] == RB_RETRACTING2 ) ? ROBOT_TM_STATION[tms] : ROBOT_TM_STATION_W[tms] , ROBOT_STATION[tms] , runtype , End , RB_ANIM_RETRACT , TRUE , ( ROBOT_CONTROL[tms] == RB_RETRACTING2 ) ? ROBOT_SYNCH_RETRACT_START : ROBOT_SYNCH_RETRACT_START_W , ( ROBOT_MODE[tms] != RB_MODE_PICKPLACE ) ? ROBOT_SYNCH_RETRACT_START_W : 0 );
			//
			if ( Res == 0 ) {
				if ( ROBOT_CONTROL[tms] == RB_RETRACTING2 ) { // 2013.01.11
					if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) {
						ROBOT_CONTROL[tms] = RB_STARTING_W;
					}
					else {
						ROBOT_CONTROL[tms] = RB_LASTING;
					}
				}
				else {
					ROBOT_CONTROL[tms] = RB_LASTING;
				}
			}
			else if ( Res == 1 ) { // Rev1
				ROBOT_CONTROL[tms] = RB_RESETTING1;
			}
			else if ( Res == 2 ) { // Rev2
				ROBOT_CONTROL[tms] = RB_RESETTING1_W;
			}
			//
			break;
			//
		case RB_LASTING :
			if ( runtype == 1 ) {
				if ( !End ) {
					if ( ( ROBOT_MODE[tms] != RB_MODE_PICK_UPDATE ) && ( ROBOT_MODE[tms] != RB_MODE_PLACE_UPDATE ) ) {
						if ( ROBOT_SYNCH_CHECK[tms] ) {
							//
							ss = ROBOT_GET_SYNCH_STATUS( tms );
							//
							if ( ss != ROBOT_SYNCH_SUCCESS ) {
								//
								//-------------------------------------------
								//----- New Append 2016.02.18 Begin
								//-------------------------------------------
								//
								if ( ss == ROBOT_SYNCH_IGNORE ) break; // 2016.02.18
								//
								//-------------------------------------------
								//----- New Append 2016.02.18 End
								//-------------------------------------------
								//
								if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
									ROBOT_CONTROL[tms] = RB_RESETTING1;
									break;
								}
								if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
									if ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) { // 2013.01.11
										ROBOT_CONTROL[tms] = RB_RESETTING1_W;
									}
									else {
										ROBOT_CONTROL[tms] = RB_RESETTING1;
									}
									break;
								}
								//
								break;
							}
							//
						}
					}
				}
			}
			//
			switch ( ROBOT_MODE[tms] ) {
			case RB_MODE_HOME			:	ROBOT_CONTROL[tms]	= RB_HOMEDONE;		break;
			case RB_MODE_PICK			:	ROBOT_CONTROL[tms]	= RB_PICKDONE;		break;
			case RB_MODE_PICKPLACE		:	ROBOT_CONTROL[tms]	= RB_PICKPLACEDONE;		break; // 2013.01.11
			case RB_MODE_PLACE			:	ROBOT_CONTROL[tms]	= RB_PLACEDONE;		break;
			case RB_MODE_RETPIC			:	ROBOT_CONTROL[tms]	= RB_RETRACTDONE;	break;
			case RB_MODE_RETPLC			:	ROBOT_CONTROL[tms]	= RB_RETRACTDONE;	break;
			case RB_MODE_RETRACT		:	ROBOT_CONTROL[tms]	= RB_RETRACTDONE;	break;
			case RB_MODE_EXTPIC			:	ROBOT_CONTROL[tms]	= RB_EXTENDDONE;	break;
			case RB_MODE_EXTPLC			:	ROBOT_CONTROL[tms]	= RB_EXTENDDONE;	break;
			case RB_MODE_EXTEND			:	ROBOT_CONTROL[tms]	= RB_EXTENDDONE;	break;
			case RB_MODE_ROTATE			:	ROBOT_CONTROL[tms]	= RB_ROTATEDONE;	break;
			case RB_MODE_PICK_UPDATE	:	ROBOT_CONTROL[tms]	= RB_UPDATEDONE;		break;
			case RB_MODE_PLACE_UPDATE	:	ROBOT_CONTROL[tms]	= RB_UPDATEDONE;		break;
			}
			break;
			//
		case RB_RESETTING1 :
		case RB_RESETTING1_W :
			//
			gs = 0;
			gl = 0;
			//
			if ( ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ) { // 2013.01.11
				if ( ROBOT_CONTROL[tms] == RB_RESETTING1 ) {
					if ( ROBOT_UPDATING[tms] == 2 ) {
						gs = 0;
						gl = 2;
					}
					else if ( ROBOT_UPDATING[tms] == 1 ) {
						gs = 1;
						gl = 2;
					}
					//
					ROBOT_CONTROL[tms] = RB_RESETTING2;
					//
				}
				else {
					if ( ROBOT_UPDATING[tms] == 2 ) {
						gs = 0;
						gl = 1;
					}
					//
					ROBOT_CONTROL[tms] = RB_RESETTING2_W;
					//
				}
			}
			else {
				//
				gs = 1;
				gl = 2;
				//
				ROBOT_CONTROL[tms] = RB_RESETTING2;
				//
			}
			//
			for ( g = gs ; g < gl ; g++ ) {
				//=====================================================================================================================
				pickplace = 0;
				//
				if ( g == 0 ) {
					//
					pickplace = 2;
					//
					arm = ROBOT_TM_STATION_W[tms];
					//
				}
				else {
					//
					arm = ROBOT_TM_STATION[tms];
					//
					if ( ( ROBOT_MODE[tms] == RB_MODE_PICKPLACE ) ) {
						//
						pickplace = 1;
						//
					}
					else {
						//
						if ( ( ROBOT_MODE[tms] == RB_MODE_PICK ) || ( ROBOT_MODE[tms] == RB_MODE_PICK_UPDATE ) ) { 
							pickplace = 1;
						}
						else if ( ( ROBOT_MODE[tms] == RB_MODE_PLACE ) || ( ROBOT_MODE[tms] == RB_MODE_PLACE_UPDATE ) ) {
							pickplace = 2;
						}
						//

					}
				}
				//=====================================================================================================================
				//
				if ( pickplace == 1 ) {
					//
					if ( ROBOT_STATION[tms] < PM1 ) {
						for ( lp = 0 ; lp < 2 ; lp++ ) {
							if ( ROBOT_SLOT[lp][tms] > 0 ) {
								//===========================================================================
								if ( lp == 0 ) {
									wsts = WAFER_STATUS_IN_TM( tms , arm );
								}
								else {
									wsts = WAFER_STATUS_IN_TM2( tms , arm );
								}
								//===========================================================================
								if ( wsts > 0 ) _i_SCH_IO_SET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , CWM_PRESENT );
								//-----------------------------
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
							}
						}
					}
					else if ( ( ROBOT_STATION[tms] >= BM1 ) && ( ROBOT_STATION[tms] < TM ) ) {
						for ( lp = 0 ; lp < 2 ; lp++ ) {
							if ( ROBOT_DEPTH[lp][tms] > 0 ) {
								//-----------------------------
								IO_WAFER_DATA_GET_TO_TM( tms , arm , lp , &wsrc , &wsts , &wsre ); // 2007.08.10
								//-----------------------------
								IO_WAFER_DATA_SET_TO_BM( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , wsrc , wsts , -1 , wsre );
								//-----------------------------
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , wsrc + CM1 , wsts , -1 );
								//-----------------------------
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
							}
						}
					}
					else {
						for ( lp = 0 ; lp < 2 ; lp++ ) {
							if ( ROBOT_DEPTH[lp][tms] > 0 ) {
								//=========================================================================
								// 2007.10.29
								//=========================================================================
								if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2007.10.29
									dualsepchk = TRUE;
									if ( lp == 0 ) {
										ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
										ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
									}
									else {
										if ( ( ( ROBOT_STATION[tms] - PM1 ) % 2 ) == 0 ) {
											ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms] + 1;
										}
										else {
											ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
										}
										ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms] - 1;
									}
								}
								else {
									dualsepchk = FALSE;
									ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
									ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
								}
								//=========================================================================
								//
								//=========================================================================
								//=========================
								IO_WAFER_DATA_GET_TO_TM( tms , arm , lp , &wsrc , &wsts , &wsre ); // 2007.08.10
								//====================================================================================================
								if ( lp == 0 ) {
									_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] , wsrc ); /* 2007.05.02 */
									_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] , wsre );
								}
								else {
									if ( dualsepchk ) { // 2007.10.29
										_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 , wsrc );
										_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 , wsre );
									}
									else {
//										if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , 2 ) ) { /* 2007.03.20 */ // 2013.04.29
										if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) ) { /* 2007.03.20 */ // 2013.04.29
//											_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 2 , wsrc ); /* 2007.03.20 */ // 2013.04.29
											_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc ); /* 2007.03.20 */ // 2013.04.29
											_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsre );
										}
										else {
											_i_SCH_IO_SET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 , wsrc );
											_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 , wsre );
										}
									}
								}
								//====================================================================================================
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , -1 , wsts , -1 , -1 ); // 2007.08.10
								//-----------------------------
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , wsrc + CM1 , wsts , -1 );
								//-----------------------------
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
							}
						}
					}
					//
					if ( ROBOT_ANIMUSE[tms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( tms , MACRO_PICK , TRUE , runtype , ROBOT_STATION[tms] , arm , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] ); // 2009.07.16
					//
				}
				else if ( pickplace == 2 ) {
					if ( ROBOT_STATION[tms] < PM1 ) {
						for ( lp = 0 ; lp < 2 ; lp++ ) {
							if ( ROBOT_SLOT[lp][tms] > 0 ) {
								//-----------------------------
								//
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] );
								//
								//-----------------------------
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , ROBOT_STATION[tms] - 1 , ROBOT_SLOT[lp][tms] , -1 , wsre ); // 2007.08.10
								//-----------------------------
//								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , ROBOT_STATION[tms] - 1 + CM1 , ROBOT_SLOT[lp][tms] , tms ); // 2015.05.27
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , ROBOT_STATION[tms] - 1 + CM1 , ROBOT_SLOT[lp][tms] , tms ); // 2015.05.27
								//-----------------------------
								_i_SCH_IO_SET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] , CWM_ABSENT );
								//-----------------------------
							}
						}
					}
					else if ( ( ROBOT_STATION[tms] >= BM1 ) && ( ROBOT_STATION[tms] < TM ) ) {
						for ( lp = 0 ; lp < 2 ; lp++ ) {
							if ( ROBOT_DEPTH[lp][tms] > 0 ) {
								//-----------------------------
								IO_WAFER_DATA_GET_TO_BM( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , &wsrc , &wsts , &wsre ); // 2007.08.10
								//-----------------------------
								IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , wsrc , wsts , -1 , wsre ); // 2007.08.10
								//-----------------------------
//								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms ); // 2015.05.28
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms ); // 2015.05.28
								//-----------------------------
								IO_WAFER_DATA_SET_TO_BM( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] , -1 , 0 , -1 , -1 ); // 2007.07.25
								//-----------------------------
							}
						}
					}
					else {
						for ( lp = 0 ; lp < 2 ; lp++ ) {
							if ( ROBOT_DEPTH[lp][tms] > 0 ) {
								//=========================================================================
								// 2007.10.29
								//=========================================================================
								if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) { // 2007.10.29
									dualsepchk = TRUE;
									if ( lp == 0 ) {
										ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
										ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
									}
									else {
										if ( ( ( ROBOT_STATION[tms] - PM1 ) % 2 ) == 0 ) {
											ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms] + 1;
										}
										else {
											ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
										}
										ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms] - 1;
									}
								}
								else {
									dualsepchk = FALSE;
									ROBOT_TEMP_STATION[tms] = ROBOT_STATION[tms];
									ROBOT_TEMP_DEPTH[tms] = ROBOT_DEPTH[lp][tms];
								}
								//=========================================================================
								//=========================================================================
								if ( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] ) != 0 ) {
									//=============================================================================================================
									if ( lp == 0 ) {
										wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] ); /* 2007.05.02 */
										wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] );
									}
									else {
										if ( dualsepchk ) { // 2007.10.29
											wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 );
											wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 );
										}
										else {
//											if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , 2 ) ) { /* 2007.03.20 */ // 2013.04.29
											if ( WAFER_SOURCE_IO_CHECK( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ) ) { /* 2007.03.20 */ // 2013.04.29
//												wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 2 ); /* 2007.03.20 */ // 2013.04.29
												wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] ); /* 2007.03.20 */ // 2013.04.29
												wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] );
											}
											else {
												wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_TEMP_STATION[tms] , 1 );
												wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_TEMP_STATION[tms] , 1 );
											}
										}
									}
									//=============================================================================================================
									//-----------------------------
									wsts = _i_SCH_IO_GET_MODULE_STATUS( ROBOT_TEMP_STATION[tms] , ( ( ROBOT_TEMP_STATION[tms] == AL ) || ( ROBOT_TEMP_STATION[tms] == IC ) ) ? 1 : ROBOT_TEMP_DEPTH[tms] );
									//-----------------------------
									IO_WAFER_DATA_SET_TO_TM( tms , arm , lp , wsrc , wsts , -1 , wsre ); // 2007.08.10
									//-----------------------------
//									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , 0 , wsrc + CM1 , wsts , tms ); // 2015.05.28
									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 2 , arm , lp , wsrc + CM1 , wsts , tms ); // 2015.05.28
									//-----------------------------
								}
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( ROBOT_TEMP_STATION[tms] , ROBOT_TEMP_DEPTH[tms] , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
							}
						}
					}
					//
					if ( ROBOT_ANIMUSE[tms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( tms , MACRO_PLACE , TRUE , runtype , ROBOT_STATION[tms] , arm , ( ROBOT_SLOT[1][tms] * 100 ) + ROBOT_SLOT[0][tms] , ( ROBOT_DEPTH[1][tms] * 100 ) + ROBOT_DEPTH[0][tms] ); // 2009.07.16
					//
				}
				//
				*update = TRUE;
			}
			//
			if ( runtype == -99 ) break; // 2008.12.10
			//
		case RB_RESETTING2 :
		case RB_RESETTING2_W : // 2013.01.11
			//
			ss = ROBOT_GET_SYNCH_STATUS( tms );
			//
			if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
				if ( ROBOT_CONTROL[tms]	== RB_RESETTING2_W ) {
					ROBOT_CONTROL[tms]	= RB_RESETTING1;
				}
				break;
			}
			if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) break;
			//
			switch ( ROBOT_MODE[tms] ) {
			case RB_MODE_HOME			:	ROBOT_CONTROL[tms]	= RB_HOMING;		break;
			case RB_MODE_PICK			:	ROBOT_CONTROL[tms]	= RB_STARTING;		break;
			case RB_MODE_PICKPLACE		:
				if ( ROBOT_CONTROL[tms]	== RB_RESETTING2_W ) {
					ROBOT_CONTROL[tms]	= RB_STARTING_W;
				}
				else {
					ROBOT_CONTROL[tms]	= RB_STARTING;
				}
				break;
			case RB_MODE_PLACE			:	ROBOT_CONTROL[tms]	= RB_STARTING;		break;
			case RB_MODE_RETPIC			:	ROBOT_CONTROL[tms]	= RB_RETRACTING2;	break;
			case RB_MODE_RETPLC			:	ROBOT_CONTROL[tms]	= RB_RETRACTING2;	break;
			case RB_MODE_RETRACT		:	ROBOT_CONTROL[tms]	= RB_RETRACTING2;	break;
			case RB_MODE_EXTPIC			:	ROBOT_CONTROL[tms]	= RB_STARTING;		break;
			case RB_MODE_EXTPLC			:	ROBOT_CONTROL[tms]	= RB_STARTING;		break;
			case RB_MODE_EXTEND			:	ROBOT_CONTROL[tms]	= RB_STARTING;		break;
			case RB_MODE_ROTATE			:	ROBOT_CONTROL[tms]	= RB_ROTATING;		break;
			case RB_MODE_PICK_UPDATE	:	ROBOT_CONTROL[tms]	= RB_UPDATING;		break;
			case RB_MODE_PLACE_UPDATE	:	ROBOT_CONTROL[tms]	= RB_UPDATING;		break;
			}
			break;
			//
	}
	return ROBOT_CONTROL[tms];
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL ROBOT_ANIMATION_PROCESS_CHECK( int ch ) { // 2012.09.03
	if (
		( ( ch >= BM1 ) && ( ch < TM ) ) ||
		( ( ch >= PM1 ) && ( ch < AL ) ) ) {
		if ( EQUIP_STATUS_PROCESS( 0 , ch ) == SYS_RUNNING ) return FALSE;
	}
	return TRUE;
}

int ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( int mac , int tms , int mode , int ch , int slot ) {
	//
	// mode
	//  -2 : Process(Place)
	//  -1 : Process(Pick)
	//   0 : Pick
	//   1 : Pick(T)
	//   2 : Place
	//   3 : Place(T)
	//   4 : Pick(Disabled)
	//   5 : Place(Disabled)
	//   ? : Unknown
	//
	int Res;
	char Buffer[256];
	//
	switch( mode ) {
	case -2 :
		sprintf( Buffer , "When %s is Placing , Processing has been detected in %s" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM)  , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) );
		break;
	case -1 :
		sprintf( Buffer , "When %s is Picking , Processing has been detected in %s" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM)  , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) );
		break;
	case 0 :
		sprintf( Buffer , "When %s is Picking , %s has not been detected in %s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) , slot );
		break;
	case 1 :
		sprintf( Buffer , "When %s is Picking , %s has been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM)  , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , slot + 'A' );
		break;
	case 2 :
		sprintf( Buffer , "When %s is Placing , %s has been detected in %s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) , slot );
		break;
	case 3 :
		sprintf( Buffer , "When %s is Placing , %s has not been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM)  , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , slot + 'A' );
		break;
	case 4 : // 2014.08.28
		sprintf( Buffer , "When %s is Picking , Disabled Arm %c has been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , slot + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , slot + 'A' );
		break;
	case 5 : // 2014.08.28
		sprintf( Buffer , "When %s is Placing , Disabled Arm %c has been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , slot + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME(tms+TM) , slot + 'A' );
		break;
	default :
		sprintf( Buffer , "" );
		break;
	}
	//
	if ( !ALARM_MESSAGE_SET( mac + tms , Buffer ) ) {
		//
		ALARM_POST( mac + tms );
		//
		ALARM_CLEAR( mac + tms );
		//
		Sleep(250);
		//
		ALARM_MESSAGE_SET( mac + tms , Buffer );
		//
	}
	//
	Res = ALARM_MANAGE( mac + tms );
	//
	if ( Res == 0 ) return 0; // ALM_CLEAR
	if ( Res == 1 ) return 1; // ALM_RETRY
	if ( Res == 2 ) return 1; // ALM_WAIT
	if ( Res == 3 ) return 0; // ALM_IGNORE
	if ( Res == 5 ) return 0; // ALM_ENABLE
	//
	return -1;
	//
}

BOOL ROBOT_ANIMATION_TM_WAFER_CHECK( int tms , int mode , int STATION , int TMSTATION , int SLOT , int SLOT2 , int DEPTH , int DEPTH2 , int SEPERATE ) {
	int Res , mac , pac;

	if ( GET_RUN_LD_CONTROL(0) > 0 ) { // 2014.01.09
		mac = 30;
//		pac = 40; // 2016.12.09
		pac = 0; // 2016.12.09
	}
	else {
		mac = _i_SCH_PRM_GET_WAFER_ANIM_STATUS_CHECK() % 100000;	if ( mac <= 0 ) return TRUE;
		pac = _i_SCH_PRM_GET_WAFER_ANIM_STATUS_CHECK() / 100000;
	}
	//
	while( TRUE ) {
		//
//		if ( pac > 0 ) { // 2013.03.20
		if ( pac <= 0 ) { // 2013.03.20
			if ( !ROBOT_ANIMATION_PROCESS_CHECK( STATION ) ) { // 2012.09.03
				Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , ( mode == RB_MODE_PICK ) ? -1 : -2 , STATION , 0 );
				if ( Res ==  1 ) continue;
				if ( Res == -1 ) return FALSE;
			}
		}
		//
		switch ( mode ) {
		case RB_MODE_PICK			:
			//
			if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TMSTATION ) ) { // 2014.08.28
				Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 4 , tms , TMSTATION );
				if ( Res ==  1 ) continue;
				if ( Res == -1 ) return FALSE;
			}
			//
			if ( STATION < PM1 ) { // 2018.09.11
				if ( DEPTH > 0 ) { // 2017.07.14
//					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH ) != CWM_PRESENT ) { // 2018.10.31
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH ) < CWM_PRESENT ) { // 2018.10.31
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 0 , STATION , DEPTH );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				//
				if ( DEPTH2 > 0 ) { // 2017.07.14
//					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH2 ) != CWM_PRESENT ) { // 2018.10.31
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH2 ) < CWM_PRESENT ) { // 2018.10.31
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 0 , STATION , DEPTH2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
			}
			else {
				if ( DEPTH > 0 ) { // 2017.07.14
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH ) <= 0 ) {
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 0 , STATION , DEPTH );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				//
				if ( DEPTH2 > 0 ) { // 2017.07.14
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH2 ) <= 0 ) {
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 0 , STATION , DEPTH2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
			}
			//
			if ( WAFER_STATUS_IN_TM( tms , TMSTATION ) > 0 ) {
				Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 1 , tms , TMSTATION );
				if ( Res ==  1 ) continue;
				if ( Res == -1 ) return FALSE;
			}
			if ( WAFER_STATUS_IN_TM2( tms , TMSTATION ) > 0 ) { // 2017.07.14
				Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 1 , tms , TMSTATION );
				if ( Res ==  1 ) continue;
				if ( Res == -1 ) return FALSE;
			}
			break;

		case RB_MODE_PLACE			:
			//
			if ( !_i_SCH_ROBOT_GET_FINGER_HW_USABLE( tms , TMSTATION ) ) { // 2014.08.28
				Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 5 , tms , TMSTATION );
				if ( Res ==  1 ) continue;
				if ( Res == -1 ) return FALSE;
			}
			//
			if ( STATION < PM1 ) { // 2018.09.11
				if ( DEPTH > 0 ) { // 2017.07.14
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH ) != CWM_ABSENT ) {
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 2 , STATION , DEPTH );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				//
				if ( DEPTH2 > 0 ) { // 2017.07.14
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH2 ) != CWM_ABSENT ) {
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 2 , STATION , DEPTH2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
			}
			else {
				if ( DEPTH > 0 ) { // 2017.07.14
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH ) > 0 ) {
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 2 , STATION , DEPTH );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				//
				if ( DEPTH2 > 0 ) { // 2017.07.14
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , DEPTH2 ) > 0 ) {
						Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 2 , STATION , DEPTH2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
			}
			//
			if ( WAFER_STATUS_IN_TM( tms , TMSTATION ) <= 0 ) {
				if ( WAFER_STATUS_IN_TM2( tms , TMSTATION ) <= 0 ) { // 2017.07.14
					Res = ROBOT_ANIMATION_TM_WAFER_FAIL_CHECK( mac , tms , 3 , tms , TMSTATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
			}
			break;
		}
		//
		break;
		//
	}
	return TRUE;
}

//--------------------------------------------------------------------------
BOOL ROBOT_ANIMATION_TM_SET_FOR_READY_DATA( int tms , int MODE , int STATION , int TMSTATION , int SLOT , int SLOT2 , int DEPTH , int DEPTH2 , int SYNCH , int SEPERATE , BOOL MaintInfUse , BOOL animuse ) {
	int lp , ts , td;
	//
	ROBOT_STATION[tms]		= STATION;
//	ROBOT_TM_STATION[tms]	= TMSTATION;
	ROBOT_TM_STATION[tms]	= TMSTATION % 100; // 2013.01.11
	//
	ROBOT_MODE[tms]			= MODE;
	ROBOT_SLOT[0][tms]		= SLOT;
	ROBOT_SLOT[1][tms]		= SLOT2;
	ROBOT_DEPTH[0][tms]		= DEPTH;
	ROBOT_DEPTH[1][tms]		= DEPTH2;
	ROBOT_SEPERATE[tms]		= SEPERATE;
	ROBOT_MAINTINF[tms]		= MaintInfUse; // 2008.02.26
	ROBOT_ANIMUSE[tms]		= animuse; // 2011.07.27
	//
	ROBOT_TM_STATION_W[tms] = _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( TMSTATION ); // 2013.01.11
	//
	if ( SYNCH < 0 ) { // 2015.12.16
		ROBOT_SYNCH_CHECK[tms]	= 0;
	}
	else {
		ROBOT_SYNCH_CHECK[tms]	= SYNCH;
		if ( ( ROBOT_MODE[tms] != RB_MODE_PICK_UPDATE ) && ( ROBOT_MODE[tms] != RB_MODE_PLACE_UPDATE ) ) {
			if ( ROBOT_SYNCH_CHECK[tms] ) ROBOT_SET_SYNCH_STATUS( tms , ROBOT_SYNCH_READY );
		}
	}
	//
	if ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) { // 2013.05.04
		if ( ( STATION >= BM1 ) && ( STATION < TM ) && ( tms != _i_SCH_PRM_GET_MODULE_GROUP( STATION ) ) ) {
			switch ( ROBOT_MODE[tms] ) {
			case RB_MODE_PICK			:
			case RB_MODE_PICKPLACE 		:
			case RB_MODE_PLACE			:

				if ( DEPTH <= 0 ) {
					if ( ( DEPTH2 % 2 ) == 0 ) {
						ROBOT_DEPTH[0][tms]		= 0;
						ROBOT_DEPTH[1][tms]		= DEPTH2 - 1;
					}
					else {
						ROBOT_DEPTH[0][tms]		= 0;
						ROBOT_DEPTH[1][tms]		= DEPTH2 + 1;
					}
				}
				else if ( DEPTH2 <= 0 ) {
					if ( ( DEPTH % 2 ) == 0 ) {
						ROBOT_DEPTH[0][tms]		= DEPTH - 1;
						ROBOT_DEPTH[1][tms]		= 0;
					}
					else {
						ROBOT_DEPTH[0][tms]		= DEPTH + 1;
						ROBOT_DEPTH[1][tms]		= 0;
					}
				}
				else {
					ROBOT_DEPTH[0][tms]		= DEPTH2;
					ROBOT_DEPTH[1][tms]		= DEPTH;
				}
				break;
			}
		}
	}
	//
	ROBOT_SYNCH_IGNORING[tms] = 0; // 2016.02.18
	//
	//
	// 2015.12.15
	//
//_IO_CIM_PRINTF( "A.[tms=%d][animuse=%d][mode=%d][ST,SL,DP=%d,%d,%d,%d,%d]\n" , tms , animuse , ROBOT_MODE[tms] , ROBOT_STATION[tms] , ROBOT_SLOT[0][tms] , ROBOT_SLOT[1][tms] , ROBOT_DEPTH[0][tms] , ROBOT_DEPTH[1][tms] );
	//
	if ( !animuse ) {
		switch ( ROBOT_MODE[tms] ) {
		case RB_MODE_EXTPIC			:
		case RB_MODE_PICK			:
		case RB_MODE_PICKPLACE 		:
			//
			if ( ROBOT_STATION[tms] < PM1 ) {
				//
				for ( lp = 0 ; lp < 2 ; lp++ ) {
					//
					if ( ROBOT_SLOT[lp][tms] <= 0 ) continue;
					//
					switch( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_SLOT[lp][tms] ) ) {
					case CWM_UNKNOWN :
					case CWM_ABSENT :
						ROBOT_ANX1_WSTS[lp][tms] = 0;
						break;
					default :
						ROBOT_ANX1_WSTS[lp][tms] = ROBOT_SLOT[lp][tms];
					}
					//
				}
				//
			}
			else if ( ( ROBOT_STATION[tms] >= BM1 ) && ( ROBOT_STATION[tms] < TM ) ) {
				//
				for ( lp = 0 ; lp < 2 ; lp++ ) {
					//
					if ( ROBOT_DEPTH[lp][tms] <= 0 ) continue;
					//-----------------------------
					ROBOT_ANX1_WSTS[lp][tms] = _i_SCH_IO_GET_MODULE_STATUS( ROBOT_STATION[tms] , ROBOT_DEPTH[lp][tms] );
					//-----------------------------
					//
				}
			}
			else {
				for ( lp = 0 ; lp < 2 ; lp++ ) {
					//
					if ( ROBOT_DEPTH[lp][tms] <= 0 ) continue;
					//=========================================================================
					if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 2 ) {
						if ( lp == 0 ) {
							ts = ROBOT_STATION[tms];
							td = ROBOT_DEPTH[lp][tms];
						}
						else {
							if ( ( ( ROBOT_STATION[tms] - PM1 ) % 2 ) == 0 ) {
								ts = ROBOT_STATION[tms] + 1;
							}
							else {
								ts = ROBOT_STATION[tms];
							}
							td = ROBOT_DEPTH[lp][tms] - 1;
						}
					}
					else {
						ts = ROBOT_STATION[tms];
						td = ROBOT_DEPTH[lp][tms];
					}
					//
					//-----------------------------
					ROBOT_ANX1_WSTS[lp][tms] = _i_SCH_IO_GET_MODULE_STATUS( ts , td );
					//-----------------------------
//_IO_CIM_PRINTF( "B.[tms=%d][animuse=%d][lp=%d][ST,DP=%d,%d][%d]\n" , tms , animuse , lp , ts , td , ROBOT_ANX1_WSTS[lp][tms] );
					//
				}
			}
			//
			break;

		case RB_MODE_EXTPLC			:
		case RB_MODE_PLACE			:
			//
			if ( ROBOT_STATION[tms] < PM1 ) {
				for ( lp = 0 ; lp < 2 ; lp++ ) {
					//
					if ( ROBOT_SLOT[lp][tms] <= 0 ) continue;
					//
					//===========================================================
					if ( lp == 0 ) {
						ROBOT_ANX1_WSTS[lp][tms] = WAFER_STATUS_IN_TM( tms , ROBOT_TM_STATION[tms] );
					}
					else {
						ROBOT_ANX1_WSTS[lp][tms] = WAFER_STATUS_IN_TM2( tms , ROBOT_TM_STATION[tms] );
					}
					//===========================================================
				}
			}
			else {
				for ( lp = 0 ; lp < 2 ; lp++ ) {
					//
					if ( ROBOT_DEPTH[lp][tms] <= 0 ) continue;
					//
					//===========================================================
					if ( lp == 0 ) {
						ROBOT_ANX1_WSTS[lp][tms] = WAFER_STATUS_IN_TM( tms , ROBOT_TM_STATION[tms] );
					}
					else {
						ROBOT_ANX1_WSTS[lp][tms] = WAFER_STATUS_IN_TM2( tms , ROBOT_TM_STATION[tms] );
					}
					//===========================================================
				}
			}
			//
			break;
		}
		//
	}
	//
	switch ( ROBOT_MODE[tms] ) {
	case RB_MODE_HOME			:
		ROBOT_UPDATING[tms] = 0; // 2008.12.10
		ROBOT_CONTROL[tms]	= RB_HOMING;
		break;
	case RB_MODE_UP				:
		ROBOT_CONTROL[tms]	= RB_HOMING;
		break;
	case RB_MODE_DOWN			:
		ROBOT_CONTROL[tms]	= RB_HOMING;
		break;
	case RB_MODE_PICK			:
	case RB_MODE_PICKPLACE 		: // 2013.01.11
		//
		if ( SYNCH >= 0 ) { // 2015.12.15
			//
			// 2017.07.13
			//
			lp = 0;
			//
			if ( ( STATION >= BM1 ) && ( STATION < TM ) ) {
				//
				if ( ( _i_SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tms ) > 0 ) ) {
					//
					if ( ( _i_SCH_PRM_GET_INTERLOCK_TM_BM_OTHERGROUP_SWAP() >= 1 ) && ( tms != _i_SCH_PRM_GET_MODULE_GROUP( STATION ) ) ) { // 2013.05.06
						lp = 1;
					}
					//
				}
			}
			//
			if ( lp == 0 ) { 
				if ( !ROBOT_ANIMATION_TM_WAFER_CHECK( tms , RB_MODE_PICK , STATION , TMSTATION , SLOT , SLOT2 , DEPTH , DEPTH2 , SEPERATE ) ) return FALSE;
			}
			else { // 2017.07.13
				if ( DEPTH > 0 ) {
					if ( ( DEPTH % 2 ) == 0 ) { // 2,4
						ts = DEPTH - 1;
					}
					else { // 1,3
						ts = DEPTH + 1;
					}
				}
				else {
					ts = 0;
				}
				if ( DEPTH2 > 0 ) {
					if ( ( DEPTH2 % 2 ) == 0 ) { // 2,4
						td = DEPTH2 - 1;
					}
					else { // 1,3
						td = DEPTH2 + 1;
					}
				}
				else {
					td = 0;
				}
				//
				if ( !ROBOT_ANIMATION_TM_WAFER_CHECK( tms , RB_MODE_PICK , STATION , TMSTATION , SLOT , SLOT2 , ts , td , SEPERATE ) ) return FALSE;
			}
		}
		//
		ROBOT_UPDATING[tms] = 0; // 2008.12.10
		ROBOT_CONTROL[tms]	= RB_STARTING;
		break;
	case RB_MODE_PLACE			:
		//
		if ( SYNCH >= 0 ) { // 2015.12.15
			if ( !ROBOT_ANIMATION_TM_WAFER_CHECK( tms , RB_MODE_PLACE , STATION , TMSTATION , SLOT , SLOT2 , DEPTH , DEPTH2 , SEPERATE ) ) return FALSE;
		}
		//
		ROBOT_UPDATING[tms] = 0; // 2008.12.10
		ROBOT_CONTROL[tms]	= RB_STARTING;
		break;
	case RB_MODE_RETPIC			:
		ROBOT_CONTROL[tms]	= RB_RETRACTING2;
		break;
	case RB_MODE_RETPLC			:
		ROBOT_CONTROL[tms]	= RB_RETRACTING2;
		break;
	case RB_MODE_RETRACT		:
		ROBOT_CONTROL[tms]	= RB_RETRACTING2;
		break;
	case RB_MODE_EXTPIC			:	
		ROBOT_UPDATING[tms] = 0; // 2008.12.10
		ROBOT_CONTROL[tms]	= RB_STARTING;
		break;
	case RB_MODE_EXTPLC			:
		ROBOT_UPDATING[tms] = 0; // 2008.12.10
		ROBOT_CONTROL[tms]	= RB_STARTING;
		break;
	case RB_MODE_EXTEND			:
		ROBOT_CONTROL[tms]	= RB_STARTING;
		break;
	case RB_MODE_ROTATE			:
		ROBOT_CONTROL[tms]	= RB_STARTING;
		break;
	case RB_MODE_PICK_UPDATE	:
		ROBOT_CONTROL[tms]	= RB_UPDATING;
		break;
	case RB_MODE_PLACE_UPDATE	:
		ROBOT_CONTROL[tms]	= RB_UPDATING;
		break;
	}
	return TRUE;
}



/*
// 2013.01.11
void ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( int tms , int mode , int Chamber , int Finger ) { // 2008.12.10
	BOOL update;
	//
	if ( ROBOT_UPDATING[tms] != 0 ) {
		ROBOT_CONTROL[tms] = RB_RESETTING1;
		ROBOT_MODE[tms] = mode;
		ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( tms , -99 , FALSE , &update );
		if ( update ) _i_SCH_IO_MTL_SAVE();
	}
	//
	ROBOT_CONTROL[tms] = RB_RETRACTING;
	ROBOT_MODE[tms] = mode;
	ROBOT_STATION[tms] = Chamber;
	ROBOT_TM_STATION[tms] = Finger;
	ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( tms , -99 , TRUE , &update );
	//
}
*/

void ROBOT_ANIMATION_TM_RUN_FOR_RESETTING( int tms , int mode , int Chamber , int Finger ) { // 2013.01.11
	BOOL update;
	//
	if ( ROBOT_UPDATING[tms] != 0 ) {
		ROBOT_CONTROL[tms] = RB_RESETTING1;
		ROBOT_MODE[tms] = mode;
		ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( tms , -99 , FALSE , &update );
		if ( update ) _i_SCH_IO_MTL_SAVE();
	}
	//
	ROBOT_CONTROL[tms] = RB_RETRACTING;
	ROBOT_MODE[tms] = mode;
	ROBOT_STATION[tms] = Chamber;
	ROBOT_TM_STATION[tms] = Finger % 100; 
	ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( tms , -99 , TRUE , &update );
	//
	if ( mode == RB_MODE_PICKPLACE ) {
		//
		ROBOT_CONTROL[tms] = RB_RETRACTING;
		ROBOT_MODE[tms] = mode;
		ROBOT_STATION[tms] = Chamber;
		ROBOT_TM_STATION[tms] = _i_SCH_ROBOT_GET_SWAP_PLACE_ARM( Finger );
		ROBOT_ANIMATION_TM_RUN_FOR_NORMAL( tms , -99 , TRUE , &update );
		//
	}
	//
}
