#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "iolog.h"
#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_log.h"
#include "Robot_Animation.h"
#include "Timer_Handling.h"
#include "Equip_Handling.h"
#include "sch_sub.h"
#include "sch_estimate.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Home FM/BM
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_HOME_FROM_FMBM( int fms , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 ) {
	int timex = 100;
	char ParamF[256];
	int Mf , Mr;
	BOOL FM_Run;
	int Rb_Run;
	long Mon_Goal;
	BOOL update;
	int l_c; // 2008.09.14

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , TRUE ); // 2012.10.31

	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms );

	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( EQUIP_ADDRESS_FM(fms) >= 0 ) && ( GET_RUN_LD_CONTROL(0) <= 0 ) )
		FM_Run = TRUE;
	else
		FM_Run = FALSE;

	if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_HOME , Chamber , Slot1 , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , FALSE , ( Rb_Run == 1 ) ) ) {
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
		return SYS_ABORTED;
	}

	if ( EQUIP_INTERFACE_INVALID_STATION( Chamber ) ) {
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
		return SYS_ABORTED;
	}

	sprintf( ParamF , "HOME_WAFER %s %d %d" , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot1 , Slot2 );

	EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
	if ( Rb_Run == 1 ) {
		if ( FM_Run ) {
//			_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mf = SYS_RUNNING; // 2008.04.03
			}
			else {
				Mf = SYS_ABORTED; // 2008.04.03
			}
		}
		else {
			Mf = SYS_RUNNING; // 2008.04.03
		}
//		Mf = SYS_RUNNING; // 2008.04.03
		while ( TRUE ) {
			//========================
			_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
			//========================
			Mon_Goal = Clock_Goal_Get_Start_Time();
			if ( FM_Run ) {
				if ( Mf == SYS_RUNNING ) {
					Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
				}
				if ( Mf == SYS_ABORTED ) {
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				if ( Rb_Run == 1 ) {
					Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
					if ( update ) _i_SCH_IO_MTL_SAVE();
					if ( Mr == RB_HOMEDONE ) {
						if ( Mf != SYS_RUNNING ) break;
						Rb_Run = 0;
					}
				}
				else {
					if ( Mf != SYS_RUNNING ) break;
				}
			}
			else {
				if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_HOMEDONE ) {
					if ( update ) _i_SCH_IO_MTL_SAVE();
					break;
				}
				if ( update ) _i_SCH_IO_MTL_SAVE();
			}
			l_c = 0; // 2008.09.14
			do {
				if ( MANAGER_ABORT() ) {
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				//===================================================================
				// 2005.10.26
				//===================================================================
				if ( FM_Run ) {
					if ( Mf == SYS_RUNNING ) {
						if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
							break;
						}
					}
				}
				//===================================================================
				//
				switch ( GET_RUN_LD_CONTROL(0) ) {
				case 1 :
					timex = 0;
					break;
				case 2 :
					timex = 20;
					break;
				}
				//
				//===================================================================
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			while( (!Clock_Goal_Data_Check( &Mon_Goal , timex )) && ( Rb_Run == 1 ) );
		}
	}
	else {
		if ( FM_Run ) {
			if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) {
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
				return SYS_ABORTED;
			}
		}
		//==================================================================================================
		// 2006.11.09
		//==================================================================================================
		while ( TRUE ) {
			if ( RB_HOMEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , Rb_Run , FALSE , &update ) ) break;
		}
		_i_SCH_IO_MTL_SAVE();
		//==================================================================================================
	}
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
	return SYS_SUCCESS;
}

int EQUIP_ROBOT_FROM_FMBM( int fms , int mode , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 , BOOL MaintInfUse ) {
	int timex = 100;
	char ParamF[256];
	int Mf , Mr , rmode , rres;
	BOOL FM_Run;
	int Rb_Run;
	long Mon_Goal;
	BOOL update;
	int l_c; // 2008.09.14
	char MsgAppchar[2]; /* 2013.04.26 */
	//

	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//========================
	//
	if ( Chamber != 0 ) { // 2012.10.31
		if ( EQUIP_INTERFACE_INVALID_STATION( Chamber ) ) { // 2012.10.31
			return SYS_ABORTED;
		}
	}
	//
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , TRUE ); // 2012.10.31
	if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , TRUE ); // 2012.10.31

	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms );

	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( EQUIP_ADDRESS_FM(fms) >= 0 ) && ( GET_RUN_LD_CONTROL(4) <= 0 ) )
		FM_Run = 1;
	else
		FM_Run = 0;
	//
	switch( mode ) {
	case 0 : rmode = RB_MODE_EXTEND;	rres = RB_EXTENDDONE;	break;
	case 1 : rmode = RB_MODE_RETRACT;	rres = RB_RETRACTDONE;	break;
	case 2 : rmode = RB_MODE_UP;		rres = RB_HOMEDONE;	break;
	case 3 : rmode = RB_MODE_DOWN;		rres = RB_HOMEDONE;	break;
	case 4 : rmode = RB_MODE_ROTATE;	rres = RB_ROTATEDONE;	break;
	default :
		if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
		return SYS_ABORTED;
		break;
	}
	//
	if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , ( Chamber == 0 ) ? ( rmode + 100 ) : rmode , Chamber , Slot1 , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , FALSE , ( Rb_Run == 1 ) ) ) {
		if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
		return SYS_ABORTED;
	}
	//
	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */
	//
	if ( Chamber == 0 ) { // 2011.12.08
		switch( mode ) {
		case 0 : sprintf( ParamF , "EXTEND_WAFER%s - %c %d"		, MsgAppchar , ( Slot1 == 0 ) ? 'A' : 'B' , Slot2 );	break;
		case 1 : sprintf( ParamF , "RETRACT_WAFER%s - %c %d"	, MsgAppchar , ( Slot1 == 0 ) ? 'A' : 'B' , Slot2 );	break;
		case 2 : sprintf( ParamF , "UP_WAFER%s - %c %d"			, MsgAppchar , ( Slot1 == 0 ) ? 'A' : 'B' , Slot2 );	break;
		case 3 : sprintf( ParamF , "DOWN_WAFER%s - %c %d"		, MsgAppchar , ( Slot1 == 0 ) ? 'A' : 'B' , Slot2 );	break;
		case 4 : sprintf( ParamF , "ROTATE_WAFER%s - %c %d"		, MsgAppchar , ( Slot1 == 0 ) ? 'A' : 'B' , Slot2 );	break;
		default :
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
			break;
		}
	}
	else {
		//
//		if ( EQUIP_INTERFACE_INVALID_STATION( Chamber ) ) return SYS_ABORTED; // 2012.10.31
		//
		switch( mode ) {
		case 0 : sprintf( ParamF , "EXTEND_WAFER%s %s %d %d"	, MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot1 , Slot2 );	break;
		case 1 : sprintf( ParamF , "RETRACT_WAFER%s %s %d %d"	, MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot1 , Slot2 );	break;
		case 2 : sprintf( ParamF , "UP_WAFER%s %s %d %d"		, MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot1 , Slot2 );	break;
		case 3 : sprintf( ParamF , "DOWN_WAFER%s %s %d %d"		, MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot1 , Slot2 );	break;
		case 4 : sprintf( ParamF , "ROTATE_WAFER%s %s %d %d"	, MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot1 , Slot2 );	break;
		default :
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
			break;
		}
	}
	//
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
	if ( Rb_Run == 1 ) {
		if ( FM_Run ) {
			if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mf = SYS_RUNNING; // 2008.04.03
			}
			else {
				Mf = SYS_ABORTED; // 2008.04.03
			}
		}
		else {
			Mf = SYS_RUNNING; // 2008.04.03
		}
		while ( TRUE ) {
			//========================
			_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
			//========================
			Mon_Goal = Clock_Goal_Get_Start_Time();
			if ( FM_Run ) {
				if ( Mf == SYS_RUNNING ) {
					Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
				}
				if ( Mf == SYS_ABORTED ) {
					if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				if ( Rb_Run == 1 ) {
					Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
					if ( update ) _i_SCH_IO_MTL_SAVE();
					if ( Mr == rres ) {
						if ( Mf != SYS_RUNNING ) break;
						Rb_Run = 0;
					}
				}
				else {
					if ( Mf != SYS_RUNNING ) break;
				}
			}
			else {
				if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == rres ) {
					if ( update ) _i_SCH_IO_MTL_SAVE();
					break;
				}
				if ( update ) _i_SCH_IO_MTL_SAVE();
			}
			l_c = 0; // 2008.09.14
			do {
				if ( MANAGER_ABORT() ) {
					if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				//===================================================================
				// 2005.10.26
				//===================================================================
				if ( FM_Run ) {
					if ( Mf == SYS_RUNNING ) {
						if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
							break;
						}
					}
				}
				//===================================================================
				//
				switch( GET_RUN_LD_CONTROL(0) ) {
				case 1 :
					timex = 0;
					break;
				case 2 :
					timex = 20;
					break;
				}
				//
				//===================================================================
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			while( (!Clock_Goal_Data_Check( &Mon_Goal , timex )) && ( Rb_Run == 1 ) );
		}
	}
	else {
		//
		if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
			while ( TRUE ) {
				if ( rres == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
			}
		}
		//
		if ( FM_Run ) {
			if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) {
				if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
				_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
				return SYS_ABORTED;
			}
		}
		//
		if ( Rb_Run == 4 ) { // 2015.12.16
			ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , ( Chamber == 0 ) ? ( rmode + 100 ) : rmode , Chamber , Slot1 , Slot2 , -1 , FALSE , TRUE );
		}
		//
		if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
			//==================================================================================================
			// 2006.11.09
			//==================================================================================================
			while ( TRUE ) {
				if ( rres == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
			}
		}
		//
		_i_SCH_IO_MTL_SAVE();
		//==================================================================================================
	}
	//
	if ( Chamber != 0 ) _i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
	//
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Pick from FM/BM
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_PICK_FROM_FMBM( int fms , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 , BOOL FullRun , int SrcCas , int order , BOOL MaintInfUse ) {
	int timex = 100;
	char ParamF[256] , ParamB[256];
	int Mb , Mf , Mr , s1 , s2;
	BOOL BM_RunF , FM_RunF , BM_RunR , FM_RunR;
	int Rb_Run;
	long Mon_Goal;
	BOOL update;
	int l_c;
	char MsgAppchar[2]; /* 2013.04.26 */
	//
	//===============================================================
	//
	long Sim_Goal; // 2016.12.09
	int  Sim_Trg; // 2016.12.09
	//
	//===============================================================
	//
	//================================================================================================
	if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( ( Chamber < CM1 ) || ( Chamber >= TM ) ) ) { // 2014.11.07
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Fail in EQUIP_PICK_FROM_FMBM(fms=%d,Chamber=%d,Slot1=%d,Slot2=%d,FullRun=%d,SrcCas=%d,order=%d,MaintInfUse=%d)\n" , fms , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order , MaintInfUse );
		return SYS_ABORTED;
	}
	//================================================================================================
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//
//	_EQUIP_RUNNING2_TAG[FM] = TRUE; // 2012.03.23 // 2012.10.31
//	_EQUIP_RUNNING2_TAG[Chamber] = TRUE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , TRUE ); // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , TRUE ); // 2012.10.31
	//========================
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] FMS=%d,Chamber=%d,Slot1=%d,Slot2=%d,FullRun=%d,SrcCas=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PICK_FROM_FMBM" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order );
	}
	//======================================================================================================================================================

	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms );

	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) && ( Address_FUNCTION_INTERFACE[Chamber] >= 0 ) && ( GET_RUN_LD_CONTROL(3) <= 0 ) )
		BM_RunF = TRUE;
	else
		BM_RunF = FALSE;
	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( EQUIP_ADDRESS_FM(fms) >= 0 ) && ( GET_RUN_LD_CONTROL(4) <= 0 ) ) {
		FM_RunF = TRUE;
		Sim_Trg = 0; // 2017.03.24
	}
	else {
		FM_RunF = FALSE;
		//
		//===============================================================
		//
		Sim_Trg = _SCH_Get_SIM_TIME1( CHECKING_SIDE , FM );
		//
		if ( Sim_Trg > 0 ) {
			Sim_Goal = Clock_Goal_Get_Start_Time();
			timex = Sim_Trg / 20;
			if ( timex <= 0 ) timex = 1;
		}
		//===============================================================
		//
	}

	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */

	if ( Chamber < PM1 ) {
		//===========================================================
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2008.06.24
		}
		//===========================================================
		KPLT_START_FM_PICK_ROBOT_TIME( fms , 0 );
		//
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 1 );
		//===========================================================
		if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , Slot1 , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		if ( Rb_Run == 1 ) {
			if ( FM_RunF ) {
				sprintf( ParamF , "PICK_WAFER%s CM%d %d %d" , MsgAppchar , Chamber - CM1 + 1 , Slot1 , Slot2 );
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
//				_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
				if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
					Mf = SYS_RUNNING; // 2008.04.03
				}
				else {
					Mf = SYS_ABORTED; // 2008.04.03
				}
			}
			else {
				Mf = SYS_RUNNING; // 2008.04.03
			}
			//
//			Mf = SYS_RUNNING; // 2008.04.03
			while ( TRUE ) {
				//========================
				_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
				//========================
				Mon_Goal = Clock_Goal_Get_Start_Time();
				if ( FM_RunF ) {
					if ( Mf == SYS_RUNNING ) {
						Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
					}
					if ( Mf == SYS_ABORTED ) {
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
						return SYS_ABORTED;
					}
					if ( Mf == SYS_ERROR ) { // 2018.06.14
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
						//
						ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PICK , Chamber , Slot1 , Slot2 );
						//
						return SYS_ERROR;
					}
					if ( Rb_Run == 1 ) {
						Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == RB_PICKDONE ) {
							if ( Mf != SYS_RUNNING ) break;
							Rb_Run = 0;
						}
					}
					else {
						if ( Mf != SYS_RUNNING ) break;
					}
				}
				else {
					//
					//===============================================================
					//
					if ( Sim_Trg > 0 ) { // 2016.12.09
						if ( Mf == SYS_RUNNING ) {
							if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
								Mf = SYS_SUCCESS;
							}
						}
						if ( Rb_Run == 1 ) {
							Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
							if ( update ) _i_SCH_IO_MTL_SAVE();
							if ( Mr == RB_PICKDONE ) {
								if ( Mf != SYS_RUNNING ) break;
								Rb_Run = 0;
							}
						}
						else {
							if ( Mf != SYS_RUNNING ) break;
						}
					}
					//
					//===============================================================
					//
					else {
						if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PICKDONE ) {
							if ( update ) _i_SCH_IO_MTL_SAVE();
							break;
						}
						if ( update ) _i_SCH_IO_MTL_SAVE();
					}
				}
				l_c = 0; // 2008.09.14
				do {
					if ( MANAGER_ABORT() ) {
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
						return SYS_ABORTED;
					}
					//===================================================================
					// 2005.10.26
					//===================================================================
					if ( FM_RunF ) {
						if ( Mf == SYS_RUNNING ) {
							if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
								break;
							}
						}
					}
					//===================================================================
					//
					switch( GET_RUN_LD_CONTROL(0) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
					//===================================================================
					// Sleep(1); // 2008.09.14
					if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
					l_c++; // 2008.09.14
				}
				while( (!Clock_Goal_Data_Check( &Mon_Goal , timex )) && ( Rb_Run == 1 ) );
			}
		}
		else {
			//
			if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
				}
			}
			//
			if ( FM_RunF ) {
				sprintf( ParamF , "PICK_WAFER%s CM%d %d %d" , MsgAppchar , Chamber - CM1 + 1 , Slot1 , Slot2 );
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
				case SYS_ABORTED :
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				case SYS_ERROR : // 2018.06.14
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
					return SYS_ERROR;
				}
			}
			//
			if ( Rb_Run == 4 ) { // 2015.12.16
				ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , Slot1 , Slot2 , -1 , MaintInfUse , TRUE );
			}
			//==================================================================================================
			// 2006.11.09
			//==================================================================================================
			//
			if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
				}
			}
			//
			_i_SCH_IO_MTL_SAVE();
			//==================================================================================================
		}
		//===========================================================
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
		//
		KPLT_END_FM_PICK_ROBOT_TIME( fms , 0 );
		//===========================================================
	}
//	else if ( Chamber >= BM1 ) { // 2014.11.06
	else { // 2014.11.06
		//===========================================================
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2008.06.24
		}
		//===========================================================
		//===========================================================
		KPLT_START_FM_PICK_ROBOT_TIME( fms , 1 );
		//
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 1 );
		//===========================================================
		//-----------------------------------------------------------------
		// PRE PART
		//-----------------------------------------------------------------
		if ( Slot1 <= 0 ) {
			s1 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		if ( Slot2 <= 0 ) {
			s2 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		//======================================================================================
		// 2005.12.27	// 0 : All Use 1 : FM Not Use 2 : BM Not Use 3 : All NotUse
		//======================================================================================
		BM_RunR = BM_RunF;
		if ( BM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_PREPSEND( MaintInfUse , FM , Chamber ) ) BM_RunR = FALSE; // 2013.04.26
			//
		}
		//
		FM_RunR = FM_RunF;
		if ( FM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_PREPRECV( MaintInfUse , FM , -1 ) ) FM_RunR = FALSE; // 2013.04.26
			//
		}
		//======================================================================================
		if ( BM_RunR ) {
			sprintf( ParamB , "FM_PREPARE_SEND%s %d %d" , MsgAppchar , s1 , s2 );
			if ( FullRun ) strcat( ParamB , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamB );
			Mb = SYS_RUNNING;
		}
		else {
			Mb = SYS_SUCCESS;
		}
		if ( FM_RunR ) {
			//
			if ( Chamber >= BM1 ) { // 2014.11.06
				sprintf( ParamF , "PREPARE_RECV%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
			}
			else { // 2014.11.06
				sprintf( ParamF , "PREPARE_RECV%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
			}
			//
			if ( FullRun ) strcat( ParamF , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
			Mf = SYS_RUNNING;
		}
		else {
			Mf = SYS_SUCCESS;
		}
		//==============================================================================
		// 2005.10.26
		//==============================================================================
		if ( BM_RunR && FM_RunR ) {
//			_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mb = SYS_ABORTED;
				Mf = SYS_ABORTED;
			}
		}
		else if ( BM_RunR ) {
//			_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ) ) Mb = SYS_ABORTED;// 2008.04.03
		}
		else if ( FM_RunR ) {
//			_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) Mf = SYS_ABORTED; // 2008.04.03
		}
		//==============================================================================
		l_c = 0; // 2008.09.14
		while ( TRUE ) {
			if ( FM_RunR && ( Mf == SYS_RUNNING ) ) {
				Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
			}
			if ( BM_RunR && ( Mb == SYS_RUNNING ) ) {
				Mb = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
			}
			if ( ( Mf != SYS_RUNNING ) && ( Mb != SYS_RUNNING ) ) {
				if ( ( Mf == SYS_ABORTED ) || ( Mb == SYS_ABORTED ) ) {
					EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 1 ); // 2007.12.12
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				else if ( Mf == SYS_ERROR ) { // 2005.11.24
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ERROR;
				}
				else if ( Mb == SYS_ERROR ) { // 2005.11.24
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ERROR;
				}
				else {
					break;
				}
			}
//			if ( MANAGER_ABORT() ) { // 2007.12.14
//				return SYS_ABORTED; // 2007.12.14
//			} // 2007.12.14
			// Sleep(1); // 2008.09.14
			if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
			l_c++; // 2008.09.14
		}
		//=================================================================
		// 2007.12.14
		//=================================================================
//		if ( MANAGER_ABORT() ) { // 2007.12.14 // 2008.06.24
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { // 2008.06.24
			EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 11 ); // 2007.12.12
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2007.12.14
		} // 2007.12.14
		//=================================================================
		//-----------------------------------------------------------------
		// ROBOT PART
		//-----------------------------------------------------------------
		if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , Slot1 , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		if ( Rb_Run == 1 ) {
			if ( FM_RunF ) {
				if ( Slot1 <= 0 ) {
					s1 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				if ( Slot2 <= 0 ) {
					s2 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				//
				if ( Chamber >= BM1 ) { // 2014.11.06
					sprintf( ParamF , "PICK_WAFER%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
				}
				else { // 2014.11.06
					sprintf( ParamF , "PICK_WAFER%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
				}
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
//				_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
				if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
					Mf = SYS_RUNNING; // 2008.04.03
				}
				else {
					Mf = SYS_ABORTED; // 2008.04.03
				}
			}
			else {
				Mf = SYS_RUNNING; // 2008.04.03
			}
			//
//			Mf = SYS_RUNNING; // 2008.04.03
			while ( TRUE ) {
				//========================
				_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
				//========================
				Mon_Goal = Clock_Goal_Get_Start_Time();
				if ( FM_RunF ) {
					if ( Mf == SYS_RUNNING ) {
						Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
					}
					if ( Mf == SYS_ABORTED ) {
						EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 3 ); // 2007.12.12
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
						return SYS_ABORTED;
					}
					if ( Mf == SYS_ERROR ) { // 2018.06.14
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
						//
						ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PICK , Chamber , Slot1 , Slot2 );
						//
						return SYS_ERROR;
					}
					if ( Rb_Run == 1 ) {
						Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == RB_PICKDONE ) {
							if ( Mf != SYS_RUNNING ) break;
							Rb_Run = 0;
						}
					}
					else {
						if ( Mf != SYS_RUNNING ) break;
					}
				}
				else {
					//
					//===============================================================
					//
					if ( Sim_Trg > 0 ) { // 2016.12.09
						if ( Mf == SYS_RUNNING ) {
							if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
								Mf = SYS_SUCCESS;
							}
						}
						if ( Rb_Run == 1 ) {
							Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
							if ( update ) _i_SCH_IO_MTL_SAVE();
							if ( Mr == RB_PICKDONE ) {
								if ( Mf != SYS_RUNNING ) break;
								Rb_Run = 0;
							}
						}
						else {
							if ( Mf != SYS_RUNNING ) break;
						}
					}
					//
					//===============================================================
					//
					else {
						if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PICKDONE ) {
							if ( update ) _i_SCH_IO_MTL_SAVE();
							break;
						}
						if ( update ) _i_SCH_IO_MTL_SAVE();
					}
					//
				}
				l_c = 0; // 2008.09.14
				do {
//					if ( MANAGER_ABORT() ) return SYS_ABORTED; // 2007.12.14
					//===================================================================
					// 2005.10.26
					//===================================================================
					if ( FM_RunF ) {
						if ( Mf == SYS_RUNNING ) {
							if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
								break;
							}
						}
					}
					//===================================================================
					//
					switch( GET_RUN_LD_CONTROL(0) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
					//===================================================================
					// Sleep(1); // 2008.09.14
					if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
					l_c++; // 2008.09.14
				}
				while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
			}
		}
		else {
			//
			if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( MANAGER_ABORT() ) break; // 2007.12.14
					if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
				}
			}
			//
			if ( FM_RunF ) {
				if ( Slot1 <= 0 ) {
					s1 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				if ( Slot2 <= 0 ) {
					s2 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				//
				if ( Chamber >= BM1 ) { // 2014.11.06
					sprintf( ParamF , "PICK_WAFER%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
				}
				else { // 2014.11.06
					sprintf( ParamF , "PICK_WAFER%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
				}
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
				case SYS_ABORTED :
					EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 3 ); // 2007.12.12
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				case SYS_ERROR : // 2018.06.14
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
					return SYS_ERROR;
				}
			}
			//
			if ( Rb_Run == 4 ) { // 2015.12.16
				ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PICK , Chamber , Slot1 , Slot2 , -1 , MaintInfUse , TRUE );
			}
			//
			//==================================================================================================
			// 2006.11.09
			//==================================================================================================
			//
			if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( MANAGER_ABORT() ) break; // 2007.12.14
					if ( RB_PICKDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
				}
			}
			//
			_i_SCH_IO_MTL_SAVE();
			//==================================================================================================
		}
		//==============================================================================
		// 2007.12.14
		//==============================================================================
//		if ( MANAGER_ABORT() ) { // 2008.06.24
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { // 2008.06.24
			EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 13 ); // 2007.12.12
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2007.12.14
		}
		//==============================================================================
		//-----------------------------------------------------------------
		// POST PART
		//-----------------------------------------------------------------
		if ( Slot1 <= 0 ) {
			s1 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		if ( Slot2 <= 0 ) {
			s2 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		//======================================================================================
		// 2005.12.27	// 0 : All Use 1 : FM Not Use 2 : BM Not Use 3 : All NotUse
		//======================================================================================
		BM_RunR = BM_RunF;
		if ( BM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_POSTSEND( MaintInfUse , FM , Chamber ) ) BM_RunR = FALSE; // 2013.04.26
			//
		}
		//
		FM_RunR = FM_RunF;
		if ( FM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_POSTRECV( MaintInfUse , FM , -1 ) ) FM_RunR = FALSE; // 2013.04.26
			//
		}
		//======================================================================================
		if ( BM_RunR ) {
			sprintf( ParamB , "FM_POST_SEND%s %d %d" , MsgAppchar , s1 , s2 );
			if ( FullRun ) strcat( ParamB , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamB );
			Mb = SYS_RUNNING;
		}
		else {
			Mb = SYS_SUCCESS;
		}
		if ( FM_RunR ) {
			//
			if ( Chamber >= BM1 ) { // 2014.11.06
				sprintf( ParamF , "POST_RECV%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
			}
			else { // 2014.11.06
				sprintf( ParamF , "POST_RECV%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
			}
			//
			if ( FullRun ) strcat( ParamF , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
			Mf = SYS_RUNNING;
		}
		else {
			Mf = SYS_SUCCESS;
		}
		//==============================================================================
		// 2005.10.26
		//==============================================================================
		if ( BM_RunR && FM_RunR ) {
//			_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mb = SYS_ABORTED;
				Mf = SYS_ABORTED;
			}
		}
		else if ( BM_RunR ) {
//			_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ) ) Mb = SYS_ABORTED; // 2008.04.03
		}
		else if ( FM_RunR ) {
//			_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) Mf = SYS_ABORTED; // 2008.04.03
		}
		//==============================================================================
		l_c = 0; // 2008.09.14
		while ( TRUE ) {
			if ( FM_RunR && ( Mf == SYS_RUNNING ) ) {
				Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
			}
			if ( BM_RunR && ( Mb == SYS_RUNNING ) ) {
				Mb = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
			}
			if ( ( Mf != SYS_RUNNING ) && ( Mb != SYS_RUNNING ) ) {
				if ( ( Mf == SYS_ABORTED ) || ( Mb == SYS_ABORTED ) ) {
					EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 5 ); // 2007.12.12
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				else if ( ( Mf == SYS_ERROR ) || ( Mb == SYS_ERROR ) ) { // 2018.06.14
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
					return SYS_ERROR;
				}
				else {
					break;
				}
			}
//			if ( MANAGER_ABORT() ) return SYS_ABORTED; // 2007.12.14
			// Sleep(1); // 2008.09.14
			if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
			l_c++; // 2008.09.14
		}
		//===========================================================
		// 2007.12.14
		//===========================================================
//		if ( MANAGER_ABORT() ) { // 2008.06.24
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { // 2008.06.24
			EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 0 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 15 ); // 2007.12.12
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2007.12.14
		}
		//===========================================================
		//--
		//===========================================================
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
		//
		KPLT_END_FM_PICK_ROBOT_TIME( fms , 1 );
		//===========================================================
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] FMS=%d,Chamber=%d,Slot1=%d,Slot2=%d,FullRun=%d,SrcCas=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PICK_FROM_FMBM" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order );
	}
	//======================================================================================================================================================
	//
//	_EQUIP_RUNNING2_TAG[FM] = FALSE; // 2012.03.23 // 2012.10.31
//	_EQUIP_RUNNING2_TAG[Chamber] = FALSE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
	//
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Place to FM/BM
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_PLACE_TO_FMBM( int fms , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 , int FullRun , int SrcCas , int order , BOOL MaintInfUse ) {
	int timex = 100;
	char ParamF[256] , ParamB[256];
	int Mb , Mf , Mr , s1 , s2;
	BOOL BM_RunF , FM_RunF , BM_RunR , FM_RunR;
	int Rb_Run;
	long Mon_Goal;
	BOOL update;
	int l_c;
	char MsgAppchar[2]; /* 2013.04.26 */
	//
	//===============================================================
	//
	long Sim_Goal; // 2016.12.09
	int  Sim_Trg; // 2016.12.09
	//
	//===============================================================
	//
	//================================================================================================
	if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( ( Chamber < CM1 ) || ( Chamber >= TM ) ) ) { // 2014.11.07
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Fail in EQUIP_PLACE_TO_FMBM(fms=%d,Chamber=%d,Slot1=%d,Slot2=%d,FullRun=%d,SrcCas=%d,order=%d,MaintInfUse=%d)\n" , fms , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order , MaintInfUse );
		return SYS_ABORTED;
	}
	//================================================================================================
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//
//	_EQUIP_RUNNING2_TAG[FM] = TRUE; // 2012.03.23 // 2012.10.31
//	_EQUIP_RUNNING2_TAG[Chamber] = TRUE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , TRUE ); // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , TRUE ); // 2012.10.31
	//========================
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] FMS=%d,Chamber=%d,Slot1=%d,Slot2=%d,FullRun=%d,SrcCas=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_TO_FMBM" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order );
	}
	//======================================================================================================================================================

	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms );

	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( Chamber ) && ( Address_FUNCTION_INTERFACE[Chamber] >= 0 ) && ( GET_RUN_LD_CONTROL(3) <= 0 ) )
		BM_RunF = TRUE;
	else
		BM_RunF = FALSE;
	if ( _i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) && ( EQUIP_ADDRESS_FM(fms) >= 0 ) && ( GET_RUN_LD_CONTROL(4) <= 0 ) ) {
		FM_RunF = TRUE;
		Sim_Trg = 0; // 2017.03.24
	}
	else {
		FM_RunF = FALSE;
		//
		//===============================================================
		//
		Sim_Trg = _SCH_Get_SIM_TIME2( CHECKING_SIDE , FM );
		//
		if ( Sim_Trg > 0 ) {
			Sim_Goal = Clock_Goal_Get_Start_Time();
			timex = Sim_Trg / 20;
			if ( timex <= 0 ) timex = 1;
		}
		//
		//===============================================================
		//
	}

	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */

	if ( Chamber < PM1 ) {
		//===========================================================
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2008.06.24
		}
		//===========================================================
		KPLT_START_FM_PLACE_ROBOT_TIME( fms , 0 );
		//
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 1 );
		//===========================================================
		if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , Slot1 , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		if ( Rb_Run == 1 ) {
			if ( FM_RunF ) {
				sprintf( ParamF , "PLACE_WAFER%s CM%d %d %d" , MsgAppchar , Chamber - CM1 + 1 , Slot1 , Slot2 );
				//--------------------------------------------------------------------------------
				// 2006.03.22
				//--------------------------------------------------------------------------------
				if ( FullRun == 2 ) strcat( ParamF , " RET" );
				//--------------------------------------------------------------------------------
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
//				_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
				if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
					Mf = SYS_RUNNING; // 2008.04.03
				}
				else {
					Mf = SYS_ABORTED; // 2008.04.03
				}
			}
			else {
				Mf = SYS_RUNNING; // 2008.04.03
			}
//			Mf = SYS_RUNNING; // 2008.04.03
			while ( TRUE ) {
				//========================
				_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
				//========================
				Mon_Goal = Clock_Goal_Get_Start_Time();
				if ( FM_RunF ) {
					if ( Mf == SYS_RUNNING ) {
						Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
					}
					if ( Mf == SYS_ABORTED ) {
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
						return SYS_ABORTED;
					}
					if ( Mf == SYS_ERROR ) { // 2018.06.14
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
						//
						ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PLACE , Chamber , Slot1 , Slot2 );
						//
						return SYS_ERROR;
					}
					if ( Rb_Run == 1 ) {
						Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == RB_PLACEDONE ) {
							if ( Mf != SYS_RUNNING ) break;
							Rb_Run = 0;
						}
					}
					else {
						if ( Mf != SYS_RUNNING ) break;
					}
				}
				else {
					//
					//===============================================================
					//
					if ( Sim_Trg > 0 ) { // 2016.12.09
						if ( Mf == SYS_RUNNING ) {
							if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
								Mf = SYS_SUCCESS;
							}
						}
						if ( Rb_Run == 1 ) {
							Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
							if ( update ) _i_SCH_IO_MTL_SAVE();
							if ( Mr == RB_PLACEDONE ) {
								if ( Mf != SYS_RUNNING ) break;
								Rb_Run = 0;
							}
						}
						else {
							if ( Mf != SYS_RUNNING ) break;
						}
					}
					//
					//===============================================================
					//
					else {
						if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) {
							if ( update ) _i_SCH_IO_MTL_SAVE();
							break;
						}
						if ( update ) _i_SCH_IO_MTL_SAVE();
					}
					//
				}
				l_c = 0; // 2008.09.14
				do {
					if ( MANAGER_ABORT() ) {
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
						return SYS_ABORTED;
					}
					//===================================================================
					// 2005.10.26
					//===================================================================
					if ( FM_RunF ) {
						if ( Mf == SYS_RUNNING ) {
							if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
								break;
							}
						}
					}
					//===================================================================
					//
					switch( GET_RUN_LD_CONTROL(0) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
					//===================================================================
					// Sleep(1); // 2008.09.14
					if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
					l_c++; // 2008.09.14
				}
				while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
			}
		}
		else {
			//
			if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
				}
			}
			//
			if ( FM_RunF ) {
				sprintf( ParamF , "PLACE_WAFER%s CM%d %d %d" , MsgAppchar , Chamber - CM1 + 1 , Slot1 , Slot2 );
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
				case SYS_ABORTED :
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				case SYS_ERROR :
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
					return SYS_ERROR;
				}
			}
			//
			if ( Rb_Run == 4 ) { // 2015.12.16
				ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , Slot1 , Slot2 , -1 , MaintInfUse , TRUE );
			}
			//
			//==================================================================================================
			// 2006.11.09
			//==================================================================================================
			//
			if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
				}
			}
			//
			_i_SCH_IO_MTL_SAVE();
			//==================================================================================================
		}
		//===========================================================
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
		//
		KPLT_END_FM_PLACE_ROBOT_TIME( fms , 0 );
		//===========================================================
	}
//	else if ( Chamber >= BM1 ) { // 2014.11.06
	else { // 2014.11.06
		//===========================================================
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED; // 2008.06.24
		}
		//===========================================================
		//===========================================================
		KPLT_START_FM_PLACE_ROBOT_TIME( fms , 1 );
		//
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_START( -1 , 1 );
		//===========================================================
		//-----------------------------------------------------------------
		// PRE PART
		//-----------------------------------------------------------------
		if ( Slot1 <= 0 ) {
			s1 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		if ( Slot2 <= 0 ) {
			s2 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		//======================================================================================
		// 2005.12.27	// 0 : All Use 1 : FM Not Use 2 : BM Not Use 3 : All NotUse
		//======================================================================================
		BM_RunR = BM_RunF;
		if ( BM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_PREPRECV( MaintInfUse , FM , Chamber ) ) BM_RunR = FALSE; // 2013.04.26
			//
		}
		//
		FM_RunR = FM_RunF;
		if ( FM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_PREPSEND( MaintInfUse , FM , -1 ) ) FM_RunR = FALSE; // 2013.04.26
			//
		}
		//======================================================================================
		if ( BM_RunR ) {
			sprintf( ParamB , "FM_PREPARE_RECV%s %d %d" , MsgAppchar , s1 , s2 );
			if ( FullRun ) strcat( ParamB , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamB );
			Mb = SYS_RUNNING;
		}
		else {
			Mb = SYS_SUCCESS;
		}
		if ( FM_RunR ) {
			//
			if ( Chamber >= BM1 ) { // 2014.11.06
				sprintf( ParamF , "PREPARE_SEND%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
			}
			else { // 2014.11.06
				sprintf( ParamF , "PREPARE_SEND%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
			}
			//
			if ( FullRun ) strcat( ParamF , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
			Mf = SYS_RUNNING;
		}
		else {
			Mf = SYS_SUCCESS;
		}
		//==============================================================================
		// 2005.10.26
		//==============================================================================
		if ( BM_RunR && FM_RunR ) {
//			_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mf = SYS_ABORTED; // 2008.04.03
				Mb = SYS_ABORTED; // 2008.04.03
			}
		}
		else if ( BM_RunR ) {
//			_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ) ) Mb = SYS_ABORTED; // 2008.04.03
		}
		else if ( FM_RunR ) {
//			_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) Mf = SYS_ABORTED; // 2008.04.03
		}
		//==============================================================================
		l_c = 0; // 2008.09.14
		while ( TRUE ) {
			if ( FM_RunR && ( Mf == SYS_RUNNING ) ) {
				Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
			}
			if ( BM_RunR && ( Mb == SYS_RUNNING ) ) {
				Mb = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
			}
			if ( ( Mf != SYS_RUNNING ) && ( Mb != SYS_RUNNING ) ) {
				if ( ( Mf == SYS_ABORTED ) || ( Mb == SYS_ABORTED ) ) {
					EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 1 ); // 2007.12.12
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				else if ( Mf == SYS_ERROR ) { // 2005.11.24
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ERROR;
				}
				else if ( Mb == SYS_ERROR ) { // 2005.11.24
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ERROR;
				}
				else
					break;
			}
//			if ( MANAGER_ABORT() ) return SYS_ABORTED; // 2007.12.14
			// Sleep(1); // 2008.09.14
			if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
			l_c++; // 2008.09.14
		}
		//===========================================================================================
		// 2007.12.14
		//===========================================================================================
//		if ( MANAGER_ABORT() ) { // 2008.06.24
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { // 2008.06.24
			EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 11 ); // 2007.12.12
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		//===========================================================================================
		//-----------------------------------------------------------------
		// ROBOT PART
		//-----------------------------------------------------------------
		if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , Slot1 , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , MaintInfUse , ( Rb_Run == 1 ) ) ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		if ( Rb_Run == 1 ) {
			if ( FM_RunF ) {
				if ( Slot1 <= 0 ) {
					s1 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				if ( Slot2 <= 0 ) {
					s2 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				//
				if ( Chamber >= BM1 ) { // 2014.11.06
					sprintf( ParamF , "PLACE_WAFER%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
				}
				else { // 2014.11.06
					sprintf( ParamF , "PLACE_WAFER%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
				}
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
//				_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
				if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
					Mf = SYS_RUNNING; // 2008.04.03
				}
				else {
					Mf = SYS_ABORTED; // 2008.04.03
				}
			}
			else {
				Mf = SYS_RUNNING; // 2008.04.03
			}
			//
//			Mf = SYS_RUNNING; // 2008.04.03
			while ( TRUE ) {
				//========================
				_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
				//========================
				Mon_Goal = Clock_Goal_Get_Start_Time();
				if ( FM_RunF ) {
					if ( Mf == SYS_RUNNING ) {
						Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
					}
					if ( Mf == SYS_ABORTED ) {
						EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 3 ); // 2007.12.12
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
						return SYS_ABORTED;
					}
					if ( Mf == SYS_ERROR ) { // 2018.06.14
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
						_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
						//
						ROBOT_ANIMATION_FM_RUN_FOR_RESETTING( fms , RB_MODE_PLACE , Chamber , Slot1 , Slot2 );
						//
						return SYS_ERROR;
					}
					if ( Rb_Run == 1 ) {
						Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == RB_PLACEDONE ) {
							if ( Mf != SYS_RUNNING ) break;
							Rb_Run = 0;
						}
					}
					else {
						if ( Mf != SYS_RUNNING ) break;
					}
				}
				else {
					//
					//===============================================================
					//
					if ( Sim_Trg > 0 ) { // 2016.12.09
						if ( Mf == SYS_RUNNING ) {
							if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
								Mf = SYS_SUCCESS;
							}
						}
						if ( Rb_Run == 1 ) {
							Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , Mf == SYS_SUCCESS , &update );
							if ( update ) _i_SCH_IO_MTL_SAVE();
							if ( Mr == RB_PLACEDONE ) {
								if ( Mf != SYS_RUNNING ) break;
								Rb_Run = 0;
							}
						}
						else {
							if ( Mf != SYS_RUNNING ) break;
						}
					}
					//
					//===============================================================
					//
					else {
						if ( ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update ) == RB_PLACEDONE ) {
							if ( update ) _i_SCH_IO_MTL_SAVE();
							break;
						}
						if ( update ) _i_SCH_IO_MTL_SAVE();
					}
					//
				}
				l_c = 0; // 2008.09.14
				do {
//					if ( MANAGER_ABORT() ) return SYS_ABORTED; // 2007.12.14
					//===================================================================
					// 2005.10.26
					//===================================================================
					if ( FM_RunF ) {
						if ( Mf == SYS_RUNNING ) {
							if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
								break;
							}
						}
					}
					//===================================================================
					//
					switch( GET_RUN_LD_CONTROL(0) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
					//===================================================================
					// Sleep(1); // 2008.09.14
					if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
					l_c++; // 2008.09.14
				}
				while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
			}
		}
		else {
			if ( ( Rb_Run == 3 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( MANAGER_ABORT() ) break; // 2007.12.14
					if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 3 , FALSE , &update ) ) break;
				}
			}
			//
			if ( FM_RunF ) {
				if ( Slot1 <= 0 ) {
					s1 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				if ( Slot2 <= 0 ) {
					s2 = 0;
				}
				else {
					if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
					else {
						s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
					}
				}
				//
				if ( Chamber >= BM1 ) { // 2014.11.06
					sprintf( ParamF , "PLACE_WAFER%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
				}
				else { // 2014.11.06
					sprintf( ParamF , "PLACE_WAFER%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
				}
				//
				EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
				//
				switch ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) {
				case SYS_ABORTED :
					EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 3 ); // 2007.12.12
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				case SYS_ERROR :
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
					return SYS_ERROR;
				}
			}
			//
			if ( Rb_Run == 4 ) { // 2015.12.16
				ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_PLACE , Chamber , Slot1 , Slot2 , -1 , MaintInfUse , TRUE );
			}
			//
			//==================================================================================================
			// 2006.11.09
			//==================================================================================================
			//
			if ( ( Rb_Run == 2 ) || ( Rb_Run == 4 ) ) { // 2015.12.16
				while ( TRUE ) {
					if ( MANAGER_ABORT() ) break; // 2007.12.14
					if ( RB_PLACEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 2 , FALSE , &update ) ) break;
				}
			}
			//
			_i_SCH_IO_MTL_SAVE();
			//==================================================================================================
		}
		//===========================================================================================
		// 2007.12.14
		//===========================================================================================
//		if ( MANAGER_ABORT() ) { // 2008.06.24
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { // 2008.06.24
			EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 13 ); // 2007.12.12
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		//===========================================================================================
		//-----------------------------------------------------------------
		// POST PART
		//-----------------------------------------------------------------
		if ( Slot1 <= 0 ) {
			s1 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s1 = Slot1 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		if ( Slot2 <= 0 ) {
			s2 = 0;
		}
		else {
			if ( ( SrcCas < 0 ) || ( SrcCas >= MAX_CASSETTE_SIDE ) ) { // 2006.11.23
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
			else {
				s2 = Slot2 + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_CM( SrcCas , Chamber ) + _i_SCH_PRM_GET_OFFSET_SLOT_FROM_ALL( Chamber );
			}
		}
		//======================================================================================
		// 2005.12.27	// 0 : All Use 1 : FM Not Use 2 : BM Not Use 3 : All NotUse
		//======================================================================================
		BM_RunR = BM_RunF;
		if ( BM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_POSTRECV( MaintInfUse , FM , Chamber ) ) BM_RunR = FALSE; // 2013.04.26
			//
		}
		//
		FM_RunR = FM_RunF;
		if ( FM_RunF ) {
			//
			if ( EQUIP_MESAGE_NOTUSE_POSTSEND( MaintInfUse , FM , -1 ) ) FM_RunR = FALSE; // 2013.04.26
			//
		}
		//======================================================================================
		if ( BM_RunR ) {
			sprintf( ParamB , "FM_POST_RECV%s %d %d" , MsgAppchar , s1 , s2 );
			if ( FullRun ) strcat( ParamB , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamB );
			Mb = SYS_RUNNING;
		}
		else {
			Mb = SYS_SUCCESS;
		}
		if ( FM_RunR ) {
			//
			if ( Chamber >= BM1 ) { // 2014.11.06
				sprintf( ParamF , "POST_SEND%s BM%d %d %d" , MsgAppchar , Chamber - BM1 + 1 , s1 , s2 );
			}
			else { // 2014.11.06
				sprintf( ParamF , "POST_SEND%s PM%d %d %d" , MsgAppchar , Chamber - PM1 + 1 , s1 , s2 );
			}
			//
			if ( FullRun ) strcat( ParamF , " ALL" );
			EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
			Mf = SYS_RUNNING;
		}
		else {
			Mf = SYS_SUCCESS;
		}
		//==============================================================================
		// 2005.10.26
		//==============================================================================
		if ( BM_RunR && FM_RunR ) {
//			_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET2_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB , EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mf = SYS_ABORTED;
				Mb = SYS_ABORTED;
			}
		}
		else if ( BM_RunR ) {
//			_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] , ParamB ) ) Mb = SYS_ABORTED; // 2008.04.03
		}
		else if ( FM_RunR ) {
//			_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( !_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) Mf = SYS_ABORTED; // 2008.04.03
		}
		//==============================================================================
		l_c = 0; // 2008.09.14
		while ( TRUE ) {
			if ( FM_RunR && ( Mf == SYS_RUNNING ) ) {
				Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
			}
			if ( BM_RunR && ( Mb == SYS_RUNNING ) ) {
				Mb = _dREAD_FUNCTION( Address_FUNCTION_INTERFACE[Chamber] );
			}
			if ( ( Mf != SYS_RUNNING ) && ( Mb != SYS_RUNNING ) ) {
				if ( ( Mf == SYS_ABORTED ) || ( Mb == SYS_ABORTED ) ) {
					EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 5 ); // 2007.12.12
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				else if ( ( Mf == SYS_ERROR ) || ( Mb == SYS_ERROR ) ) {
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE );
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE );
					return SYS_ERROR;
				}
				else {
					break;
				}
			}
//			if ( MANAGER_ABORT() ) return SYS_ABORTED; // 2007.12.14
			// Sleep(1); // 2008.09.14
			if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
			l_c++; // 2008.09.14
		}
		//===========================================================================================
		// 2007.12.14
		//===========================================================================================
//		if ( MANAGER_ABORT() ) { // 2008.06.24
		if ( !_i_SCH_SYSTEM_PAUSE_ABORT_CHECK( CHECKING_SIDE ) ) { // 2008.06.24
			EQUIP_INTERFACE_PICKPLACE_FM_CANCEL( 1 , Chamber , Slot1 , Slot2 , CHECKING_SIDE , SrcCas , 15 ); // 2007.12.12
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		//===========================================================================================
		//--
		//===========================================================
		if ( Slot1 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 0 );
		if ( Slot2 > 0 ) _i_SCH_TIMER_SET_ROBOT_TIME_END( -1 , 1 );
		//
		KPLT_END_FM_PLACE_ROBOT_TIME( fms , 1 );
		//===========================================================
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] FMS=%d,Chamber=%d,Slot1=%d,Slot2=%d,FullRun=%d,SrcCas=%d,order=%d\n" , CHECKING_SIDE + 1 , "EQUIP_PLACE_TO_FMBM" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order );
	}
	//======================================================================================================================================================
	//
//	_EQUIP_RUNNING2_TAG[FM] = FALSE; // 2012.03.23 // 2012.10.31
//	_EQUIP_RUNNING2_TAG[Chamber] = FALSE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( Chamber , FALSE ); // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
	//
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Equip Robot FM Move Control
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int EQUIP_ROBOT_FM_MOVE_CONTROL( int fms , int CHECKING_SIDE , int Mode , int Chamber , int Slot , int Slot2 , BOOL MaintInfUse ) {
	int timex = 100;
	int Mf , Mr;
	BOOL FM_Run , Rb_Run;
	char ParamF[64];
	long Mon_Goal;
	BOOL update;
	int l_c;
	char MsgAppchar[2]; /* 2013.04.26 */
	//
	//===============================================================
	//
	long Sim_Goal; // 2016.12.09
	int  Sim_Trg; // 2016.12.09
	//
	//===============================================================
	//

	//================================================================================================
	if ( ( Chamber != F_AL ) && ( Chamber != F_IC ) && ( ( Chamber < CM1 ) || ( Chamber >= TM ) ) ) { // 2014.11.07
		_i_SCH_LOG_LOT_PRINTF( CHECKING_SIDE , "FM* Scheduling Fail in EQUIP_ROBOT_FM_MOVE_CONTROL(fms=%d,Mode=%d,Chamber=%d,Slot1=%d,Slot2=%d,MaintInfUse=%d)\n" , fms , Mode , Chamber , Slot , Slot2 , MaintInfUse );
		return SYS_ABORTED;
	}
	//================================================================================================
	//========================
	_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
	//
//	_EQUIP_RUNNING2_TAG[FM] = TRUE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , TRUE ); // 2012.10.31
	//
	//========================
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_START\t%s\t[%d] FMS=%d,Mode=%d,Chamber=%d,Slot=%d,Slot2=%d\n" , CHECKING_SIDE + 1 , "EQUIP_ROBOT_FM_MOVE_CONTROL" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Mode , Chamber , Slot , Slot2 );
	}
	//======================================================================================================================================================

	if      ( Chamber == F_AL ) Chamber = AL;
	else if ( Chamber == F_IC ) Chamber = IC;

	if ( !_i_SCH_PRM_GET_MODULE_SERVICE_MODE( FM ) || ( EQUIP_ADDRESS_FM(fms) < 0 ) || ( GET_RUN_LD_CONTROL(4) > 0 ) ) {
		FM_Run = FALSE;
		//
		//===============================================================
		//
		Sim_Trg = _SCH_Get_SIM_TIME3( CHECKING_SIDE , FM );
		//
		if ( Sim_Trg > 0 ) {
			Sim_Goal = Clock_Goal_Get_Start_Time();
			timex = Sim_Trg / 20;
			if ( timex <= 0 ) timex = 1;
		}
		//
		//===============================================================
		//
	}
	else {
		Sim_Trg = 0; // 2017.03.24
		FM_Run = TRUE;
	}
	//
	Rb_Run = _i_SCH_PRM_GET_RB_ROBOT_FM_ANIMATION( fms ); // 2006.11.09
	//
	EQUIP_INTERFACE_STRING_APPEND_MESSAGE( MaintInfUse , MsgAppchar ); /* 2013.04.26 */
	//
	switch ( Mode ) {
	case 0 :	sprintf( ParamF , "MOVE_RECV%s %s %d %d" , MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot , Slot2 );	break;
	case 1 :	sprintf( ParamF , "MOVE_SEND%s %s %d %d" , MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot , Slot2 );	break;
	default :	sprintf( ParamF , "ROTATE_WAFER%s %s %d %d" , MsgAppchar , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( Chamber ) , Slot , Slot2 );	break;
	}
	//
	EQUIP_INTERFACE_STRING_APPEND_TRANSFER( CHECKING_SIDE , ParamF );
	//
	if ( !ROBOT_ANIMATION_FM_SET_FOR_READY_DATA( fms , RB_MODE_ROTATE , Chamber , Slot , Slot2 , EQUIP_FM_GET_SYNCH_DATA( fms ) , FALSE , ( Rb_Run == 1 ) ) ) {
		_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
		return SYS_ABORTED;
	}
	if ( Rb_Run == 1 ) {
		if ( FM_Run ) {
//			_dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ); // 2008.04.03
			if ( _dRUN_SET_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) ) { // 2008.04.03
				Mf = SYS_RUNNING;
			}
			else {
				Mf = SYS_ABORTED; // 2008.04.03
			}
		}
		else {
			Mf = SYS_SUCCESS;
		}
		while ( TRUE ) {
			//========================
			_EQUIP_RUNNING_TAG = TRUE; // 2008.04.17
			//========================
			Mon_Goal = Clock_Goal_Get_Start_Time();
			if ( FM_Run && ( Mf == SYS_RUNNING ) ) {
				Mf = _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) );
			}
			if ( FM_Run ) {
				if ( Rb_Run == 1 ) {
					if ( Mf != SYS_RUNNING ) Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , TRUE , &update );
					else                     Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update );
					if ( update ) _i_SCH_IO_MTL_SAVE();
					if ( Mr == RB_ROTATEDONE ) Rb_Run = 0;
				}
				else {
					Mr = RB_ROTATEDONE;
				}
			}
			else {
				//
				//===============================================================
				//
				if ( Sim_Trg > 0 ) { // 2016.12.09
					//
					if ( Clock_Goal_Data_Check( &Sim_Goal , Sim_Trg ) ) {
						Mf = SYS_SUCCESS;
					}
					else {
						Mf = SYS_RUNNING;
					}
					//
					if ( Rb_Run == 1 ) {
						if ( Mf != SYS_RUNNING ) Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , TRUE , &update );
						else                     Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update );
						if ( update ) _i_SCH_IO_MTL_SAVE();
						if ( Mr == RB_ROTATEDONE ) Rb_Run = 0;
					}
					else {
						Mr = RB_ROTATEDONE;
					}
				}
				//
				//===============================================================
				//
				else {
					Mr = ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , 1 , FALSE , &update );
					if ( update ) _i_SCH_IO_MTL_SAVE();
				}
				//
			}
			if ( Mf != SYS_RUNNING ) {
				if ( Mf == SYS_ABORTED ) {
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				else {
					if ( Mr == RB_ROTATEDONE ) break;
				}
			}
			l_c = 0; // 2008.09.14
			do {
				if ( MANAGER_ABORT() ) {
					_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
					return SYS_ABORTED;
				}
				//===================================================================
				// 2005.10.26
				//===================================================================
				if ( FM_Run ) {
					if ( Mf == SYS_RUNNING ) {
						if ( _dREAD_FUNCTION( EQUIP_ADDRESS_FM(fms) ) != SYS_RUNNING ) {
							break;
						}
					}
				}
				//===================================================================
					//
					switch( GET_RUN_LD_CONTROL(0) ) {
					case 1 :
						timex = 0;
						break;
					case 2 :
						timex = 20;
						break;
					}
					//
				//===================================================================
				// Sleep(1); // 2008.09.14
				if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
				l_c++; // 2008.09.14
			}
			while ( ( !Clock_Goal_Data_Check( &Mon_Goal , timex ) ) && ( Rb_Run == 1 ) );
		}
	}
	else if ( FM_Run ) {
		if ( _dRUN_FUNCTION( EQUIP_ADDRESS_FM(fms) , ParamF ) == SYS_ABORTED ) {
			_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
			return SYS_ABORTED;
		}
		//==================================================================================================
		// 2006.11.09
		//==================================================================================================
		while ( TRUE ) {
			if ( RB_ROTATEDONE == ROBOT_ANIMATION_FM_RUN_FOR_NORMAL( fms , Rb_Run , FALSE , &update ) ) break;
		}
		_i_SCH_IO_MTL_SAVE();
		//==================================================================================================
	}
	//======================================================================================================================================================
	// 2005.10.10
	//======================================================================================================================================================
	if ( ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 2 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 4 ) || ( SYSTEM_LOGSKIP_DETAIL_STATUS() == 5 ) ) {
		_IO_EVENT_DATA_LOG_PRINTF( "LOT%d_END\t%s\t[%d] FMS=%d,Mode=%d,Chamber=%d,Slot=%d,Slot2=%d\n" , CHECKING_SIDE + 1 , "EQUIP_ROBOT_FM_MOVE_CONTROL" , KPLT_GET_LOT_PROGRESS_TIME( FM ) , fms , Mode , Chamber , Slot , Slot2 );
	}
	//======================================================================================================================================================
	//
//	_EQUIP_RUNNING2_TAG[FM] = FALSE; // 2012.03.23 // 2012.10.31
	_i_SCH_SYSTEM_EQUIP_RUNNING_SET( FM , FALSE ); // 2012.10.31
	//
	return SYS_SUCCESS;
}



int _i_SCH_EQ_PICK_FROM_FMBM( int fms , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 , BOOL FullRun , int SrcCas , int order ) {
	return EQUIP_PICK_FROM_FMBM( fms , CHECKING_SIDE , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order , FALSE );
}

int _i_SCH_EQ_PLACE_TO_FMBM( int fms , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 , int FullRun , int SrcCas , int order ) {
	return EQUIP_PLACE_TO_FMBM( fms , CHECKING_SIDE , Chamber , Slot1 , Slot2 , FullRun , SrcCas , order , FALSE );
}


int _i_SCH_EQ_ROBOT_FROM_FMBM( int fms , int mode , int CHECKING_SIDE , int Chamber , int Slot1 , int Slot2 ) {
	return EQUIP_ROBOT_FROM_FMBM( fms , mode , CHECKING_SIDE , Chamber , Slot1 , Slot2 , FALSE );
}
