#include "default.h"

#include "EQ_Enum.h"

#include "System_Tag.h"
#include "User_Parameter.h"
#include "Robot_Animation.h"
#include "Robot_Handling.h"
#include "FA_Handling.h"
#include "sch_CassetteResult.h"
#include "IO_Part_data.h"
#include "IO_Part_Log.h"
#include "sch_estimate.h"

#include "INF_sch_CommonUser.h"

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
CONTROL_ROBOT_OBJECT	ROBOT_FM_CONTROL[MAX_FM_CHAMBER_DEPTH];
MODE_ROBOT_OBJECT		ROBOT_FM_MODE[MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_STATION[2][MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_SLOT[2][MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_REAL_IC_SLOT[2][MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_RUN_ID[MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_BMMDLEXT[MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_MAINTINF[MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_ANIMUSE[MAX_FM_CHAMBER_DEPTH];
int						ROBOT_FM_ANX1_WSTS[2][MAX_FM_CHAMBER_DEPTH]; // 2015.12.15
//
int						ROBOT_FM_SYNCH_CHECK[MAX_FM_CHAMBER_DEPTH];
//
int						ROBOT_FM_RUN_ID_BUF[MAX_FM_CHAMBER_DEPTH]; // 2014.11.04
//
int						ROBOT_FM_UPDATING[MAX_FM_CHAMBER_DEPTH]; // 2018.06.14

//
//-------------------------------------------
//----- New Append 2016.02.18 Begin
//-------------------------------------------
//
int						ROBOT_FM_SYNCH_IGNORING[MAX_FM_CHAMBER_DEPTH]; // 2016.02.18
//
//-------------------------------------------
//----- New Append 2016.02.18 End
//-------------------------------------------
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int ROBOT_ANIM_FM_ABSENT_ANIM() { // 2010.03.08
	if ( _i_SCH_PRM_GET_DFIX_CASSETTE_ABSENT_RUN_DATA() > 0 ) {
		return _i_SCH_PRM_GET_DFIX_CASSETTE_ABSENT_RUN_DATA();
	}
	else {
		return CWM_ABSENT;
	}
}
//-----------------------------------------------------------------------
void ROBOT_ANIM_FM_RUN_ONEBODY_Anim_Slot( int ch , int slot , int *rch , int *rslot ) { // 2005.09.30
	if ( ( ch >= BM1 ) && ( ch < TM ) ) {
		if ( slot > _i_SCH_PRM_GET_MODULE_SIZE( ch ) ) {
			if ( ( ( ch - BM1 ) % 2 ) == 0 )
				*rch = ch + 1;
			else
				*rch = ch - 1;
			//
			*rslot = slot - _i_SCH_PRM_GET_MODULE_SIZE( ch );
			return;
		}
	}
	*rch = ch;
	*rslot = slot;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
BOOL ROBOT_ANIM_FM_RUN_SAME( int fms ) { // 2007.05.31
	if ( ROBOT_FM_SYNCH_CHECK[fms] <= 2 ) return FALSE;
	if ( ROBOT_FM_SLOT[0][fms] <= 0 ) return FALSE;
	if ( ROBOT_FM_SLOT[1][fms] <= 0 ) return FALSE;
	return TRUE;
}

BOOL ROBOT_ANIM_FM_APLUSB_SWAP( int fms ) { // 2007.07.09
	if ( !_SCH_COMMON_FM_ARM_APLUSB_SWAPPING() ) return FALSE;
	if ( ROBOT_FM_SLOT[0][fms] <= 0 ) return FALSE;
	if ( ROBOT_FM_SLOT[1][fms] <= 0 ) return FALSE;
	return TRUE;
}

int ROBOT_ANIMATION_FM_RUN_UPDOWNING_HALF( int fms , int id , int runtype , BOOL End , int mode , int mode2 ) { // 2014.11.04
	double Result;
	double data;
	if ( runtype == 1 ) {
		//
		data = ( _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[id][fms] , ROBOT_FM_SLOT[id][fms] , mode ) + _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[id][fms] , ROBOT_FM_SLOT[id][fms] , mode2 ) ) / 2;
		//
		if ( End ) {
			//
			ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , data );
			//
			return 0;
			//
		}
		else {
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_UPDOWN ) ,
											data ,
											_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_UP ) , &Result ) ) {
				//
				Result = data;
				//
				ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
}

int ROBOT_ANIMATION_FM_RUN_UPDOWNING( int fms , int id , int runtype , BOOL End , int mode ) { // 2014.11.04
	double Result;
	if ( runtype == 1 ) {
		if ( End ) {
			//
			ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[id][fms] , ROBOT_FM_SLOT[id][fms] , mode ) );
			//
			return 0;
			//
		}
		else {
			//
			if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_UPDOWN ) ,
											_i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[id][fms] , ROBOT_FM_SLOT[id][fms] , mode ) ,
											_i_SCH_PRM_GET_RB_FM_RNG( fms , mode ) , &Result ) ) {
				//
				Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[id][fms] , ROBOT_FM_SLOT[id][fms] , mode );
				//
				ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , Result );
				//
				return 0;
			}
			else {
				//
				ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , Result );
				//
				return -1;
			}
		}
	}
	else {
		return 0;
	}
}

//-----------------------------------------------------------------------
//--------------------------------------------------------------------------
int ROBOT_ANIMATION_FM_RUN_FOR_NORMAL_SUB( int fms , int runtype , BOOL End , BOOL *update ) {
	BOOL a , b;
	int Rb_Rotate , ss , wsrc , wsts , l , oldid , offset_3p1;
	int wsre;
	double Result , Result2 , Buffer2 , Buffer3 , BMode;
	double Target0 , Target1;
	int Res;

	*update = FALSE;
	switch( ROBOT_FM_CONTROL[fms] ) {
		case RB_HOMING :
			ROBOT_FM_RUN_ID[fms] = 0;
			//
			if ( runtype == 1 ) {
				if      ( ROBOT_FM_MODE[fms] == RB_MODE_UP ) { // 2011.04.11
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , 100 );
				}
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_DOWN ) { // 2011.04.11
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , 0 );
				}
				else if ( ROBOT_FM_MODE[fms] == ( RB_MODE_UP + 100 ) ) { // 2011.12.08
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , ROBOT_FM_SLOT[0][fms] );
				}
				else if ( ROBOT_FM_MODE[fms] == ( RB_MODE_DOWN + 100 ) ) { // 2011.12.08
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , ROBOT_FM_SLOT[0][fms] );
				}
				else {
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , 0 );
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , 0 );
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_ROTATE , _i_SCH_PRM_GET_RB_FM_HOME_POSITION( fms ) );
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , 0 );
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_MOVE , 0 );
				}
			}
			//
			ROBOT_FM_CONTROL[fms]	= RB_HOMEDONE;
			break;

		case RB_STARTING :
		case RB_STARTING2 :
			//
			if ( ROBOT_FM_MODE[fms] >= 100 ) { // 2011.12.08
				//
				if ( ROBOT_FM_SLOT[0][fms]	== 0 )	ROBOT_FM_RUN_ID[fms]		= 0;
				else								ROBOT_FM_RUN_ID[fms]		= 1;
				//
				if ( ROBOT_FM_MODE[fms] == ( RB_MODE_EXTEND + 100 ) ) { // 2011.12.08
					ROBOT_FM_CONTROL[fms]	= RB_EXTENDING;
					break;
				}
				else if ( ROBOT_FM_MODE[fms] == ( RB_MODE_RETRACT + 100 ) ) { // 2011.12.08
					ROBOT_FM_CONTROL[fms]	= RB_RETRACTING2;
					break;
				}
				else {
					ROBOT_FM_CONTROL[fms]	= RB_RETRACTING;
				}
			}
			else {
				if ( ROBOT_FM_CONTROL[fms] == RB_STARTING ) {
					if ( ROBOT_FM_MODE[fms] == RB_MODE_RETRACT ) { // 2011.12.08
						if ( ROBOT_FM_SLOT[0][fms]	<= 0 )	ROBOT_FM_RUN_ID[fms]		= 2;
						else								ROBOT_FM_RUN_ID[fms]		= 1;
					}
					else {
						if ( ROBOT_FM_SLOT[0][fms]	<= 0 )	ROBOT_FM_RUN_ID[fms]		= 2;
						else								ROBOT_FM_RUN_ID[fms]		= 0;
					}
				}
				else {
					if ( ROBOT_FM_RUN_ID[fms] < 2 )	ROBOT_FM_RUN_ID[fms]		= 0;
					else							ROBOT_FM_RUN_ID[fms]		= 2;
				}
				//
				if ( ROBOT_FM_MODE[fms] == RB_MODE_EXTEND ) { // 2011.12.08
					ROBOT_FM_CONTROL[fms]	= RB_EXTENDING;
					break;
				}
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_RETRACT ) { // 2011.12.08
					ROBOT_FM_CONTROL[fms]	= RB_RETRACTING2;
					break;
				}
				else {
					ROBOT_FM_CONTROL[fms]	= RB_RETRACTING;
				}
			}

		case RB_RETRACTING :
			//
//			if ( runtype == 1 ) { // 2006.11.09 // 2018.06.14
			if ( ( runtype == 1 ) || ( runtype == -99 ) ) { // 2006.11.09 // 2018.06.14
				//
				if ( ROBOT_FM_MODE[fms] >= 100 ) { // 2011.12.08
					Target0 = 0;
					Target1 = 0;
				}
				else {
					Target0 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_RETRACT );
					Target1 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_RETRACT );
				}
				//
				if ( End ) {
					if ( ( ROBOT_FM_RUN_ID[fms] == 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Target0 );
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Target0 );
					}
					if ( ( ROBOT_FM_RUN_ID[fms] != 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Target1 );
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Target1 );
					}
					a = b = TRUE;
				}
				else {
					oldid = FALSE; // 2007.05.31
					if ( ( ROBOT_FM_RUN_ID[fms] == 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						a = ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND ) ,
														Target0 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_RETRACT ) , &Result );
						if ( a ) Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_RETRACT );
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Result );
						b = ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND2 ) ,
														Target0 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_RETRACT ) , &Result );
						if ( b ) Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_RETRACT );
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Result );
						//=====================================================
						if ( !a || !b ) oldid = TRUE; // 2007.05.31
						//=====================================================
					}
					// else { // 2007.05.31
					if ( ( ROBOT_FM_RUN_ID[fms] != 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						a = ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND ) ,
														Target1 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_RETRACT ) , &Result );
						if ( a ) Result = Target1;
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Result );
						b = ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND2 ) ,
														Target1 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_RETRACT ) , &Result );
						if ( b ) Result = Target1;
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Result );
					}
					//=====================================================
					if ( oldid ) { // 2007.05.31
						a = FALSE;
						b = FALSE;
					}
					//=====================================================
				}
			}
			else { // 2006.11.09
				a = b = TRUE; // 2006.11.09
			}
			if ( a && b ) ROBOT_FM_CONTROL[fms] = RB_ROTATING;
			break;

		case RB_ROTATING :
			if ( runtype == 1 ) { // 2006.11.09
				//
				if ( ROBOT_FM_MODE[fms] >= 100 ) { // 2011.12.08
					Target0 = ROBOT_FM_SLOT[1][fms] % 1000;
					Target1 = ROBOT_FM_SLOT[1][fms] / 1000;
				}
				else {
					if ( ROBOT_FM_RUN_ID[fms] == 0 ) {
						Target0 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_ROTATE );
						Target1 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_MOVE );
					}
					else {
						Target0 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_ROTATE );
						Target1 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_MOVE );
					}
				}
				//
				if ( End ) {
					//
					//-------------------------------------------
					//----- New Append 2016.02.18 Begin
					//-------------------------------------------
					//
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] == 0 ) {
							//
							if ( ROBOT_FM_GET_SYNCH_STATUS( fms ) == ROBOT_SYNCH_IGNORE ) {
								ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							}
							else {
								ROBOT_FM_SYNCH_IGNORING[fms] = 2;
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
					a = TRUE;
					b = TRUE;
					//
					if ( ROBOT_FM_SYNCH_IGNORING[fms] != 1 ) { // 2016.02.18
						//
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_ROTATE , Target0 );
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_MOVE , Target1 );
						//
					}
					//
				}
				else {
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							//
							break;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						if ( ss < ROBOT_SYNCH_ROTATE_START ) break;
						//
						//-------------------------------------------
						//----- New Append Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
							break;
						}
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING2;
							break;
						}
						//-------------------------------------------
						//----- New Append End
						//-------------------------------------------
					}
					a = FALSE;
					b = FALSE;
					//
					//--------------------------
					Result = ROBOT_FM_GET_POSITION( fms , RB_ANIM_ROTATE );
					a = ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( Result ,
							Target0 ,
							_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE ) );
					Result2 = ROBOT_FM_GET_POSITION( fms , RB_ANIM_MOVE );
					b = ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( Result2 ,
							Target1 ,
							_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_MOVE ) );
					if ( !a ) {
						Rb_Rotate = _i_SCH_PRM_GET_RB_FM_ROTATION_STYLE( fms );
						if ( Rb_Rotate == ROTATION_STYLE_HOME_CW ) {
							Buffer2 = _i_SCH_PRM_GET_RB_FM_HOME_POSITION( fms ) - Result;
							if ( Buffer2 < 0 ) Buffer2 += 360;
							Buffer3 = _i_SCH_PRM_GET_RB_FM_HOME_POSITION( fms ) - Target0;
							if ( Buffer3 < 0 ) Buffer3 += 360;
							if ( Buffer3 > Buffer2 ) Rb_Rotate = ROTATION_STYLE_CW;
							else                     Rb_Rotate = ROTATION_STYLE_CCW;
						}
						else if ( Rb_Rotate == ROTATION_STYLE_HOME_CCW ) {
							Buffer2 = Result - _i_SCH_PRM_GET_RB_FM_HOME_POSITION( fms );
							if ( Buffer2 < 0 ) Buffer2 += 360;
							Buffer3 = Target0 - _i_SCH_PRM_GET_RB_FM_HOME_POSITION( fms );
							if ( Buffer3 < 0 ) Buffer3 += 360;
							if ( Buffer3 > Buffer2 ) Rb_Rotate = ROTATION_STYLE_CCW;
							else                     Rb_Rotate = ROTATION_STYLE_CW;
						}
						if ( Rb_Rotate == ROTATION_STYLE_MINIMUM ) {
							if ( Target0 > Result )
								Buffer2 = Target0 - Result;
							else 
								Buffer2 = Result - Target0;
							if ( Buffer2 >= 180 ) {
								if ( Target0 > Result )
									BMode = -_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE );
								else
									BMode =  _i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE );
							}
							else {
								if ( Target0 > Result )
									BMode =  _i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE );
								else 
									BMode = -_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE );
							}
							Result = Result + BMode;
						}
						else if ( Rb_Rotate == ROTATION_STYLE_CCW ) {
							Result = Result + _i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE );
						}
						else {
							Result = Result - _i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE );
						}
						if      ( Result >= 360 ) Result = Result - 360;
						else if ( Result <    0 ) Result = Result + 360;
						a = ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( Result ,
								Target0 ,
								_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_ROTATE ) );
						if ( a ) Result  = Target0;
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_ROTATE , Result );
					}
					else {
						Result = Target0;
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_ROTATE , Result );
					}
					//--------------------------
					if ( !b ) {
						b = ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	Result2 ,
														Target1 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_MOVE ) , &Result2 );
						if ( b ) Result2 = Target1;
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_MOVE , Result2 );
					}
					else {
						Result2 = Target1;
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_MOVE , Result2 );
					}
					//--------------------------
				}
			}
			else { // 2006.11.09
				a = b = TRUE; // 2006.11.09
			}
			if ( a && b ) {
				if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK   ) ROBOT_FM_CONTROL[fms] = RB_DOWNING;
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE  ) ROBOT_FM_CONTROL[fms] = RB_UPPING;
//				else if ( ROBOT_FM_MODE[fms] == RB_MODE_EXTEND ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING; // 2011.04.11 // 2011.12.08
				else											 ROBOT_FM_CONTROL[fms] = RB_LASTING;
				if ( ROBOT_FM_RUN_ID[fms] == 0 ) {
					if ( ROBOT_FM_SLOT[0][fms] <= 0 ) ROBOT_FM_RUN_ID[fms] = 1;
				}
			}
			break;

		case RB_EXTENDING :
			if ( runtype == 1 ) { // 2006.11.09
				//
				if ( ROBOT_FM_MODE[fms] >= 100 ) { // 2011.12.08
					Target0 = ROBOT_FM_SLOT[1][fms];
					Target1 = ROBOT_FM_SLOT[1][fms];
				}
				else {
					Target0 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_EXTEND );
					Target1 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_EXTEND );
				}
				//
				if ( End ) {
					//
					//-------------------------------------------
					//----- New Append 2016.02.18 Begin
					//-------------------------------------------
					//
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] == 0 ) {
							//
							if ( ROBOT_FM_GET_SYNCH_STATUS( fms ) == ROBOT_SYNCH_IGNORE ) {
								ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							}
							else {
								ROBOT_FM_SYNCH_IGNORING[fms] = 2;
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
					if ( ( ROBOT_FM_RUN_ID[fms] == 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] != 1 ) { // 2016.02.18
							//
							ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Target0 );
							//
						}
						//
						if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPPING;
						else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_DOWNING;
						else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
					}
					if ( ( ROBOT_FM_RUN_ID[fms] != 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] != 1 ) { // 2016.02.18
							//
							ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Target1 );
							//
						}
						//
						if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPPING;
						else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_DOWNING;
						else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
					}
				}
				else {
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							//
							break;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						if ( ss < ROBOT_SYNCH_EXTEND_START ) break;
						//
						//-------------------------------------------
						//----- New Append Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
							break;
						}
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING2;
							break;
						}
						//-------------------------------------------
						//----- New Append End
						//-------------------------------------------
					}
					if ( ( ROBOT_FM_RUN_ID[fms] == 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND ) ,
														Target0 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_EXTEND ) , &Result ) ) {
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPPING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_DOWNING;
							else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
							Result = Target0;
						}
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Result );
					}
					if ( ( ROBOT_FM_RUN_ID[fms] != 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND2 ) ,
														Target1 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_EXTEND ) , &Result ) ) {
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPPING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_DOWNING;
							else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
							Result = Target1;
						}
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Result );
					}
				}
			}
			else { // 2006.11.09
				if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPPING; // 2006.11.09
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_DOWNING; // 2006.11.09
				else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
			}
			break;

/*
//
// 2014.11.04
//
		case RB_DOWNING :
			if ( runtype == 1 ) { // 2006.11.09
				if ( End ) {
					if ( ROBOT_FM_RUN_ID[fms] == 0 ) {
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_DOWN ) );
					}
					else {
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_DOWN ) );
					}
					if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
					else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
				}
				else {
					if ( ROBOT_FM_RUN_ID[fms] == 0 ) {
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_UPDOWN ) ,
														_i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_DOWN ) ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_DOWN ) , &Result ) ) {
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
							Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_DOWN );
						}
					}
					else {
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_UPDOWN ) ,
														_i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_DOWN ) ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_DOWN ) , &Result ) ) {
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
							Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_DOWN );
						}
					}
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , Result );
				}
			}
			else { // 2006.11.09
				if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING; // 2006.11.09
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_UPDATING; // 2006.11.09
			}
			break;

		case RB_UPPING :
			if ( runtype == 1 ) { // 2006.11.09
				if ( End ) {
					if ( ROBOT_FM_RUN_ID[fms] == 0 ) {
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_UP ) );
					}
					else {
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_UP ) );
					}
					if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
					else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
				}
				else {
					if ( ROBOT_FM_RUN_ID[fms] == 0 ) {
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_UPDOWN ) ,
														_i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_UP ) ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_UP ) , &Result ) ) {
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
							Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_UP );
						}
					}
					else {
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_UPDOWN ) ,
														_i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_UP ) ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_UP ) , &Result ) ) {
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
							Result = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_UP );
						}
					}
					ROBOT_FM_SET_POSITION( fms , RB_ANIM_UPDOWN , Result );
				}
			}
			else { // 2006.11.09
				if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPDATING; // 2006.11.09
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING; // 2006.11.09
			}
			break;
*/

//
// 2014.11.04
//
		case RB_DOWNING :
			//
			ROBOT_FM_RUN_ID_BUF[fms] = ROBOT_FM_RUN_ID[fms];
			//
			if ( ROBOT_FM_SYNCH_IGNORING[fms] ) { // 2016.02.19
				//
				Res = 0;
				//
			}
			else {
				//
				if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) {
					Res = ROBOT_ANIMATION_FM_RUN_UPDOWNING_HALF( fms , ( ROBOT_FM_RUN_ID[fms] == 0 ) ? 0 : 1 , runtype , End , RB_ANIM_DOWN , RB_ANIM_UP );
				}
				else {
					Res = ROBOT_ANIMATION_FM_RUN_UPDOWNING( fms , ( ROBOT_FM_RUN_ID[fms] == 0 ) ? 0 : 1 , runtype , End , RB_ANIM_DOWN );
				}
				//
			}
			//
			if ( Res == 0 ) {
				if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
			}
			//
			break;

		case RB_UPPING :
			//
			ROBOT_FM_RUN_ID_BUF[fms] = ROBOT_FM_RUN_ID[fms];
			//
			if ( ROBOT_FM_SYNCH_IGNORING[fms] ) { // 2016.02.19
				//
				Res = 0;
				//
			}
			else {
				//
				if ( ROBOT_FM_MODE[fms] == RB_MODE_PICK ) {
					Res = ROBOT_ANIMATION_FM_RUN_UPDOWNING_HALF( fms , ( ROBOT_FM_RUN_ID[fms] == 0 ) ? 0 : 1 , runtype , End , RB_ANIM_UP , RB_ANIM_DOWN );
				}
				else {
					Res = ROBOT_ANIMATION_FM_RUN_UPDOWNING( fms , ( ROBOT_FM_RUN_ID[fms] == 0 ) ? 0 : 1 , runtype , End , RB_ANIM_UP );
				}
				//
			}
			//
			if ( Res == 0 ) {
				if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_UPDATING;
				else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_EXTENDING;
			}
			//
			break;

		case RB_DOWNING2 : // 2014.11.04
			//
			if ( ROBOT_FM_SYNCH_IGNORING[fms] ) { // 2016.02.19
				//
				Res = 0;
				//
			}
			else {
				//
				Res = ROBOT_ANIMATION_FM_RUN_UPDOWNING( fms , ( ROBOT_FM_RUN_ID_BUF[fms] == 0 ) ? 0 : 1 , runtype , End , RB_ANIM_DOWN );
				//
			}
			//
			if ( Res == 0 ) {
				ROBOT_FM_CONTROL[fms] = RB_RETRACTING2;
			}
			//
			break;

		case RB_UPPING2 : // 2014.11.04
			//
			if ( ROBOT_FM_SYNCH_IGNORING[fms] ) { // 2016.02.19
				//
				Res = 0;
				//
			}
			else {
				//
				Res = ROBOT_ANIMATION_FM_RUN_UPDOWNING( fms , ( ROBOT_FM_RUN_ID_BUF[fms] == 0 ) ? 0 : 1 , runtype , End , RB_ANIM_UP );
				//
			}
			//
			if ( Res == 0 ) {
				ROBOT_FM_CONTROL[fms] = RB_RETRACTING2;
			}
			//
			break;

		case RB_UPDATING :
			if ( runtype == 1 ) { // 2006.11.09
				if ( !End ) {
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							//
							break;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						if ( ss < ROBOT_SYNCH_UPDATE ) break;
						//
						//-------------------------------------------
						//----- New Append Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
							break;
						}
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING2;
							break;
						}
						//-------------------------------------------
						//----- New Append End
						//-------------------------------------------
					}
					//
					ROBOT_FM_UPDATING[fms] = 1; // 2018.06.14
					//
				}
			}
			//==============================================================
			oldid = ROBOT_FM_RUN_ID[fms]; // 2007.06.04
			//==============================================================
			if ( ROBOT_FM_MODE[fms] == RB_MODE_PICK ) {
				if ( ( oldid == 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
					if ( ROBOT_FM_STATION[0][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[0][fms] , 1 );
						wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , 1 );
						//
						if ( runtype == 1 ) { // 2015.12.15
							switch( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] ) ) {
							case CWM_UNKNOWN :
							case CWM_ABSENT :
								wsts = 0;
								break;
							default :
								wsts = ROBOT_FM_SLOT[0][fms];
								break;
							}
						}
						else { // 2015.12.15
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//
						//===================================================================================
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 , TA_STATION );
						}
						//===================================================================================
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , ROBOT_ANIM_FM_ABSENT_ANIM() );
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_SLOT[0][fms] ); // 2007.03.14
							//-----------------------------
						}
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[0][fms] , 1 );
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									switch( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 ) ) {
									case CWM_UNKNOWN :
									case CWM_ABSENT :
										wsts = 0;
										break;
									default :
										wsts = ROBOT_FM_SLOT[0][fms] + l + 1;
										break;
									}
									//===================================================================================
									wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 );
									//===================================================================================
//									if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
									if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
									}
									//===================================================================================
									if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
									}
									//-----------------------------
									if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
									}
									//-----------------------------
									if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
										_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , ROBOT_ANIM_FM_ABSENT_ANIM() );
										//-----------------------------
										_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] + l + 1 ); // 2007.03.14
										//-----------------------------
									}
									//-----------------------------
								}
							}
						}
					}
//					else if ( ROBOT_FM_STATION[0][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == AL ) || ( ROBOT_FM_STATION[0][fms] == F_AL ) ) { // 2009.03.24
						//
						if ( runtype == 1 ) { // 2015.12.15
							//-----------------------------
							if ( ROBOT_FM_SLOT[0][fms] <= 1 ) { // 2008.08.29
								IO_WAFER_DATA_GET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							}
							else { // 2008.08.29
								IO_WAFER_DATA_GET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							}
							//-----------------------------
						}
						else { // 2015.12.15
							//-----------------------------
							if ( ROBOT_FM_SLOT[0][fms] <= 1 ) {
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_AL , 1 );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_AL , 1 );
							}
							else {
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( 1001 , 1 );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( 1001 , 1 );
							}
							//-----------------------------
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_AL , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 , TA_STATION );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION , 0 , wsrc + CM1 , wsts , -1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							if ( ROBOT_FM_SLOT[0][fms] <= 1 ) { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
							else { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_AL , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
					}
//					else if ( ROBOT_FM_STATION[0][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == IC ) || ( ROBOT_FM_STATION[0][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							//
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							//
							if ( runtype == 1 ) { // 2015.12.15
								wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							}
							else { // 2015.12.15
								wsts = ROBOT_FM_ANX1_WSTS[0][fms];
							}
							//
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , 0 );
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 0 , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
						else {
							//
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , 1 );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , 1 );
							//
							if ( runtype == 1 ) { // 2015.12.15
								wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							}
							else { // 2015.12.15
								wsts = ROBOT_FM_ANX1_WSTS[0][fms];
							}
							//
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 0 , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
						//-----------------------------
						//-----------------------------
						// 2006.12.18
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) {
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 );
									wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 );
									wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 );
									//-----------------------------
//									if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
									if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
									}
									//-----------------------------
									if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
									}
									//-----------------------------
									if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
									}
									//-----------------------------
									if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
										_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , 0 );
										//-----------------------------
										_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 0 , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsts ); // 2007.03.14
										//-----------------------------
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//
						if ( runtype == 1 ) { // 2015.12.15
							//-----------------------------
							IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
						}
						else { // 2015.12.15
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] );
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								if ( ROBOT_FM_BMMDLEXT[fms] ) { // 2006.12.15
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
//										if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
											SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											//-----------------------------
											IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
											//-----------------------------
											_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_STATION[0][fms] + l + 1 , wsts ); // 2007.03.14
											//-----------------------------
										}
									}
								}
								else {
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
//										if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
											SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											//-----------------------------
											IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , -1 , 0 , -1 , -1 ); // 2007.07.25
											//-----------------------------
											_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_STATION[0][fms] + l + 1 , wsts ); // 2007.03.14
											//-----------------------------
										}
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					ROBOT_FM_RUN_ID[fms] = 1;
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , FALSE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , FALSE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_SLOT[0][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
				// else { // 2007.05.31
				if ( ( oldid != 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
					if ( ROBOT_FM_STATION[1][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//
						wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[1][fms] , 1 );
						wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[1][fms] , 1 );
						//
						if ( runtype == 1 ) { // 2015.12.15
							switch( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] ) ) {
							case CWM_UNKNOWN :
							case CWM_ABSENT :
								wsts = 0;
								break;
							default :
								wsts = ROBOT_FM_SLOT[1][fms];
								break;
							}
						}
						else { // 2015.12.15
							wsts = ROBOT_FM_ANX1_WSTS[1][fms];
						}
						//
						//-----------------------------
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , ROBOT_ANIM_FM_ABSENT_ANIM() );
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[1][fms] , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_SLOT[1][fms] ); // 2007.03.14
							//-----------------------------
						}
					}
//					else if ( ROBOT_FM_STATION[1][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == AL ) || ( ROBOT_FM_STATION[1][fms] == F_AL ) ) { // 2009.03.24
						//
						if ( runtype == 1 ) { // 2015.12.15
							//-----------------------------
							if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( 1001 , 1 );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( 1001 , 1 );
								wsts = _i_SCH_IO_GET_MODULE_STATUS( 1001 , 1 );
							}
							else {
								if ( ROBOT_FM_SLOT[1][fms] <= 1 ) { // 2008.08.29
									IO_WAFER_DATA_GET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
								}
								else { // 2008.08.29
									IO_WAFER_DATA_GET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
								}
							}
						}
						else { // 2015.12.15
							if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( 1001 , 1 );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( 1001 , 1 );
							}
							else {
								if ( ROBOT_FM_SLOT[1][fms] <= 1 ) {
									wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_AL , 1 );
									wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_AL , 1 );
								}
								else {
									wsrc = _i_SCH_IO_GET_MODULE_SOURCE( 1001 , 1 );
									wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( 1001 , 1 );
								}
							}
							//-----------------------------
							wsts = ROBOT_FM_ANX1_WSTS[1][fms];
							//
						}
						//
						//-----------------------------
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_AL , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 , TB_STATION );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
								_i_SCH_IO_SET_MODULE_STATUS( 1001 , 1 , 0 );
							}
							else {
								if ( ROBOT_FM_SLOT[1][fms] <= 1 ) { // 2008.08.29
									IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
								}
								else { // 2008.08.29
									IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
								}
							}
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_AL , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
					}
//					else if ( ROBOT_FM_STATION[1][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == IC ) || ( ROBOT_FM_STATION[1][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							//
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							//
							if ( runtype == 1 ) { // 2015.12.15
								wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							}
							else { // 2015.12.15
								wsts = ROBOT_FM_ANX1_WSTS[1][fms];
							}
							//
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , 0 );
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 1 , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
						else {
							//
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , 1 );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , 1 );
							//
							if ( runtype == 1 ) { // 2015.12.15
								wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							}
							else { // 2015.12.15
								wsts = ROBOT_FM_ANX1_WSTS[1][fms];
							}
							//
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 1 , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
							//
							if ( runtype == 1 ) { // 2015.12.15
								//-----------------------------
								IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , &wsrc , &wsts , &wsre ); // 2007.08.10
								//-----------------------------
							}
							else { // 2015.12.15
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 );
								wsts = ROBOT_FM_ANX1_WSTS[1][fms];
							}
							//
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
						}
						else {
							//
							if ( runtype == 1 ) { // 2015.12.15
								//-----------------------------
								IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
								//-----------------------------
							}
							else { // 2015.12.15
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] );
								wsts = ROBOT_FM_ANX1_WSTS[1][fms];
							}
							//
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
						}
						//
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
								//-----------------------------
								IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , -1 , 0 , -1 , -1 ); // 2007.07.25
								//-----------------------------
							}
							else {
								//-----------------------------
								IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
								//-----------------------------
							}
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PICK2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[1][fms] , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
					}
					ROBOT_FM_RUN_ID[fms] = 3;
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , FALSE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , FALSE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_SLOT[1][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
			}
			else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) {
				if ( ( oldid == 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
					if ( ROBOT_FM_STATION[0][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
//
// 2015.12.15
//
						//
						wsrc = WAFER_SOURCE_IN_FM( fms , TA_STATION + offset_3p1 ); // 2008.02.26
						wsre = WAFER_SOURCE_IN_FM_E( fms , TA_STATION + offset_3p1 ); // 2008.02.26
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TA_STATION + offset_3p1 ); // 2007.03.14 // 2007.07.09
						}
						else { // 2015.12.15
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//
						//-----------------------------
//
//printf( "==============================\n" );
//printf( "A=[fms=%d][runtype=%d][offset_3p1=%d][wsrc=%d][wsts=%d][%d,%d]\n" , fms , runtype , offset_3p1 , wsrc , wsts , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] );
//printf( "==============================\n" );
//
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , -1 , TA_STATION + offset_3p1 );
						}
						//-----------------------------
						if ( wsts > 0 ) { // 2007.07.09
//							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09 // 2018.01.18(#2439)
//								_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , CWM_PRESENT ); // 2018.01.18
//							} // 2018.01.18
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2018.12.13
								//
								_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsre ); // 2018.12.13
								//
							}
							//
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2018.01.18(#2439)
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2008.02.26 // 2018.01.18
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , -1 );
							}
							//-----------------------------
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
//
//
//
/*
// 2015.12.15
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							wsrc = WAFER_SOURCE_IN_FM( fms , TA_STATION + offset_3p1 ); // 2008.02.26
							wsts = WAFER_STATUS_IN_FM( fms , TA_STATION + offset_3p1 ); // 2007.03.14 // 2007.07.09
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , -1 , TA_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( wsts > 0 ) { // 2007.07.09
								_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , CWM_PRESENT );
								//-----------------------------
								if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26
									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , -1 );
								}
								//-----------------------------
							}
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
*/
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
										//-----------------------------
//										if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
											SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , -1 , 2 + l - offset_3p1 );
										}
										//-----------------------------
										wsts = WAFER_STATUS_IN_FM( fms , 2 + l - offset_3p1 ); // 2007.03.14 // 2007.07.09
										//
										wsre = WAFER_SOURCE_IN_FM_E( fms , 2 + l - offset_3p1 ); // 2018.12.13
										//
										if ( wsts > 0 ) {
											//
//											_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , CWM_PRESENT ); // 2018.12.13
											//
											_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsre ); // 2018.12.13
											//
											//-----------------------------
//											if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2018.01.18
											if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2008.02.26 // 2018.01.18(#2439)
												SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , -1 );
												//
											}
											//-----------------------------
										}
										//-----------------------------
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
										//-----------------------------
										_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] + l + 1 , wsts ); // 2007.03.14
										//-----------------------------
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[0][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == AL ) || ( ROBOT_FM_STATION[0][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						wsrc = WAFER_SOURCE_IN_FM( fms , TA_STATION );
						wsre = WAFER_SOURCE_IN_FM_E( fms , TA_STATION );
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TA_STATION );
						}
						else {
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//-----------------------------
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION , 0 , wsrc + CM1 , wsts , -1 , TA_STATION );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							if ( ROBOT_FM_SLOT[0][fms] <= 1 ) { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							else { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							//-----------------------------
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_AL , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_AL , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
					}
//					else if ( ROBOT_FM_STATION[0][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == IC ) || ( ROBOT_FM_STATION[0][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//
						wsrc = WAFER_SOURCE_IN_FM( fms , TA_STATION + offset_3p1 );
						wsre = WAFER_SOURCE_IN_FM_E( fms , TA_STATION + offset_3p1 );
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TA_STATION + offset_3p1 );
						}
						else {
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//-----------------------------
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								_i_SCH_IO_SET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc );
								_i_SCH_IO_SET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsre );
								_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsts );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 0 , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
						else {
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
								//-----------------------------
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , 1 , wsrc + CM1 , wsts , -1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 0 , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
						//-----------------------------
						//-----------------------------
						// 2006.12.18
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) {
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
										//-----------------------------
										IO_WAFER_DATA_GET_TO_FM( fms , 2 + l - offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
//										if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
											SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
											_i_SCH_IO_SET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsrc );
											_i_SCH_IO_SET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsre );
											_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsts );
										}
										if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) {
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 );
										}
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) {
											//-----------------------------
											IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
											//-----------------------------
											_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 0 , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsts ); // 2007.03.14
											//-----------------------------
										}
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//===================================================================================
						//
						wsrc = WAFER_SOURCE_IN_FM( fms , TA_STATION + offset_3p1 );
						wsre = WAFER_SOURCE_IN_FM_E( fms , TA_STATION + offset_3p1 );
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TA_STATION + offset_3p1 );
						}
						else {
							wsts = ROBOT_FM_ANX1_WSTS[0][fms];
						}
						//-----------------------------
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TA_STATION + offset_3p1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								if ( ROBOT_FM_BMMDLEXT[fms] ) { // 2006.12.15
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_FM( fms , 2 + l - offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
//										if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
											SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											//-----------------------------
											IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
											//-----------------------------
											_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] , wsts ); // 2007.03.14
											//-----------------------------
										}
									}
								}
								else {
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_FM( fms , 2 + l - offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
//										if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
										if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
											SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , 2 + l - offset_3p1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsrc , wsts , -1 , wsre );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
											SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 );
										}
										//-----------------------------
										if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
											//-----------------------------
											IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
											//-----------------------------
											_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[0][fms] , ( fms * 100 ) , 0 , ROBOT_FM_SLOT[0][fms] + l + 1 , wsts ); // 2007.03.14
											//-----------------------------
										}
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					ROBOT_FM_RUN_ID[fms] = 1;
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , FALSE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , FALSE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_SLOT[0][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
				// else { // 2007.05.31
				if ( ( oldid != 0 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
					if ( ROBOT_FM_STATION[1][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
//
// 2015.12.15
//
						//
						wsrc = WAFER_SOURCE_IN_FM( fms , TB_STATION + offset_3p1 ); // 2008.02.26
						wsre = WAFER_SOURCE_IN_FM_E( fms , TB_STATION + offset_3p1 ); // 2008.02.26
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TB_STATION + offset_3p1 ); // 2007.03.14 // 2007.07.09
						}
						else { // 2015.12.15
							wsts = ROBOT_FM_ANX1_WSTS[1][fms];
						}
						//
//printf( "==============================\n" );
//printf( "B=[fms=%d][runtype=%d][offset_3p1=%d][wsrc=%d][wsts=%d][%d,%d]\n" , fms , runtype , offset_3p1 , wsrc , wsts , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] );
//printf( "==============================\n" );
						//-----------------------------
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 , TB_STATION + offset_3p1 );
						}
						//-----------------------------
						if ( wsts > 0 ) {
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
//								_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , CWM_PRESENT ); // 2018.12.13
								//
								_i_SCH_IO_SET_MODULE_SOURCE_E( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsre ); // 2018.12.13
								//
							}
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2019.01.11
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2008.02.26 // 2019.01.11
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 );
							}
							//-----------------------------
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[1][fms] , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}

/*
//
// 2015.12.15
//
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							wsrc = WAFER_SOURCE_IN_FM( fms , TB_STATION + offset_3p1 ); // 2008.02.26
							wsts = WAFER_STATUS_IN_FM( fms , TB_STATION + offset_3p1 ); // 2007.03.15 // 2007.07.09
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( wsts > 0 ) {
								_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , CWM_PRESENT );
								//-----------------------------
								if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26
									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 );
								}
								//-----------------------------
							}
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[1][fms] , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
*/
//
					}
//					else if ( ROBOT_FM_STATION[1][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == AL ) || ( ROBOT_FM_STATION[1][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						wsrc = WAFER_SOURCE_IN_FM( fms , TB_STATION );
						wsre = WAFER_SOURCE_IN_FM_E( fms , TB_STATION );
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TB_STATION );
						}
						else {
							wsts = ROBOT_FM_ANX1_WSTS[1][fms];
						}
						//-----------------------------
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 , TB_STATION );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
								 _i_SCH_IO_SET_MODULE_SOURCE( 1001 , 1 , wsrc );
								 _i_SCH_IO_SET_MODULE_SOURCE_E( 1001 , 1 , wsre );
								 _i_SCH_IO_SET_MODULE_STATUS( 1001 , 1 , wsts );
							}
							else {
								//-----------------------------
								if ( ROBOT_FM_SLOT[1][fms] <= 1 ) { // 2008.08.29
									IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
								}
								else { // 2008.08.29
									IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
								}
								//-----------------------------
							}
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_AL , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_AL , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
					}
//					else if ( ROBOT_FM_STATION[1][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == IC ) || ( ROBOT_FM_STATION[1][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//
						wsrc = WAFER_SOURCE_IN_FM( fms , TB_STATION + offset_3p1 );
						wsre = WAFER_SOURCE_IN_FM_E( fms , TB_STATION + offset_3p1 );
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TB_STATION + offset_3p1 );
						}
						else {
							wsts = ROBOT_FM_ANX1_WSTS[1][fms];
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								_i_SCH_IO_SET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc );
								_i_SCH_IO_SET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsre );
								_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsts );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 1 , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
						else {
							//-----------------------------
//							if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
								SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
								//-----------------------------
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , 1 , wsrc + CM1 , wsts , -1 );
							}
							//-----------------------------
							if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
								//-----------------------------
								IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
								//-----------------------------
								_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , F_IC , ( fms * 100 ) , 1 , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsts ); // 2007.03.14
								//-----------------------------
							}
						}
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//
						wsrc = WAFER_SOURCE_IN_FM( fms , TB_STATION + offset_3p1 );
						wsre = WAFER_SOURCE_IN_FM_E( fms , TB_STATION + offset_3p1 );
						//
						if ( runtype == 1 ) { // 2015.12.15
							wsts = WAFER_STATUS_IN_FM( fms , TB_STATION + offset_3p1 );
						}
						else {
							wsts = ROBOT_FM_ANX1_WSTS[1][fms];
						}
						//===================================================================================
//						if ( ROBOT_FM_MAINTINF[fms] && ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
						if ( ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) ) { // 2008.02.26 // 2011.04.18
							SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 , TB_STATION + offset_3p1 );
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
								IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , wsrc , wsts , -1 , wsre );
							}
							else {
								IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre );
							}
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 3 ) || ( runtype == 4 ) ) { // 2006.11.09
							if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , wsrc + CM1 , wsts , -1 );
							}
							else {
								SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
							}
						}
						//-----------------------------
						if ( ( runtype == 1 ) || ( runtype == 2 ) || ( runtype == 4 ) ) { // 2006.11.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
							_i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( 0 , 0 , FA_SUBSTRATE_PLACE2 , FA_SUBST_SUCCESS , ROBOT_FM_STATION[1][fms] , ( fms * 100 ) , 1 , ROBOT_FM_SLOT[1][fms] , wsts ); // 2007.03.14
							//-----------------------------
						}
					}
					ROBOT_FM_RUN_ID[fms] = 3;
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , FALSE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , FALSE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_SLOT[1][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
			}
			//
//			ROBOT_FM_CONTROL[fms] = RB_RETRACTING2; // 2014.11.04
			//
			if ( ROBOT_FM_MODE[fms] == RB_MODE_PICK ) {
				ROBOT_FM_CONTROL[fms] = RB_UPPING2; // 2014.11.04
			}
			else {
				ROBOT_FM_CONTROL[fms] = RB_DOWNING2; // 2014.11.04
			}
			//---------------------------------------------------------------------
			//---------------------------------------------------------------------
			*update = TRUE;
			//---------------------------------------------------------------------
			//---------------------------------------------------------------------
			break;

		case RB_RETRACTING2 :
			//==============================================================
			oldid = ROBOT_FM_RUN_ID[fms]; // 2007.06.04
			//==============================================================
			if ( runtype == 1 ) { // 2006.11.09
				//
				if ( ROBOT_FM_MODE[fms] >= 100 ) { // 2011.12.08
					//
					oldid++;
					//
					Target0 = ROBOT_FM_SLOT[1][fms];
					Target1 = ROBOT_FM_SLOT[1][fms];
				}
				else {
					Target0 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , RB_ANIM_RETRACT );
					Target1 = _i_SCH_PRM_GET_RB_FM_POSITION( fms , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , RB_ANIM_RETRACT );
				}
				//
				if ( End ) {
					//
					//-------------------------------------------
					//----- New Append 2016.02.18 Begin
					//-------------------------------------------
					//
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] == 0 ) {
							//
							if ( ROBOT_FM_GET_SYNCH_STATUS( fms ) == ROBOT_SYNCH_IGNORE ) {
								ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							}
							else {
								ROBOT_FM_SYNCH_IGNORING[fms] = 2;
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
					if ( ( oldid == 1 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] != 1 ) { // 2016.02.18
							//
							ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Target0 );
							//
						}
						//
						if ( !ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.06.04
							if ( ROBOT_FM_SLOT[1][fms] <= 0 ) {
								if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
								else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
								else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
							}
							else {
								if ( ROBOT_FM_MODE[fms] != RB_MODE_RETRACT ) // 2011.04.11
									ROBOT_FM_CONTROL[fms] = RB_ROTATING;
								//
								ROBOT_FM_RUN_ID[fms] = 2;
							}
						}
					}
					// else { // 2007.05.31
					if ( ( oldid != 1 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						//
						if ( ROBOT_FM_SYNCH_IGNORING[fms] != 1 ) { // 2016.02.18
							//
							ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Target1 );
							//
						}
						//
						if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
						else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
						else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
					}
				}
				else {
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							//
							break;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						if ( ss < ROBOT_SYNCH_RETRACT_START ) break;
						//
						//-------------------------------------------
						//----- New Append Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
							break;
						}
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING2;
							break;
						}
						//-------------------------------------------
						//----- New Append End
						//-------------------------------------------
					}
//					if ( ROBOT_FM_RUN_ID[fms] == 1 ) { // 2007.05.31
					if ( ( oldid == 1 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND ) ,
														Target0 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_RETRACT ) , &Result ) ) {
							Result = Target0;
							//
							if ( !ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.06.04
								if ( ROBOT_FM_SLOT[1][fms] <= 0 ) {
									if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
									else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
									else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
								}
								else {
									if ( ROBOT_FM_MODE[fms] != RB_MODE_RETRACT ) { // 2011.04.11
										if ( ROBOT_FM_SYNCH_CHECK[fms] >= 2 ) {
											ROBOT_FM_CONTROL[fms] = RB_WAITING;
										}
										else {
											ROBOT_FM_CONTROL[fms] = RB_ROTATING;
											ROBOT_FM_RUN_ID[fms] = 2;
										}
									}
								}
							}
							//
						}
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND , Result );
					}
					// else { // 2007.05.31
					if ( ( oldid != 1 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31
						if ( ROBOT_ANIMATION_SUB_GO_TARGET_POSITION(	ROBOT_FM_GET_POSITION( fms , RB_ANIM_EXTEND2 ) ,
														Target1 ,
														_i_SCH_PRM_GET_RB_FM_RNG( fms , RB_ANIM_RETRACT ) , &Result ) ) {
							Result = Target1;
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_LASTING;
							else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
						}
						ROBOT_FM_SET_POSITION( fms , RB_ANIM_EXTEND2 , Result );
					}
				}
			}
			else { // 2006.11.09
				if ( ( oldid == 1 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2006.11.09 // 2007.05.31
					if ( !ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.06.04
						if ( ROBOT_FM_SLOT[1][fms] <= 0 ) { // 2006.11.09
							if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2006.11.09
							else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2006.11.09
							else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
						} // 2006.11.09
						else { // 2006.11.09
							if ( ROBOT_FM_MODE[fms] != RB_MODE_RETRACT ) // 2011.04.11
								ROBOT_FM_CONTROL[fms] = RB_ROTATING; // 2006.11.09
							//
							ROBOT_FM_RUN_ID[fms] = 2; // 2006.11.09
						} // 2006.11.09
					}
				} // 2006.11.09
				if ( ( oldid != 1 ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2006.11.09 // 2007.05.31
					if	    ( ROBOT_FM_MODE[fms] == RB_MODE_PICK  ) ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2006.11.09
					else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2006.11.09
					else											ROBOT_FM_CONTROL[fms] = RB_LASTING; // 2011.04.11
				} // 2006.11.09
			} // 2006.11.09
			break;

		//=======================================================================================
		case RB_WAITING :
			if ( runtype == 1 ) { // 2006.11.09
				if ( !End ) {
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							//
							break;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						//-------------------------------------------
						//----- New Append Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
							break;
						}
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING2;
							break;
						}
						//-------------------------------------------
						//----- New Append End
						//-------------------------------------------
						if ( ss != ROBOT_SYNCH_SUCCESS ) break;
					}
				}
			}
			ROBOT_FM_CONTROL[fms] = RB_ROTATING;
			ROBOT_FM_RUN_ID[fms] = 2;
			ROBOT_FM_SET_SYNCH_STATUS( fms , ROBOT_SYNCH_READY );
			break;
		//=======================================================================================
		//=======================================================================================
		case RB_LASTING :
			if ( runtype == 1 ) { // 2006.11.09
				if ( !End ) {
					if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) {
						//
						ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_IGNORE ) { // 2016.02.18
							//
							ROBOT_FM_SYNCH_IGNORING[fms] = 1;
							//
							break;
						}
						//
						//-------------------------------------------
						//----- New Append 2016.02.18 End
						//-------------------------------------------
						//
						//-------------------------------------------
						//----- New Append Begin
						//-------------------------------------------
						if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
							break;
						}
						if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) {
							ROBOT_FM_CONTROL[fms] = RB_RESETTING2;
							break;
						}
						//-------------------------------------------
						//----- New Append End
						//-------------------------------------------
						if ( ss != ROBOT_SYNCH_SUCCESS ) break;
					}
				}
			}
			switch ( ROBOT_FM_MODE[fms] ) {
			case RB_MODE_HOME	:	ROBOT_FM_CONTROL[fms]	= RB_HOMEDONE;		break;
			case RB_MODE_PICK	:	ROBOT_FM_CONTROL[fms]	= RB_PICKDONE;		break;
			case RB_MODE_PLACE	:	ROBOT_FM_CONTROL[fms]	= RB_PLACEDONE;		break;
			case RB_MODE_ROTATE	:	ROBOT_FM_CONTROL[fms]	= RB_ROTATEDONE;	break;
			case RB_MODE_ROTATE	+ 100 :	ROBOT_FM_CONTROL[fms]	= RB_ROTATEDONE;	break; // 2011.12.08
			//
			case RB_MODE_RETRACT:	ROBOT_FM_CONTROL[fms]	= RB_RETRACTDONE;	break; // 2011.04.11
			case RB_MODE_RETRACT + 100 :	ROBOT_FM_CONTROL[fms]	= RB_RETRACTDONE;	break; // 2011.04.11
			//
			case RB_MODE_EXTEND	:	ROBOT_FM_CONTROL[fms]	= RB_EXTENDDONE;	break; // 2011.04.11
			case RB_MODE_EXTEND	+ 100 :	ROBOT_FM_CONTROL[fms]	= RB_EXTENDDONE;	break; // 2011.12.08
			//
			}
			break;

		case RB_RESETTING1 :
			ROBOT_FM_CONTROL[fms] = RB_RESETTING1_END;
			if ( ROBOT_FM_MODE[fms] == RB_MODE_PICK ) {
//				if ( ( ( ROBOT_FM_RUN_ID[fms] >= 1 ) && ( ROBOT_FM_SLOT[0][fms] > 0 ) ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31 // 2008.10.02
				if ( ( ROBOT_FM_RUN_ID[fms] >= 1 ) && ( ROBOT_FM_SLOT[0][fms] > 0 ) ) { // 2007.05.31 // 2008.10.02
					if ( ROBOT_FM_STATION[0][fms] < PM1 ) {
						// Update 2001.10.16
						//_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION , ROBOT_FM_SLOT[0][fms] , CWM_PRESENT );
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( WAFER_STATUS_IN_FM( fms ,TA_STATION + offset_3p1 ) > 0 ) { // 2007.07.09
							_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , CWM_PRESENT );
						}
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									if ( WAFER_STATUS_IN_FM( fms , 2 + l - offset_3p1 ) > 0 ) { // 2007.07.09
										_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , CWM_PRESENT );
									}
									//-----------------------------
									IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
									//-----------------------------
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[0][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == AL ) || ( ROBOT_FM_STATION[0][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TA_STATION , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						if ( ROBOT_FM_SLOT[0][fms] <= 1 ) { // 2008.08.29
							IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						else { // 2008.08.29
							IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
						}
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_AL , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[0][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == IC ) || ( ROBOT_FM_STATION[0][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							//-----------------------------
							IO_WAFER_DATA_GET_TO_FM( fms , TA_STATION + offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
							_i_SCH_IO_SET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc );
							_i_SCH_IO_SET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsre );
							_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsts );
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
							//-----------------------------
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
						}
						else {
							//-----------------------------
							IO_WAFER_DATA_GET_TO_FM( fms , TA_STATION + offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
							IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , 1 , wsrc + CM1 , wsts , -1 );
							//-----------------------------
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
						}
						//-----------------------------
						// 2006.12.18
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) {
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									//-----------------------------
									IO_WAFER_DATA_GET_TO_FM( fms , 2 + l - offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
									//-----------------------------
									_i_SCH_IO_SET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsrc );
									_i_SCH_IO_SET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsre );
									_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsts );
									//-----------------------------
									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 );
									//-----------------------------
									//-----------------------------
									IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
									//-----------------------------
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TA_STATION + offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						if ( ROBOT_FM_STATION[0][fms] >= BM1 ) {
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre );
						}
						else {
							IO_WAFER_DATA_SET_TO_CHAMBER( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre );
						}
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								if ( ROBOT_FM_BMMDLEXT[fms] ) { // 2006.12.15
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_FM( fms , 2 + l - offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
										IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , wsrc , wsts , -1 , wsre );
										//-----------------------------
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , wsrc + CM1 , wsts , -1 );
										//-----------------------------
										//-----------------------------
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
										//-----------------------------
									}
								}
								else {
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_FM( fms , 2 + l - offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
										IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsrc , wsts , -1 , wsre );
										//-----------------------------
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , wsrc + CM1 , wsts , -1 );
										//-----------------------------
										//-----------------------------
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
										//-----------------------------
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , TRUE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , TRUE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_SLOT[0][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
//				if ( ( ( ROBOT_FM_RUN_ID[fms] >= 3 ) && ( ROBOT_FM_SLOT[1][fms] > 0 ) ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31 // 2008.10.02
				if ( ( ROBOT_FM_RUN_ID[fms] >= 3 ) && ( ROBOT_FM_SLOT[1][fms] > 0 ) ) { // 2007.05.31 // 2008.10.02
					if ( ROBOT_FM_STATION[1][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , CWM_PRESENT );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[1][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == AL ) || ( ROBOT_FM_STATION[1][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
							 _i_SCH_IO_SET_MODULE_SOURCE( 1001 , 1 , wsrc );
							 _i_SCH_IO_SET_MODULE_SOURCE_E( 1001 , 1 , wsre );
							 _i_SCH_IO_SET_MODULE_STATUS( 1001 , 1 , wsts );
						}
						else {
							//-----------------------------
							if ( ROBOT_FM_SLOT[1][fms] <= 0 ) { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							else { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
							}
							//-----------------------------
						}
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_AL , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[1][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == IC ) || ( ROBOT_FM_STATION[1][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							//-----------------------------
							IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION + offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
							_i_SCH_IO_SET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc );
							_i_SCH_IO_SET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsre );
							_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsts );
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
							//-----------------------------
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
						}
						else {
							//-----------------------------
							IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION + offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
							IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , 1 , wsrc + CM1 , wsts , -1 );
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
						}
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION + offset_3p1 , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , wsrc , wsts , -1 , wsre );
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , wsrc + CM1 , wsts , -1 );
							//-----------------------------
						}
						else {
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre );
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
							//-----------------------------
						}
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , TRUE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PICK , TRUE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_SLOT[1][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
			}
			else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) {
//				if ( ( ( ROBOT_FM_RUN_ID[fms] >= 1 ) && ( ROBOT_FM_SLOT[0][fms] > 0 ) ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31 // 2008.10.02
				if ( ( ROBOT_FM_RUN_ID[fms] >= 1 ) && ( ROBOT_FM_SLOT[0][fms] > 0 ) ) { // 2007.05.31 // 2008.10.02
					if ( ROBOT_FM_STATION[0][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[0][fms] , 1 );
						wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , 1 );
						wsts = ROBOT_FM_SLOT[0][fms];
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						//-----------------------------
						_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , CWM_ABSENT );
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								//
								wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[0][fms] , 1 );
								wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[0][fms] , 1 );
								//
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									wsts = ROBOT_FM_SLOT[0][fms] + l + 1;
									//-----------------------------
									IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
									//-----------------------------
									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
									//-----------------------------
									_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , CWM_ABSENT );
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[0][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == AL ) || ( ROBOT_FM_STATION[0][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						if ( ROBOT_FM_SLOT[0][fms] <= 1 ) { // 2008.08.29
							IO_WAFER_DATA_GET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
						}
						else { // 2008.08.29
							IO_WAFER_DATA_GET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
						}
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION , 0 , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						if ( ROBOT_FM_SLOT[0][fms] <= 1 ) { // 2008.08.29
							IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
						}
						else { // 2008.08.29
							IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
						}
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[0][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[0][fms] == IC ) || ( ROBOT_FM_STATION[0][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							//
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] );
							//
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							//-----------------------------
							_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , 0 );
						}
						else {
							//-----------------------------
							IO_WAFER_DATA_GET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
						}
						//-----------------------------
						//-----------------------------
						// 2006.12.18
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) {
								for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
									//===================================================================================
									// 2007.07.09
									//===================================================================================
									if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
										if ( l == 0 ) {
											offset_3p1 = 2;
										}
										else {
											offset_3p1 = 1;
										}
									}
									else {
										offset_3p1 = 0;
									}
									//===================================================================================
									wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 );
									wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 );
									wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 );
									//-----------------------------
									IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
									//-----------------------------
									SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
									//-----------------------------
									_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[0][fms] + l + 1 , 0 );
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = 1;
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						//-----------------------------
						IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TA_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TA_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						//-----------------------------
						IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
						//-----------------------------
						//-----------------------------
						// 2003.10.21
						//-----------------------------
						//-----------------------------
						if ( _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) > 0 ) {
							if ( _i_SCH_PRM_GET_RB_FM_FINGER_ARM_STYLE( fms , 0 ) ) { // 2006.02.07
								if ( ROBOT_FM_BMMDLEXT[fms] ) { // 2006.12.15
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
										//-----------------------------
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
										//-----------------------------
										IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] + l + 1 , ROBOT_FM_SLOT[0][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
										//-----------------------------
									}
								}
								else {
									for ( l = 0 ; l < _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) ; l++ ) {
										//===================================================================================
										// 2007.07.09
										//===================================================================================
										if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
											if ( l == 0 ) {
												offset_3p1 = 2;
											}
											else {
												offset_3p1 = 1;
											}
										}
										else {
											offset_3p1 = 0;
										}
										//===================================================================================
										//-----------------------------
										IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , &wsrc , &wsts , &wsre ); // 2007.08.10
										//-----------------------------
										IO_WAFER_DATA_SET_TO_FM( fms , 2 + l - offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
										//-----------------------------
										SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , 2 + l - offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
										//-----------------------------
										IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[0][fms] , ROBOT_FM_SLOT[0][fms] + l + 1 , -1 , 0 , -1 , -1 ); // 2007.07.25
										//-----------------------------
									}
								}
							}
						}
						//-----------------------------
						//-----------------------------
					}
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , TRUE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , TRUE , runtype , ROBOT_FM_STATION[0][fms] , TA_STATION , ROBOT_FM_SLOT[0][fms] , ROBOT_FM_SLOT[0][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
//				if ( ( ( ROBOT_FM_RUN_ID[fms] >= 3 ) && ( ROBOT_FM_SLOT[1][fms] > 0 ) ) || ROBOT_ANIM_FM_RUN_SAME(fms) ) { // 2007.05.31 // 2008.10.02
				if ( ( ROBOT_FM_RUN_ID[fms] >= 3 ) && ( ROBOT_FM_SLOT[1][fms] > 0 ) ) { // 2007.05.31 // 2008.10.02
					if ( ROBOT_FM_STATION[1][fms] < PM1 ) {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[1][fms] , 1 );
						wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[1][fms] , 1 );
						wsts = ROBOT_FM_SLOT[1][fms];
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						//-----------------------------
						_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , CWM_ABSENT );
					}
//					else if ( ROBOT_FM_STATION[1][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == AL ) || ( ROBOT_FM_STATION[1][fms] == F_AL ) ) { // 2009.03.24
						if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
							 wsrc = _i_SCH_IO_GET_MODULE_SOURCE( 1001 , 1 );
							 wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( 1001 , 1 );
							 wsts = _i_SCH_IO_GET_MODULE_STATUS( 1001 , 1 );
						}
						else {
							//-----------------------------
							if ( ROBOT_FM_SLOT[1][fms] <= 1 ) { // 2008.08.29
								IO_WAFER_DATA_GET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							}
							else { // 2008.08.29
								IO_WAFER_DATA_GET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							}
							//-----------------------------
						}
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) { // 2007.10.17
							 _i_SCH_IO_SET_MODULE_STATUS( 1001 , 1 , 0 );
						}
						else {
							if ( ROBOT_FM_SLOT[1][fms] <= 1 ) { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
							else { // 2008.08.29
								IO_WAFER_DATA_SET_TO_CHAMBER( 1001 , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
							}
						}
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[1][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == IC ) || ( ROBOT_FM_STATION[1][fms] == F_IC ) ) { // 2009.03.24
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( _i_SCH_PRM_GET_DFIX_FIC_MULTI_CONTROL() == 1 ) { // 2010.12.22
							wsrc = _i_SCH_IO_GET_MODULE_SOURCE( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							wsts = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] );
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							//-----------------------------
							_i_SCH_IO_SET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , 0 );
						}
						else {
							//-----------------------------
							IO_WAFER_DATA_GET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
							IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
							//-----------------------------
							SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
							//-----------------------------
							IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
							//-----------------------------
						}
					}
					else {
						//===================================================================================
						// 2007.07.09
						//===================================================================================
						if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) { // 2007.07.09
							offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
						}
						else {
							offset_3p1 = 0;
						}
						//===================================================================================
						if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
							//-----------------------------
							IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
						}
						else {
							//-----------------------------
							IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
							//-----------------------------
						}
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION + offset_3p1 , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION + offset_3p1 , 0 , wsrc + CM1 , wsts , -1 ); // 2007.07.09
						//-----------------------------
						if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[1][fms] > 2 ) ) { // 2007.01.06
							//-----------------------------
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[1][fms] / 4 , -1 , 0 , -1 , -1 ); // 2007.07.25
							//-----------------------------
						}
						else {
							//-----------------------------
							IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
							//-----------------------------
						}
					}
					//
//					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , TRUE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , 0 ); // 2010.06.10 // 2014.06.27
					if ( ROBOT_FM_ANIMUSE[fms] ) _SCH_COMMON_ANIMATION_UPDATE_TM( -1 , MACRO_PLACE , TRUE , runtype , ROBOT_FM_STATION[1][fms] , TB_STATION , ROBOT_FM_SLOT[1][fms] , ROBOT_FM_SLOT[1][fms] ); // 2010.06.10 // 2014.06.27
					//
				}
			}
			//---------------------------------------------------------------------
			//---------------------------------------------------------------------
			*update = TRUE;
			//---------------------------------------------------------------------
			//---------------------------------------------------------------------
		case RB_RESETTING1_END :
			//
			ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
			//
			if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) break;
			if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) break;
			//
			switch ( ROBOT_FM_MODE[fms] ) {
			case RB_MODE_HOME	:	ROBOT_FM_CONTROL[fms]	= RB_HOMING;		break;
			case RB_MODE_PICK	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING;		break;
			case RB_MODE_PLACE	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING;		break;
				//
			case RB_MODE_ROTATE	:	ROBOT_FM_CONTROL[fms]	= RB_ROTATING;		break;
				//
			case RB_MODE_RETRACT:	ROBOT_FM_CONTROL[fms]	= RB_RETRACTING2;	break; // 2011.04.11
			case RB_MODE_EXTEND	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING;		break; // 2011.04.11
				//
			case RB_MODE_ROTATE	+ 100	:	ROBOT_FM_CONTROL[fms]	= RB_ROTATING;		break; // 2011.12.08
			case RB_MODE_RETRACT + 100	:	ROBOT_FM_CONTROL[fms]	= RB_RETRACTING2;	break; // 2011.12.08
			case RB_MODE_EXTEND	+ 100	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING;		break; // 2011.12.08
			}
			break;

		case RB_RESETTING2 :
			ROBOT_FM_CONTROL[fms] = RB_RESETTING2_END;
			if ( ROBOT_FM_MODE[fms] == RB_MODE_PICK ) {
				if ( ( ROBOT_FM_RUN_ID[fms] >= 3 ) && ( ROBOT_FM_SLOT[1][fms] > 0 ) ) {
					if ( ROBOT_FM_STATION[1][fms] < PM1 ) {
						_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , CWM_PRESENT );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[1][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == AL ) || ( ROBOT_FM_STATION[1][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_AL , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[1][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == IC ) || ( ROBOT_FM_STATION[1][fms] == F_IC ) ) { // 2009.03.24
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
					else {
						//-----------------------------
						IO_WAFER_DATA_GET_TO_FM( fms , TB_STATION , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc , wsts , -1 , wsre );
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
				}
			}
			else if ( ROBOT_FM_MODE[fms] == RB_MODE_PLACE ) {
				if ( ( ROBOT_FM_RUN_ID[fms] >= 3 ) && ( ROBOT_FM_SLOT[1][fms] > 0 ) ) {
					if ( ROBOT_FM_STATION[1][fms] < PM1 ) {
						wsrc = _i_SCH_IO_GET_MODULE_SOURCE( ROBOT_FM_STATION[1][fms] , 1 );
						wsre = _i_SCH_IO_GET_MODULE_SOURCE_E( ROBOT_FM_STATION[1][fms] , 1 );
						wsts = ROBOT_FM_SLOT[1][fms];
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						_i_SCH_IO_SET_MODULE_STATUS( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , CWM_ABSENT );
					}
//					else if ( ROBOT_FM_STATION[1][fms] == AL ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == AL ) || ( ROBOT_FM_STATION[1][fms] == F_AL ) ) { // 2009.03.24
						//-----------------------------
						IO_WAFER_DATA_GET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_CHAMBER( F_AL , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
//					else if ( ROBOT_FM_STATION[1][fms] == IC ) { // 2009.03.24
					else if ( ( ROBOT_FM_STATION[1][fms] == IC ) || ( ROBOT_FM_STATION[1][fms] == F_IC ) ) { // 2009.03.24
						//-----------------------------
						IO_WAFER_DATA_GET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_CHAMBER( F_IC , ROBOT_FM_REAL_IC_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.08.10
						//-----------------------------
					}
					else {
						//-----------------------------
						IO_WAFER_DATA_GET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , &wsrc , &wsts , &wsre ); // 2007.08.10
						//-----------------------------
						IO_WAFER_DATA_SET_TO_FM( fms , TB_STATION , wsrc , wsts , -1 , wsre ); // 2007.08.10
						//-----------------------------
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( ( fms * 10 ) + 1 , TB_STATION , 0 , wsrc + CM1 , wsts , -1 );
						//-----------------------------
						IO_WAFER_DATA_SET_TO_BM( ROBOT_FM_STATION[1][fms] , ROBOT_FM_SLOT[1][fms] , -1 , 0 , -1 , -1 ); // 2007.07.25
						//-----------------------------
					}
				}
			}
			//---------------------------------------------------------------------
			//---------------------------------------------------------------------
			*update = TRUE;
			//---------------------------------------------------------------------
			//---------------------------------------------------------------------
		case RB_RESETTING2_END :
			//
			ss = ROBOT_FM_GET_SYNCH_STATUS( fms );
			//
			if ( ss == ROBOT_SYNCH_RESET_REVERSE1 ) {
				if ( ROBOT_FM_SLOT[0][fms] > 0 ) {
					ROBOT_FM_RUN_ID[fms] = 1;
					ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
				}
				break;
			}
			if ( ss == ROBOT_SYNCH_RESET_REVERSE2 ) break;
			//			
			switch ( ROBOT_FM_MODE[fms] ) {
			case RB_MODE_HOME	:	ROBOT_FM_CONTROL[fms]	= RB_HOMING;		break;
			case RB_MODE_PICK	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING2;		break;
			case RB_MODE_PLACE	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING2;		break;
				//
			case RB_MODE_ROTATE	:	ROBOT_FM_CONTROL[fms]	= RB_ROTATING;		break;
				//
			case RB_MODE_RETRACT:	ROBOT_FM_CONTROL[fms]	= RB_RETRACTING2;	break; // 2011.04.11
			case RB_MODE_EXTEND	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING;		break; // 2011.04.11
				//
			case RB_MODE_ROTATE	+ 100	:	ROBOT_FM_CONTROL[fms]	= RB_ROTATING;		break; // 2011.12.08
			case RB_MODE_RETRACT + 100	:	ROBOT_FM_CONTROL[fms]	= RB_RETRACTING2;	break; // 2011.12.08
			case RB_MODE_EXTEND	+ 100	:	ROBOT_FM_CONTROL[fms]	= RB_STARTING;		break; // 2011.12.08
			}
			break;

	}
	return ROBOT_FM_CONTROL[fms];
}
//--------------------------------------------------------------------------
int ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( int fms , int runtype , BOOL End , BOOL *update ) {
	int res;

	while ( TRUE ) { // 2008.01.11
		res = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL_SUB( fms , runtype , End , update );
		switch ( ROBOT_FM_MODE[fms] ) {
		case RB_MODE_PICK	:
			if ( End ) {
				if ( res != RB_PICKDONE ) continue;
			}
			break;
		case RB_MODE_PLACE	:
			if ( End ) {
				if ( res != RB_PLACEDONE ) continue;
			}
			break;
		}
		//
		break;
		//
	}
	return res;
}
//--------------------------------------------------------------------------
BOOL ROBOT_ANIMATION_PROCESS_CHECK( int ch ); // 2012.09.03

int ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( int mac , int fms , int mode , int ch , int slot ) {
	//
	// mode
	//  -2 : Process(Place)
	//  -1 : Process(Pick)
	//   0 : Pick
	//   1 : Pick(T)
	//   2 : Place
	//   3 : Place(T)
	//   4 : Pick(Disable)
	//   5 : Place(Disable)
	//   ? : Unknown
	//
	int Res;
	char Buffer[256];
	//
	switch( mode ) {
	case -2 :
		sprintf( Buffer , "When %s is Placing , Processing has been detected in %s" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM)  , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) );
		break;
	case -1 :
		sprintf( Buffer , "When %s is Picking , Processing has been detected in %s" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM)  , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) );
		break;
	case 0 :
		sprintf( Buffer , "When %s is Picking , %s has not been detected in %s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) , slot );
		break;
	case 1 :
		sprintf( Buffer , "When %s is Picking , %s has been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM)  , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , slot + 'A' );
		break;
	case 2 :
		sprintf( Buffer , "When %s is Placing , %s has been detected in %s:%d" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(ch) , slot );
		break;
	case 3 :
		sprintf( Buffer , "When %s is Placing , %s has not been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM)  , _i_SCH_PRM_GET_DFIX_METHOD_NAME() , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , slot + 'A' );
		break;
	case 4 : // 2014.08.28
		sprintf( Buffer , "When %s is Picking , Disabled Arm %c has been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , slot + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , slot + 'A' );
		break;
	case 5 : // 2014.08.28
		sprintf( Buffer , "When %s is Placing , Disabled Arm %c has been detected in %s:%c" , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , slot + 'A' , _i_SCH_SYSTEM_GET_MODULE_NAME(FM) , slot + 'A' );
		break;
	default :
		sprintf( Buffer , "" );
		break;
	}
	//
	if ( !ALARM_MESSAGE_SET( mac + 10 + fms , Buffer ) ) {
		//
		ALARM_POST( mac + 10 + fms );
		//
		ALARM_CLEAR( mac + 10 + fms );
		//
		Sleep(250);
		//
		ALARM_MESSAGE_SET( mac + 10 + fms , Buffer );
		//
	}
	//
	Res = ALARM_MANAGE( mac + 10 + fms );
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

BOOL ROBOT_ANIMATION_FM_WAFER_CHECK( int fms , int mode , int STATION , int SLOT1 , int SLOT2 ) {
	int Res , mac , pac;

	if ( GET_RUN_LD_CONTROL(0) > 0 ) { // 2014.01.09
		mac = 20;
//		pac = 40; // 2016.12.09
		pac = 0; // 2016.12.09
	}
	else {
		mac = _i_SCH_PRM_GET_WAFER_ANIM_STATUS_CHECK() % 100000;	if ( mac <= 0 ) return TRUE;
		pac = _i_SCH_PRM_GET_WAFER_ANIM_STATUS_CHECK() / 100000;
	}
	//
//printf( "WAFER_CHECK [fms=%d][mode=%d][STATION=%d][SLOT1=%d][SLOT2=%d]\n" , fms , mode , STATION , SLOT1 , SLOT2 );

	while( TRUE ) {
		//
//		if ( pac > 0 ) { // 2013.03.20
		if ( pac <= 0 ) { // 2013.03.20
			if ( !ROBOT_ANIMATION_PROCESS_CHECK( STATION ) ) { // 2012.09.03
				Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , ( mode == RB_MODE_PICK ) ? -1 : -2 , STATION , 0 );
				if ( Res ==  1 ) continue;
				if ( Res == -1 ) return FALSE;
			}
		}
		//
		switch ( mode ) {
		case RB_MODE_PICK			:
			if ( SLOT1 > 0 ) {
				//
				if ( !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2014.08.28
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 4 , fms , TA_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
				//
				if ( STATION < PM1 ) {
//					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT1 ) != CWM_PRESENT ) { // 2018.10.31
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT1 ) < CWM_PRESENT ) { // 2018.10.31
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT1 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else if ( ( STATION == AL ) || ( STATION == F_AL ) ) { // 2010.11.05
					if ( SLOT1 <= 1 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , SLOT1 ) <= 0 ) {
							Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT1 );
							if ( Res ==  1 ) continue;
							if ( Res == -1 ) return FALSE;
						}
					}
					else {
						if ( _i_SCH_IO_GET_MODULE_STATUS( 1001 , SLOT1 ) <= 0 ) { // 2013.03.20
							Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT1 );
							if ( Res ==  1 ) continue;
							if ( Res == -1 ) return FALSE;
						}
					}
				}
				else if ( ( STATION == IC ) || ( STATION == F_IC ) ) { // 2010.11.05
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , SLOT1 ) <= 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT1 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT1 ) <= 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT1 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) > 0 ) {
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 1 , fms , TA_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
			}
			if ( SLOT2 > 0 ) {
				//
				if ( !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2014.08.28
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 4 , fms , TB_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
				//
				if ( STATION < PM1 ) {
//					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT2 ) != CWM_PRESENT ) { // 2018.10.31
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT2 ) < CWM_PRESENT ) { // 2018.10.31
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else if ( ( STATION == AL ) || ( STATION == F_AL ) ) { // 2010.11.05
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , SLOT2 ) <= 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else if ( ( STATION == IC ) || ( STATION == F_IC ) ) { // 2010.11.05
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , SLOT2 ) <= 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT2 ) <= 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 0 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				//
				if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) > 0 ) {
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 1 , fms , TB_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
			}
			break;

		case RB_MODE_PLACE			:
			if ( SLOT1 > 0 ) {
				//
				if ( !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) { // 2014.08.28
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 5 , fms , TA_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
				//
				if ( STATION < PM1 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT1 ) != CWM_ABSENT ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT1 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else if ( ( STATION == AL ) || ( STATION == F_AL ) ) { // 2010.11.05
					if ( SLOT1 <= 1 ) {
						if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , SLOT1 ) > 0 ) {
							Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT1 );
							if ( Res ==  1 ) continue;
							if ( Res == -1 ) return FALSE;
						}
					}
					else { // 2013.03.20
						if ( _i_SCH_IO_GET_MODULE_STATUS( 1001 , SLOT1 ) > 0 ) {
							Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT1 );
							if ( Res ==  1 ) continue;
							if ( Res == -1 ) return FALSE;
						}
					}
				}
				else if ( ( STATION == IC ) || ( STATION == F_IC ) ) { // 2010.11.05
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , SLOT1 ) > 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT1 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT1 ) > 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT1 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				if ( WAFER_STATUS_IN_FM( fms , TA_STATION ) <= 0 ) {
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 3 , fms , TA_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
			}
			if ( SLOT2 > 0 ) {
				//
				if ( !_i_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2014.08.28
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 5 , fms , TB_STATION );
					if ( Res ==  1 ) continue;
					if ( Res == -1 ) return FALSE;
				}
				//
				if ( STATION < PM1 ) {
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT2 ) != CWM_ABSENT ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else if ( ( STATION == AL ) || ( STATION == F_AL ) ) { // 2010.11.05
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_AL , SLOT2 ) > 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else if ( ( STATION == IC ) || ( STATION == F_IC ) ) { // 2010.11.05
					if ( _i_SCH_IO_GET_MODULE_STATUS( F_IC , SLOT2 ) > 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				else {
					if ( _i_SCH_IO_GET_MODULE_STATUS( STATION , SLOT2 ) > 0 ) {
						Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 2 , STATION , SLOT2 );
						if ( Res ==  1 ) continue;
						if ( Res == -1 ) return FALSE;
					}
				}
				if ( WAFER_STATUS_IN_FM( fms , TB_STATION ) <= 0 ) {
					Res = ROBOT_ANIMATION_FM_WAFER_FAIL_CHECK( mac , fms , 3 , fms , TB_STATION );
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
BOOL ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( int fms , int MODE , int STATION , int SLOT1 , int SLOT2 , int SYNCH , BOOL MaintInfMode , BOOL AnimUse ) {
	int lp , offset_3p1;
	//
	if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() != 0 ) { // 2005.09.30
		if ( _SCH_COMMON_BM_2MODULE_SAME_BODY() == 3 ) { // 2006.12.18
			ROBOT_FM_STATION[0][fms]		= STATION;
			ROBOT_FM_STATION[1][fms]		= STATION;
			ROBOT_FM_SLOT[0][fms]			= SLOT1;
			ROBOT_FM_SLOT[1][fms]			= SLOT2;
			//
			ROBOT_FM_REAL_IC_SLOT[0][fms]	= SLOT1; // 2007.07.02
			ROBOT_FM_REAL_IC_SLOT[1][fms]	= SLOT2; // 2007.07.02
			//
			if ( _i_SCH_PRM_GET_MODULE_ALIGN_BUFFER_CHAMBER() == STATION ) {
				ROBOT_FM_BMMDLEXT[fms] = FALSE; // 2006.12.18
			}
			else {
				ROBOT_FM_BMMDLEXT[fms] = TRUE; // 2006.12.18
			}
		}
		else {
			//-------------------------------------------------------------------------------------------------------
			ROBOT_ANIM_FM_RUN_ONEBODY_Anim_Slot( STATION , SLOT1 , &(ROBOT_FM_STATION[0][fms]) , &(ROBOT_FM_SLOT[0][fms]) );
			ROBOT_ANIM_FM_RUN_ONEBODY_Anim_Slot( STATION , SLOT2 , &(ROBOT_FM_STATION[1][fms]) , &(ROBOT_FM_SLOT[1][fms]) );
			//-------------------------------------------------------------------------------------------------------
			ROBOT_FM_BMMDLEXT[fms] = FALSE; // 2006.12.18
		}
		ROBOT_FM_MAINTINF[fms] = MaintInfMode; // 2008.02.26
	}
	else {
		ROBOT_FM_STATION[0][fms]		= STATION;
		ROBOT_FM_STATION[1][fms]		= STATION;
		ROBOT_FM_SLOT[0][fms]			= SLOT1;
		ROBOT_FM_SLOT[1][fms]			= SLOT2;
		//
		ROBOT_FM_REAL_IC_SLOT[0][fms]	= SLOT1; // 2007.07.02
		ROBOT_FM_REAL_IC_SLOT[1][fms]	= SLOT2; // 2007.07.02
		//
		ROBOT_FM_BMMDLEXT[fms]		= FALSE; // 2006.12.18

		//==================================================================================
		// 2007.07.02
		//==================================================================================
		if ( ROBOT_FM_STATION[0][fms] == IC ) {
			if ( ROBOT_FM_SLOT[0][fms] > MAX_CASSETTE_SLOT_SIZE )	ROBOT_FM_SLOT[0][fms] = ROBOT_FM_SLOT[0][fms] - MAX_CASSETTE_SLOT_SIZE;
		}
		if ( ROBOT_FM_STATION[1][fms] == IC ) {
			if ( ROBOT_FM_SLOT[1][fms] > MAX_CASSETTE_SLOT_SIZE )	ROBOT_FM_SLOT[1][fms] = ROBOT_FM_SLOT[1][fms] - MAX_CASSETTE_SLOT_SIZE;
		}
		//==================================================================================
		ROBOT_FM_MAINTINF[fms] = MaintInfMode; // 2008.02.26
	}
	//
	ROBOT_FM_ANIMUSE[fms] = AnimUse; // 2011.07.27
	//
	ROBOT_FM_MODE[fms]			= MODE;
	//
	if ( SYNCH < 0 ) { // 2015.12.16
		//
		ROBOT_FM_SYNCH_CHECK[fms]	= 0;
		//
	}
	else {
		//
		ROBOT_FM_SYNCH_CHECK[fms]	= SYNCH;
		//
		if ( ( ROBOT_FM_MODE[fms] != RB_MODE_PICK_UPDATE ) && ( ROBOT_FM_MODE[fms] != RB_MODE_PLACE_UPDATE ) ) {
			if ( ROBOT_FM_SYNCH_CHECK[fms] > 0 ) ROBOT_FM_SET_SYNCH_STATUS( fms , ROBOT_SYNCH_READY );
		}
	}
	//
	ROBOT_FM_SYNCH_IGNORING[fms] = 0; // 2016.02.18
	//
	// 2015.12.15
	//
	if ( !AnimUse ) {
		switch ( ROBOT_FM_MODE[fms] ) {
		case RB_MODE_EXTPIC	:
		case RB_MODE_PICK	:
			for ( lp = 0 ; lp < 2 ; lp++ ) {
				//
				if ( ROBOT_FM_STATION[lp][fms] < PM1 ) {
					//
					switch( _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[lp][fms] , ROBOT_FM_SLOT[lp][fms] ) ) {
					case CWM_UNKNOWN :
					case CWM_ABSENT :
						ROBOT_FM_ANX1_WSTS[lp][fms] = 0;
						break;
					default :
						ROBOT_FM_ANX1_WSTS[lp][fms] = ROBOT_FM_SLOT[lp][fms];
						break;
					}
					//
				}
				else if ( ( ROBOT_FM_STATION[lp][fms] == AL ) || ( ROBOT_FM_STATION[lp][fms] == F_AL ) ) {
					if ( lp == 0 ) {
						//-----------------------------
						if ( ROBOT_FM_SLOT[lp][fms] <= 1 ) {
							ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( F_AL , ROBOT_FM_SLOT[lp][fms] );
						}
						else {
							ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( 1001 , ROBOT_FM_SLOT[lp][fms] );
						}
						//-----------------------------
					}
					else {
						if ( _i_SCH_PRM_GET_DFIX_FAL_MULTI_CONTROL() == 1 ) {
							ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( 1001 , 1 );
						}
						else {
							if ( ROBOT_FM_SLOT[lp][fms] <= 1 ) {
								ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( F_AL , ROBOT_FM_SLOT[lp][fms] );
							}
							else {
								ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( 1001 , ROBOT_FM_SLOT[lp][fms] );
							}
						}
					}
				}
				else if ( ( ROBOT_FM_STATION[lp][fms] == IC ) || ( ROBOT_FM_STATION[lp][fms] == F_IC ) ) {
					ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( F_IC , ROBOT_FM_REAL_IC_SLOT[lp][fms] );
				}
				else {
					if ( lp == 0 ) {
						ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[lp][fms] , ROBOT_FM_SLOT[lp][fms] );
					}
					else {
						if ( ROBOT_FM_BMMDLEXT[fms] && ( ROBOT_FM_SLOT[lp][fms] > 2 ) ) {
							ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[lp][fms] + _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms ) + 1 , ROBOT_FM_SLOT[lp][fms] / 4 );
						}
						else {
							ROBOT_FM_ANX1_WSTS[lp][fms] = _i_SCH_IO_GET_MODULE_STATUS( ROBOT_FM_STATION[lp][fms] , ROBOT_FM_SLOT[lp][fms] );
						}
					}
				}
			}
			break;

		case RB_MODE_EXTPLC	:
		case RB_MODE_PLACE	:
			//
			for ( lp = 0 ; lp < 2 ; lp++ ) {
				//
				if ( ROBOT_ANIM_FM_APLUSB_SWAP(fms) ) {
					if ( lp == 0 ) {
						offset_3p1 = 1;
					}
					else {
						offset_3p1 = _i_SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( fms );
					}
				}
				else {
					offset_3p1 = 0;
				}
				//
				if ( ROBOT_FM_STATION[lp][fms] < PM1 ) {
					ROBOT_FM_ANX1_WSTS[lp][fms] = WAFER_STATUS_IN_FM( fms , lp + offset_3p1 );
				}
				else if ( ( ROBOT_FM_STATION[lp][fms] == AL ) || ( ROBOT_FM_STATION[lp][fms] == F_AL ) ) {
					ROBOT_FM_ANX1_WSTS[lp][fms] = WAFER_STATUS_IN_FM( fms , lp );
				}
				else if ( ( ROBOT_FM_STATION[lp][fms] == IC ) || ( ROBOT_FM_STATION[lp][fms] == F_IC ) ) {
					ROBOT_FM_ANX1_WSTS[lp][fms] = WAFER_STATUS_IN_FM( fms , lp + offset_3p1 );
				}
				else {
					ROBOT_FM_ANX1_WSTS[lp][fms] = WAFER_STATUS_IN_FM( fms , lp + offset_3p1 );
				}
				//
			}
			break;
		}
	}
	//
	//
	//
	switch ( ROBOT_FM_MODE[fms] ) {
	case RB_MODE_HOME	:
		ROBOT_FM_UPDATING[fms] = 0; // 2018.06.14
		ROBOT_FM_CONTROL[fms]	= RB_HOMING;
		break;
	case RB_MODE_PICK	:
		//
		if ( SYNCH >= 0 ) { // 2015.12.16
			if ( !ROBOT_ANIMATION_FM_WAFER_CHECK( fms , RB_MODE_PICK , STATION , SLOT1 , SLOT2 ) ) return FALSE;
		}
		//
		ROBOT_FM_UPDATING[fms] = 0; // 2018.06.14
		ROBOT_FM_CONTROL[fms]	= RB_STARTING;
		break;
	case RB_MODE_PLACE	:
		//
		if ( SYNCH >= 0 ) { // 2015.12.16
			if ( !ROBOT_ANIMATION_FM_WAFER_CHECK( fms , RB_MODE_PLACE , STATION , SLOT1 , SLOT2 ) ) return FALSE;
		}
		//
		ROBOT_FM_UPDATING[fms] = 0; // 2018.06.14
		ROBOT_FM_CONTROL[fms]	= RB_STARTING;
		break;
	case RB_MODE_ROTATE	:
	case RB_MODE_ROTATE	+ 100 : // 2011.12.08
		ROBOT_FM_CONTROL[fms]	= RB_STARTING;
		break;
		//

	// 2011.04.11
	case RB_MODE_UP				:
	case RB_MODE_UP	+ 100 : // 2011.12.08
		ROBOT_FM_CONTROL[fms]	= RB_HOMING;
		break;
	case RB_MODE_DOWN			:
	case RB_MODE_DOWN	+ 100 : // 2011.12.08
		ROBOT_FM_CONTROL[fms]	= RB_HOMING;
		break;
	case RB_MODE_RETRACT		:
	case RB_MODE_RETRACT	+ 100 : // 2011.12.08
		ROBOT_FM_CONTROL[fms]	= RB_STARTING;
		break;
	case RB_MODE_EXTEND			:
	case RB_MODE_EXTEND	+ 100 : // 2011.12.08
		ROBOT_FM_CONTROL[fms]	= RB_STARTING;
		break;
	//


	}
	//
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




void ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( int fms , int MODE , int STATION , int SLOT1 , int SLOT2 ) { // 2018.06.14
	BOOL update;
	//
	if ( ROBOT_FM_UPDATING[fms] != 0 ) {
		//
		ROBOT_FM_CONTROL[fms] = RB_RESETTING1;
		ROBOT_FM_MODE[fms] = MODE;
		//
		ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , -99 , FALSE , &update );
		//
		if ( update ) _i_SCH_IO_MTL_SAVE();
	}
	//
	ROBOT_FM_CONTROL[fms] = RB_RETRACTING;
	//
	ROBOT_FM_MODE[fms]			= MODE;
	//
	ROBOT_FM_STATION[0][fms]		= STATION;
	ROBOT_FM_STATION[1][fms]		= STATION;
	ROBOT_FM_SLOT[0][fms]			= SLOT1;
	ROBOT_FM_SLOT[1][fms]			= SLOT2;
	//
	ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , -99 , TRUE , &update );
	//
}
