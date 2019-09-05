//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
#include "INF_IO_Part_data.h"
#include "INF_IO_Part_Log.h"
#include "INF_system_tag.h"
#include "INF_iolog.h"
#include "INF_Robot_Handling.h"
#include "INF_sch_CassetteResult.h"
#include "INF_sch_CassetteSupply.h"
#include "INF_sch_prm.h"
#include "INF_sch_prm_cluster.h"
#include "INF_sch_prm_FBTPM.h"
#include "INF_sch_OneSelect.h"
#include "INF_sch_macro.h"
#include "INF_sch_sub.h"
#include "INF_sch_multi_alic.h"
#include "INF_MultiJob.h"
#include "INF_Timer_Handling.h"
#include "INF_User_Parameter.h"
#include "INF_CimSeqnc.h"
//================================================================================
#include "sch_A0_subBM.h"
#include "sch_A0_param.h"
#include "sch_A0_sub.h"
#include "sch_A0_dll.h"
#include "sch_A0_sub_F_sw.h"
//================================================================================
BOOL SCHEDULER_FM_PM_DIRECT_MODE( int side , int pt , BOOL *NoMore ); // 2014.11.25

BOOL SCHEDULER_FM_Current_No_More_Supply( int Side ); // 2014.11.07
BOOL SCHEDULER_FM_Current_No_More_Supply2( int Side ); // 2014.11.07
BOOL SCHEDULER_FM_Another_No_More_Supply( int Side , int option ); // 2014.11.07
BOOL SCHEDULER_FM_Another_No_More_Supply2( int Side , int option , int option2 ); // 2014.11.07
BOOL SCHEDULER_FM_FM_Finish_Status( int Side ); // 2014.11.07

//=======================================================================================================================================
//=======================================================================================================================================
// MIDDLESWAP
//=======================================================================================================================================
//=======================================================================================================================================
void SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( int bmc , BOOL inmode , int *sc , int *lc ) { // 2006.02.22
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( 0 ) == BUFFER_SWITCH_SINGLESWAP ) { // 2017.01.26
		if ( inmode ) {
			switch( _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) ) {
			case 2 :
			case 4 :
			case 6 :
			case 8 :
				*sc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 ) + 1;
				*lc = _SCH_PRM_GET_MODULE_SIZE( bmc );
				break;
			default :
				*sc = 1;
				*lc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 );
				//
				if ( *lc <= 0 ) *lc = 1; // 2011.06.23
				//
				break;
			}
		}
		else {
			switch( _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) ) {
			case 2 :
			case 3 :
			case 6 :
			case 7 :
				*sc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 ) + 1;
				*lc = _SCH_PRM_GET_MODULE_SIZE( bmc );
				break;
			default :
				*sc = 1;
				*lc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 );
				//
				if ( *lc <= 0 ) *lc = 1; // 2011.06.23
				//
				break;
			}
		}
	}
	else {
		if ( inmode ) {
			if ( _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) < 5 ) { // down
				*sc = 1;
				*lc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 );
				//
				if ( *lc <= 0 ) *lc = 1; // 2011.06.23
				//
			}
			else { // up
				*sc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 ) + 1;
				*lc = _SCH_PRM_GET_MODULE_SIZE( bmc );
			}
		}
		else {
			if ( _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) >= 5 ) { // down
				*sc = 1;
				*lc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 );
				//
				if ( *lc <= 0 ) *lc = 1; // 2011.06.23
				//
			}
			else { // up
				*sc = ( _SCH_PRM_GET_MODULE_SIZE( bmc ) / 2 ) + 1;
				*lc = _SCH_PRM_GET_MODULE_SIZE( bmc );
			}
		}
	}
}
	
//==========================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_OUT_MIDDLESWAP( int ch , int *s1 , int *s2 , int orderoption , BOOL dblcheck ) { // 2006.02.22
	int i , k = 0 , sc , lc;
	*s1 = 0;
	*s2 = 0;

	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );

	if ( orderoption == 2 ) {
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					if ( dblcheck ) { // 2009.02.16
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
							if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
								*s2 = *s1;
								*s1 = i;
								k++;
							}
						}
					}
					else {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
			}
			//
			if ( dblcheck ) i++; // 2009.02.16
		}
	}
	else {
		for ( i = lc ; i >= sc ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					if ( dblcheck ) { // 2009.02.16
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i - 1 ) <= 0 ) {
							if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i-1 ) ) { // 2018.04.03
								*s2 = *s1;
								*s1 = i;
								k++;
							}
						}
					}
					else {
						*s2 = *s1;
						*s1 = i;
						k++;
					}
				}
			}
			//
			if ( dblcheck ) i--; // 2009.02.16
		}
	}
	return k;
}

int SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( int ch , int *s , int *p , int *w , int *fs , int orderoption ) { // 2007.05.03
	int i , k = 0 , sc , lc;

	*w = 0;
	*fs = 0;

	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );

	if ( orderoption == 2 ) {
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				*s = _SCH_MODULE_Get_BM_SIDE( ch , i );
				*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
				*w = i;
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				*fs = i;
				k++;
			}
		}
	}
	else {
		for ( i = lc ; i >= sc ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				*s = _SCH_MODULE_Get_BM_SIDE( ch , i );
				*p = _SCH_MODULE_Get_BM_POINTER( ch , i );
				*w = i;
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				*fs = i;
				k++;
			}
		}
	}
	return k;
}


int SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP( int ch , int *s1 , int *s2 , int orderoption ) { // 2006.02.22
	int i , k = 0 , sc , lc;
	*s1 = 0;
	*s2 = 0;

	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );

	if ( orderoption == 2 ) {
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				*s2 = *s1;
				*s1 = i;
				k++;
			}
		}
	}
	else {
		for ( i = lc ; i >= sc ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				*s2 = *s1;
				*s1 = i;
				k++;
			}
		}
	}
	return k;
}

int SCHEDULER_CONTROL_Chk_BM_FREE_SLOT_IN_MIDDLESWAP2( int ch , int *s1 , int *s2 , int orderoption , BOOL swap ) { // 2017.07.11
	int i , k = 0 , sc , lc;
	*s1 = 0;
	*s2 = 0;

	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );

	if ( !swap ) {
		if ( orderoption == 2 ) {
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					*s2 = *s1;
					*s1 = i;
					k++;
				}
			}
		}
		else {
			for ( i = lc ; i >= sc ; i-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					*s2 = *s1;
					*s1 = i;
					k++;
				}
			}
		}
	}
	else {
		if ( orderoption != 2 ) {
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					*s2 = *s1;
					*s1 = i;
					k++;
				}
			}
		}
		else {
			for ( i = lc ; i >= sc ; i-- ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					*s2 = *s1;
					*s1 = i;
					k++;
				}
			}
		}
	}
	return k;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_OUT_MIDDLESWAP( int ch ) {
	int i , sc , lc;
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
	for ( i = sc ; i <= lc ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) return FALSE;
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_CROSS_OUT_MIDDLESWAP_ONLY_PLACE_PICK_PLACE_NOT_PICK( int ch , int ins1 , int ins2 , int *outs1 , int *outs2 ) { // 2009.10.13
	int sc , lc;
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() <= 0 ) return FALSE;
	if ( _SCH_PRM_GET_INTERLOCK_FM_PLACE_DENY_FOR_MDL( ch ) != 3 ) return FALSE;
	//
	if ( ( ins1 <= 0 ) || ( ins2 <= 0 ) ) return FALSE;
	//
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
	//
	if ( ins1 < sc ) {
		*outs1 = ins1 + sc - 1;
	}
	else {
		*outs1 = ins1 - lc;
	}
	//
	if ( _SCH_MODULE_Get_BM_WAFER( ch , *outs1 ) <= 0 ) return FALSE;
	//
	if ( ins2 < sc ) {
		*outs2 = ins2 + sc - 1;
	}
	else {
		*outs2 = ins2 - lc;
	}
	//
	if ( _SCH_MODULE_Get_BM_WAFER( ch , *outs2 ) <= 0 ) *outs2 = 0;
	//
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_FULL_OUT_MIDDLESWAP( int tms , int ch ) {
	int i , sc , lc;

	if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL(tms) > 0 ) { // 2008.07.30 // 2013.03.09
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
					if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + 1 ) ) { // 2018.04.03
							return FALSE;
						}
					}
				}
			}
			i++;
		}
	}
	else {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( int ch ) {
	int i , sc , lc;
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
	for ( i = sc ; i <= lc ; i++ ) {
//		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) return FALSE; // 2016.06.21
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
			return TRUE; // 2016.06.21
		}
	}
//	return TRUE; // 2016.06.21
	return FALSE; // 2016.06.21
}

BOOL SCHEDULER_CONTROL_Chk_BM_OUT_IS_LAST_FOR_MIDDLESWAP( int ch ) {
	int i , sc , lc;
	SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
	for ( i = sc ; i <= lc ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Chk_TM_Free_Status( _SCH_MODULE_Get_BM_SIDE( ch , i ) ) ) return TRUE;
		}
	}
	return FALSE;
}
//=======================================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_MIDDLE_SWAP( int bmc ) { // 2008.03.31
	int c;
	int hasotherbm; // 2014.04.07
	//
	hasotherbm = FALSE; // 2014.04.07
	//
	for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
		if ( c != bmc ) {
			//
			if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) ) {
				//
				hasotherbm = TRUE; // 2014.04.07
				//
//				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_TM_STATION ) ) { // 2009.10.28
					if ( !SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( c ) ) return TRUE;
//				}
			}
			//
		}
	}
	//
	if ( !hasotherbm ) return TRUE; // 2014.04.07
	//
	return FALSE;
}

//=======================================================================================================================================
BOOL SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( int side , int *ch , int addcnt , int willbm , int option ) { // 2013.10.01
	int i , c , s , p , inw , insup , selbm , selinw , selinmode , selinsup;
	int outw , seloutw , selc , ac , intrg;
	int bmcone;
	//
	selbm = -1;
	//
	bmcone = 0; // 2017.03.06
	//
//printf( "-----------------------------------------\n" );
//printf( "-[TEST] [%d][addcnt=%d] [%d]-------------\n" , *ch , addcnt , option );

	for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , c ) != MUF_00_NOTUSE ) ) {
			//
			if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( c , G_PLACE , 0 ) ) continue; // 2018.06.01
			//
			//
			// 2017.03.06
			//
			if      ( bmcone == 0 ) {
				bmcone = c;
			}
			else if ( bmcone == -1 ) {
			}
			else {
				if ( bmcone > 0 ) {
					bmcone = -1;
				}
			}
			//
			//
			if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) {
				intrg = _SCH_PRM_GET_MODULE_SIZE( c );
			}
			else {
				intrg = _SCH_PRM_GET_MODULE_SIZE( c ) / 2;
				//
				if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) intrg = 1; // 2017.01.11
				//
			}
			//
//			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_FM_STATION ) ) { // 2017.03.14
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_FM_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_WAIT_STATION ) ) {
				//
				if ( willbm == c ) {
					inw = addcnt;
				}
				else {
					inw = 0;
				}
				//
				outw = 0;
				ac = 0;
				//
				for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( c ) ; i++ ) {
					//
					if ( _SCH_MODULE_Get_BM_WAFER( c , i ) <= 0 ) continue;
					ac++;
					if ( _SCH_MODULE_Get_BM_STATUS( c , i ) == BUFFER_INWAIT_STATUS ) inw++;
					if ( _SCH_MODULE_Get_BM_STATUS( c , i ) == BUFFER_OUTWAIT_STATUS ) outw++;
					//
				}
				//
				if ( inw >= intrg ) {
					//
					if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) addcnt = 0;
					//
					continue;
				}
				//
				if ( selbm == -1 ) {
					selbm = c;
					selinw = inw;
					selinmode = BUFFER_FM_STATION;
					selinsup = -1;
					seloutw = outw;
					selc = ac;
				}
				else {
					if ( selinmode == BUFFER_TM_STATION ) {
						selbm = c;
						selinw = inw;
						selinmode = BUFFER_FM_STATION;
						selinsup = -1;
						seloutw = outw;
						selc = ac;
					}
					else {
						//
						if ( inw > selinw ) {
							selbm = c;
							selinw = inw;
							selinmode = BUFFER_FM_STATION;
							selinsup = -1;
							seloutw = outw;
							selc = ac;
						}
						else if ( ( inw == selinw ) && ( inw == 0 ) && ( seloutw > 0 ) && ( outw == 0 ) ) {
							selbm = c;
							selinw = inw;
							selinmode = BUFFER_FM_STATION;
							selinsup = -1;
							seloutw = outw;
							selc = ac;
						}
					}
				}
				//
			}
			else if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_TM_STATION ) ) {
				//
				inw = 0;
				insup = -1;
				ac = 0;
				//
				for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( c ) ; i++ ) {
					//
					if ( _SCH_MODULE_Get_BM_WAFER( c , i ) <= 0 ) continue;
					//
					ac++;
					//
					if ( _SCH_MODULE_Get_BM_STATUS( c , i ) == BUFFER_OUTWAIT_STATUS ) inw++;
					//
					if ( _SCH_MODULE_Get_BM_STATUS( c , i ) == BUFFER_INWAIT_STATUS ) {
						//
						s = _SCH_MODULE_Get_BM_SIDE( c , i );
						p = _SCH_MODULE_Get_BM_POINTER( c , i );
						//
						if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) ) {
							if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
								if ( ( insup == -1 ) || ( insup > _SCH_CLUSTER_Get_SupplyID( s , p ) ) ) {
									insup = _SCH_CLUSTER_Get_SupplyID( s , p );
								}
							}
						}
						//
					}
					//
				}
				//
				if ( willbm == c ) { // 2014.01.27
					inw = -1;
				}
				//
				if ( selbm == -1 ) {
					selbm = c;
					selinw = inw;
					selinmode = BUFFER_TM_STATION;
					selinsup = insup;
					selc = ac;
				}
				else {
					if ( selinmode == BUFFER_TM_STATION ) {
						//
						if ( inw > selinw ) {
							selbm = c;
							selinw = inw;
							selinmode = BUFFER_TM_STATION;
							selinsup = insup;
							selc = ac;
						}
						else if ( inw == selinw ) {
							if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) {
								if ( addcnt > 0 ) {
									if ( insup > selinsup ) {
										selbm = c;
										selinw = inw;
										selinmode = BUFFER_TM_STATION;
										selinsup = insup;
										selc = ac;
									}
								}
								else {
									if ( insup < selinsup ) {
										selbm = c;
										selinw = inw;
										selinmode = BUFFER_TM_STATION;
										selinsup = insup;
										selc = ac;
									}
								}
							}
							else {
								if ( insup < selinsup ) {
									selbm = c;
									selinw = inw;
									selinmode = BUFFER_TM_STATION;
									selinsup = insup;
									selc = ac;
								}
							}
						}
					}
				}
				//
			}
		}
	}
	//
	// 2017.03.06
	//
	if ( bmcone > 0 ) {
		*ch = bmcone;
		return TRUE;
	}
	//
	//
	if ( selbm != -1 ) {
		*ch = selbm;
		return ( selc == 0 );
	}
	//
	return FALSE;
//printf( "-[SELECT] [%d] -------------\n" , selbm );
//printf( "-----------------------------------------\n" );
}

//=======================================================================================================================================
BOOL SCHEDULER_CONTROL_Chk_AL2_FREE() { // 2014.05.14
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return FALSE;
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) < 2 ) return FALSE;
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) ) return FALSE;
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return FALSE;
	//
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) return FALSE;
	//
	if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) return FALSE;
	//
	return TRUE;
	//
}

BOOL SCHEDULER_CONTROL_Chk_AL_WILL_PLACE( int bmc , BOOL freew ) { // 2014.03.06
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return FALSE;
	//
//	if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2016.09.21
	if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) { // 2016.09.21
		//
		if ( freew ) {
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return FALSE;
		}
		//
		if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
			if ( bmc == _SCH_MODULE_Get_MFALS_BM( 1 ) ) return TRUE;
		}
		else if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
			if ( bmc == _SCH_MODULE_Get_MFALS_BM( 2 ) ) return TRUE;
		}
		//
	}
	//
	return FALSE;
}




BOOL SCHEDULER_CONTROL_Chk_BM_SINGLE_ONE( int side , int *ch ) { // 2017.03.13
	int c;
	int bmcone;
	//
	bmcone = 0;
	//
	for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , c ) != MUF_00_NOTUSE ) ) {
			//
			if      ( bmcone == 0 ) {
				bmcone = c;
			}
			else if ( bmcone == -1 ) {
			}
			else {
				if ( bmcone > 0 ) {
					bmcone = -1;
				}
			}
		}
		//
	}
	//
	if ( bmcone > 0 ) {
		*ch = bmcone;
		return TRUE;
	}
	//
	return FALSE;
	//
}



int SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( int side , int *ch , int *ch2 , int beforebm , int mode , BOOL allcheck ) {
	int i , k = 0 , sc , lc , c , oc , f , excepbm , all2;
	int sid , csid , sfcheck; // 2011.05.12
	int selfmmode , selinc;
	int singbm;
	int checkact; // 2015.02.17
	//
	//================================================
	*ch2 = 0; // 2006.09.26
	//================================================
	excepbm = 0; // 2014.03.05
	all2 = FALSE; // 2014.03.05
	checkact = FALSE; // 2015.02.17
	//
	if ( _SCH_MODULE_Need_Do_Multi_FAL() ) { // 2008.09.01
		//
		if ( SCHEDULER_CONTROL_Chk_BM_SINGLE_ONE( side , ch ) ) return 1; // 2017.03.13
		//
		if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) {
			//
//			return 1; // 2015.02.17
			//
			// 2015.02.17
			//
			checkact = TRUE;
			//
			all2 = TRUE;
			//
			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
				excepbm = _SCH_MODULE_Get_MFALS_BM( 1 );
			}
			else if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
				excepbm = _SCH_MODULE_Get_MFALS_BM( 2 );
			}
			//
		}
		if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) == 1 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2013.10.01
			//
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) return 1;
			//
			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) { // 2017.01.11
				excepbm = _SCH_MODULE_Get_MFALS_BM( 1 );
			}
			//
		}
		//
		if ( ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) && ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) ) { // 2014.03.05
			//
			all2 = TRUE;
			//
			if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
				excepbm = _SCH_MODULE_Get_MFALS_BM( 1 );
			}
			else if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
				excepbm = _SCH_MODULE_Get_MFALS_BM( 2 );
			}
			//
		}
		//
	}
	//================================================
	//
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2015.02.17
		checkact = TRUE;
	}
	//
	//================================================
//	if ( mode == BUFFER_SWITCH_MIDDLESWAP ) { // 2008.06.21
//	if ( ( mode == BUFFER_SWITCH_MIDDLESWAP ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.06.21 // 2011.05.25
//	if ( ( ( mode == BUFFER_SWITCH_SINGLESWAP ) || ( mode == BUFFER_SWITCH_MIDDLESWAP ) ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2008.06.21 // 2011.05.25 // 2015.02.17
	if ( ( ( mode == BUFFER_SWITCH_SINGLESWAP ) || ( mode == BUFFER_SWITCH_MIDDLESWAP ) ) && checkact ) { // 2008.06.21 // 2011.05.25 // 2015.02.17
		//
		if ( all2 ) { // 2014.03.05
			//
			*ch = 0;
			singbm = -1; // 2014.04.07
			//
			//=========================================================================================
			//
			for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
				//
				if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , c ) != MUF_00_NOTUSE ) ) { // 2011.09.15
					//
					if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( c , G_PLACE , 0 ) ) continue; // 2018.06.01
					//
					//
					if ( singbm == -1 ) { // 2014.04.07
						singbm = c;
					}
					else if ( singbm != 0 ) { // 2014.04.07
						singbm = 0;
					}
					//
					if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_FM_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_WAIT_STATION ) ) {
						//
						if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
							sc = 1;
							lc = _SCH_PRM_GET_MODULE_SIZE( c );
						}
						else {
							SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , TRUE , &sc , &lc ); // in
						}
						//
						f = 0;
						//
						for ( i = lc ; i >= sc ; i-- ) {
							//
							if ( _SCH_MODULE_Get_BM_WAFER( c , i ) <= 0 ) {
								//
								if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( c , i ) ) continue; // 2018.04.03
								//
								f++;
							}
							//
						}
						//
						if ( excepbm == c ) f--;
						if ( beforebm == c ) f--;
						//
						if ( f >= 1 ) {
							//
							if ( beforebm == c ) {
								*ch = c;
								selfmmode = TRUE;
								selinc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS );
								break;
							}
							else {
								if ( excepbm == c ) {
									*ch = c;
									selfmmode = TRUE;
									selinc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS );
									break;
								}
							}
							//
							if ( *ch == 0 ) {
								*ch = c;
								selfmmode = TRUE;
								selinc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS );
							}
							else {
								if ( selfmmode ) {
									if ( selinc < SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS ) ) {
										*ch = c;
										selfmmode = TRUE;
										selinc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS );
									}
								}
								else {
									*ch = c;
									selfmmode = TRUE;
									selinc = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS );
								}
							}
						}
					}
					else {
						//
						if ( mode == BUFFER_SWITCH_FULLSWAP ) {
							sc = 1;
							lc = _SCH_PRM_GET_MODULE_SIZE( c );
						}
						else {
							//
							SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , FALSE , &sc , &lc ); // out
							//
						}
						//
						csid = SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_INWAIT_STATUS );
						//=========================================================================================
						//
						if ( ( beforebm == c ) && ( excepbm == c ) ) {
							//
							if ( ( lc - sc ) > 1 ) {
								//
								*ch = c;
								selfmmode = FALSE;
								selinc = csid;
								break;
								//
							}
							//
						}
						else if ( beforebm == c ) {
							//
							if ( ( lc - sc ) > 0 ) {
								//
								*ch = c;
								selfmmode = FALSE;
								selinc = csid;
								break;
								//
							}
							//
						}
						else if ( ( beforebm <= 0 ) && ( excepbm == c ) ) {
							//
							if ( ( lc - sc ) > 0 ) {
								//
								*ch = c;
								selfmmode = FALSE;
								selinc = csid;
								break;
								//
							}
							//
						}
						else {
							if ( *ch == 0 ) {
								*ch = c;
								selfmmode = FALSE;
								selinc = csid;
							}
							else {
								if ( !selfmmode ) {
									//
									if ( ( selinc > 0 ) && ( csid <= 0 ) ) {
										*ch = c;
										selfmmode = FALSE;
										selinc = csid;
									}
									else if ( ( ( selinc > 0 ) && ( csid > 0 ) ) || ( ( selinc <= 0 ) && ( csid <= 0 ) ) ) {
										if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( *ch , BUFFER_OUTWAIT_STATUS ) < SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_OUTWAIT_STATUS ) ) {
											*ch = c;
											selfmmode = FALSE;
											selinc = csid;
										}
										else if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( *ch , BUFFER_OUTWAIT_STATUS ) == SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( c , BUFFER_OUTWAIT_STATUS ) ) {
											if ( _SCH_TIMER_GET_PROCESS_TIME_START( 1 /* BUFFER_TM_STATION */ , *ch ) > _SCH_TIMER_GET_PROCESS_TIME_START( 1 /* BUFFER_TM_STATION */ , c ) ) {
												*ch = c;
												selfmmode = FALSE;
												selinc = csid;
											}
										}
									}
									//
								}
							}
						}
						//
						//=========================================================================================
					}
				}
			}
			//
			if ( *ch == 0 ) {
				if ( singbm <= 0 ) return -1; // 2014.04.07
//				return -1; // 2014.04.07
				*ch = singbm; // 2014.04.07
			}
			//
			*ch2 = 0;

			return 0;
			//
		}
		else {
			//
			singbm = -1; // 2017.03.10
			//
			sfcheck = 0; // 2011.05.12
			//
			sid = -1; // 2011.05.12
			//=========================================================================================
			if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
				sc = 1;
				lc = _SCH_PRM_GET_MODULE_SIZE( *ch );
			}
			else {
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( *ch , TRUE , &sc , &lc ); // in
			}
			//
			if ( excepbm == *ch ) k = 1; // 2014.03.05
			//
			for ( i = lc ; i >= sc ; i-- ) {
				//
				if ( _SCH_MODULE_Get_BM_WAFER( *ch , i ) > 0 ) {
					//
					if ( _SCH_MODULE_Get_BM_STATUS( *ch , i ) != BUFFER_INWAIT_STATUS ) continue; // 2011.06.23
					//
					k++;
					//
					if ( sid == -1 ) { // 2011.05.12
						sid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( *ch , i ) , _SCH_MODULE_Get_BM_POINTER( *ch , i ) );
					}
					else {
						if ( sid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( *ch , i ) , _SCH_MODULE_Get_BM_POINTER( *ch , i ) ) ) {
							sid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( *ch , i ) , _SCH_MODULE_Get_BM_POINTER( *ch , i ) );
						}
					}
					//
				}
			}
			if ( ( k + 1 ) >= ( lc - sc + 1 ) ) {
				//==============================================================================
				// 2007.04.18
				//==============================================================================
				if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) return 1;
				//==============================================================================
				// 2007.04.16
				//==============================================================================
				oc = 0;
				//
				if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
					sc = 1;
					lc = _SCH_PRM_GET_MODULE_SIZE( *ch );
				}
				else {
					SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( *ch , FALSE , &sc , &lc ); // out
				}
				//
				for ( i = lc ; i >= sc ; i-- ) {
					//
					if ( _SCH_MODULE_Get_BM_WAFER( *ch , i ) > 0 ) {
						//
						if ( _SCH_MODULE_Get_BM_STATUS( *ch , i ) != BUFFER_OUTWAIT_STATUS ) continue; // 2011.06.23
						//
						oc++;
					}
				}
				//==============================================================================
				// 2006.09.26
				//==============================================================================
				if ( allcheck ) {
					//
					for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , c ) != MUF_00_NOTUSE ) ) { // 2017.03.10
							//
							if ( singbm == -1 ) {
								singbm = c;
							}
							else if ( singbm != 0 ) {
								singbm = 0;
							}
							//
						}
						//
						if ( c != *ch ) {
							//
	//						if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) ) { // 2011.09.15
	//						if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) ) { // 2011.09.15
							if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , c ) != MUF_00_NOTUSE ) ) { // 2011.09.15

								//
								if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_FM_STATION ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_WAIT_STATION ) ) {
									//
									if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
										sc = 1;
										lc = _SCH_PRM_GET_MODULE_SIZE( c );
									}
									else {
										SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , TRUE , &sc , &lc ); // in
									}
									//
									f = 0;
									for ( i = lc ; i >= sc ; i-- ) {
										//
										if ( _SCH_MODULE_Get_BM_WAFER( c , i ) <= 0 ) {
											//
											if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( c , i ) ) continue; // 2018.04.03
											//
											f = 1;
											break;
										}
										//
									}
									//
									if ( f == 0 ) { // 2011.06.23
										//
										if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) {
											//
											if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
												sc = 1;
												lc = _SCH_PRM_GET_MODULE_SIZE( c );
											}
											else {
												SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , FALSE , &sc , &lc ); // out
											}
											//
											for ( i = lc ; i >= sc ; i-- ) {
												if ( _SCH_MODULE_Get_BM_WAFER( c , i ) > 0 ) {
													//
													if ( _SCH_MODULE_Get_BM_STATUS( c , i ) != BUFFER_OUTWAIT_STATUS ) continue;
													//
													f = 1;
													//
													break;
												}
											}
											//
										}
									}
									//
									if ( f == 1 ) {
										//===================================================================
										// 2007.04.16
										//===================================================================
										//
										if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
											sc = 1;
											lc = _SCH_PRM_GET_MODULE_SIZE( c );
										}
										else {
											SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , FALSE , &sc , &lc );
										}
										//
										f = 0;
										//
										csid = -1;
										//=========================================================================================
										for ( i = lc ; i >= sc ; i-- ) {
											if ( _SCH_MODULE_Get_BM_WAFER( c , i ) > 0 ) {
												//
												if ( _SCH_MODULE_Get_BM_STATUS( c , i ) != BUFFER_OUTWAIT_STATUS ) continue; // 2011.06.23
												//
												f++;
												//
												if ( csid == -1 ) { // 2011.06.23
													csid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( c , i ) , _SCH_MODULE_Get_BM_POINTER( c , i ) );
												}
												else {
													if ( csid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( c , i ) , _SCH_MODULE_Get_BM_POINTER( c , i ) ) ) {
														csid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( c , i ) , _SCH_MODULE_Get_BM_POINTER( c , i ) );
													}
												}
												//
											}
										}
										//===================================================================
										if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( *ch ) && ( _SCH_MODULE_Get_BM_FULL_MODE( *ch ) == BUFFER_TM_STATION ) ) { // 2011.06.23
											sfcheck = c;
											sid = csid; // 2014.01.27
										}
										else {
											if ( f == oc ) {
												//
												if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) { // 2011.06.23
													if ( sid > csid ) {
														sfcheck = c;
														sid = csid; // 2014.01.27
													}
													else {
														*ch2 = c;
														return 1;
													}
												}
												else {
													*ch2 = c;
													return 0;
												}
												//
											}
											else {
												//===================================================================
												// 2007.04.18
												//===================================================================
												if ( ( oc <= 0 ) && ( f > 0 ) ) {
													*ch2 = c;
													if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) return 1; // 2011.06.23
													return 0;
												}
												else if ( ( oc > 0 ) && ( f <= 0 ) ) {
													k = *ch;
													*ch = c;
													*ch2 = k;
			//										*check = 0; // 2007.05.17
													if ( _SCH_PRM_GET_MODULE_SIZE( c ) == 1 ) return 1; // 2011.06.23
													return 0;
												}
											}
										}
										//===================================================================
									}
								}
								else { // 2011.05.12
									if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( *ch ) && ( _SCH_MODULE_Get_BM_FULL_MODE( *ch ) == BUFFER_TM_STATION ) ) {
										//
										if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
											sc = 1;
											lc = _SCH_PRM_GET_MODULE_SIZE( c );
										}
										else {
											//
											SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , TRUE , &sc , &lc ); // in
										}
										//
										csid = -1;
										//=========================================================================================
										for ( i = lc ; i >= sc ; i-- ) {
											if ( _SCH_MODULE_Get_BM_WAFER( c , i ) > 0 ) {
												//
												if ( csid == -1 ) {
													csid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( c , i ) , _SCH_MODULE_Get_BM_POINTER( c , i ) );
												}
												else {
													if ( csid > _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( c , i ) , _SCH_MODULE_Get_BM_POINTER( c , i ) ) ) {
														csid = _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( c , i ) , _SCH_MODULE_Get_BM_POINTER( c , i ) );
													}
												}
												//
											}
										}
										//=========================================================================================
										//
										if ( sid > csid ) {
											sfcheck = c;
											sid = csid; // 2014.01.27
										}
										//
										//=========================================================================================
									}
								}
							}
						}
					}
					//
					if ( sfcheck != 0 ) { // 2011.05.17
						k = *ch;
						*ch = sfcheck;
						*ch2 = k;
						//
						return 1;
					}
					//
				}
				//==============================================================================
				//
				if ( singbm > 0 ) *ch = singbm; // 2017.03.10
				//
				return 1;
			}
		}
	}
	else {
		if ( _SCH_PRM_GET_MODULE_SIZE( *ch ) == 1 ) return 1; // 2008.07.15
		//
		c = 0; // 2008.02.15
		//
		for ( i = _SCH_PRM_GET_MODULE_SIZE( *ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( *ch , i ) > 0 ) {
				k++;
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( *ch , i ) ) continue; // 2018.04.03
				c++;
			}
		}
		//
		if ( c <= 0 ) return -1; // 2008.02.15
		//
		//
		//
		//
		//
		//
		// 2018.05.08
		//
		// 이부분이 변경되면서 FM이 2장씩 안집고 한장씩만 집는다. 왜 이렇게 했는지 이유 모름
		//
		// 일단 FULLSWAP에서는 원래대로 변경
		//
		//
//		if ( ( k + 1 ) >= _SCH_PRM_GET_MODULE_SIZE( *ch ) ) return 1; // 2013.10.01
//		if ( ( k + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( *ch ) ) return 1; // 2013.10.01 // 2018.05.08
		//
		if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2018.05.08
			if ( ( k + 1 ) >= _SCH_PRM_GET_MODULE_SIZE( *ch ) ) return 1;
		}
		else {
			if ( ( k + 1 ) <= _SCH_PRM_GET_MODULE_SIZE( *ch ) ) return 1;
		}
		//
		//
	}
	return 0;
}

BOOL SCHEDULER_CONTROL_Chk_BM_HAS_OUT_CHECK_SWITCH( int ch , int mode ) { // 2006.09.26
	int i , sc , lc;
	if      ( mode == BUFFER_SWITCH_MIDDLESWAP ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) return TRUE;
		}
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( ch );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int SCHEDULER_CONTROL_Chk_BM_HAS_OTHERSIDE_OUT_GET_SLOT_SWITCH( int mode , int side , int ch ) { // 2016.04.08
	int i , sc , lc;
	int selslot = 0;
	//
	if      ( mode == BUFFER_SWITCH_MIDDLESWAP ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				//
				if ( side != _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
					//
					if ( selslot == 0 ) {
						selslot = i;
					}
					else {
						if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , selslot ) , _SCH_MODULE_Get_BM_POINTER( ch , selslot ) ) ) {
							selslot = i;
						}
					}
					//
				}
				//
			}
		}
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( ch );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
					//
					if ( side != _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
						//
						if ( selslot == 0 ) {
							selslot = i;
						}
						else {
							if ( _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , i ) , _SCH_MODULE_Get_BM_POINTER( ch , i ) ) < _SCH_CLUSTER_Get_SupplyID( _SCH_MODULE_Get_BM_SIDE( ch , selslot ) , _SCH_MODULE_Get_BM_POINTER( ch , selslot ) ) ) {
								selslot = i;
							}
						}
						//
					}
				}
			}
		}
	}
	return selslot;
}

BOOL SCHEDULER_CONTROL_Chk_BM_IN_WILL_CHECK_FULL_SWITCH( int side , BOOL noset , int *rFM_Buffer , int mode ) {
	int b = -1 , c , sc , ec , FM_Buffer , FM_Buffer2;
	//-----------------------------------------------------------------------------------------------
	if ( noset ) {
		sc = *rFM_Buffer;
		ec = *rFM_Buffer + 1;
	}
	else {
		sc = BM1;
		ec = ( BM1 + MAX_BM_CHAMBER_DEPTH );
		//
		if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
			//
			SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_BEFORE_ALIGN_FULLSWAP( side , &c , 0 , 0 , 99 );
			//
			sc = c;
			ec = c + 1;
			//
		}
		else if ( mode == BUFFER_SWITCH_SINGLESWAP ) {
			return TRUE;
		}
		//
	}
	//-----------------------------------------------------------------------------------------------
	for ( c = sc ; c < ec ; c++ ) {
//		if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) ) { // 2011.09.15
		if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) && ( _SCH_MODULE_Get_Mdl_Use_Flag( side , c ) != MUF_00_NOTUSE ) ) { // 2011.09.15
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_FM_STATION ) ) {
				if ( SCHEDULER_CONTROL_Chk_BM_FREE_IN_MIDDLESWAP( c ) ) {
					if ( b == -1 ) b = c;
				}
				else {
					FM_Buffer = c;
//					if ( SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( &FM_Buffer , &FM_Buffer2 , mode , !noset ) ) { // 2008.02.15
//					if ( SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( &FM_Buffer , &FM_Buffer2 , mode , !noset ) != 0 ) { // 2008.02.15 // 2011.09.15
					if ( SCHEDULER_CONTROL_Chk_BM_IN_NEXT_WILL_FULL_SWITCH( side , &FM_Buffer , &FM_Buffer2 , 0 , mode , !noset ) != 0 ) { // 2008.02.15 // 2011.09.15
						if ( !noset ) {
							*rFM_Buffer = FM_Buffer;
						}
						return TRUE;
					}
				}
			}
			else {
			}
		}
	}
	if ( b == -1 ) return FALSE;
	*rFM_Buffer = b;
	return TRUE;
}



BOOL SCHEDULER_CONTROL_Chk_BM_HAS_OUT_WAFER( int mode ) { // 2014.01.27
	int c;
	//-----------------------------------------------------------------------------------------------
	for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) ) {
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( c ) || ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_FM_STATION ) ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_OUT_CHECK_SWITCH( c , mode ) ) return TRUE;
			}
		}
	}
	return FALSE;
}

//=======================================================================================================================================
//=======================================================================================================================================
// BATCH / FULLSWAP
//=======================================================================================================================================
//=======================================================================================================================================

int SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_BATCH_FULLSWAP( int ch , int side , int mode , int *s1 , int orderoption , int *insder , int *inptr , int tmw ) { // 2003.11.07
	int i , k = 0 , m = 0 , sc , lc , mx , offset , overc , oc;
	*s1 = 0;
	//==============================================================
	// 2006.02.02
	//==============================================================
	if ( mode == BUFFER_SWITCH_MIDDLESWAP ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( ch );
	}
	//==============================================================
	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
		if ( ( mode != BUFFER_SWITCH_MIDDLESWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) { // 2007.11.08
			overc = FALSE;
			mx = ( lc - sc + 1 ) / 2;
			if      ( ( ( tmw / 100 ) > 0 ) && ( ( tmw % 100 ) > 0 ) ) {
				offset = 2;
			}
			else if ( ( tmw / 100 ) > 0 ) {
	//			offset = 2; // 2008.01.09
				offset = 1; // 2008.01.09
			}
			else {
				offset = 0;
			}
		}
		else {
			overc = TRUE;
			mx = 1;
			if      ( ( ( tmw / 100 ) > 0 ) && ( ( tmw % 100 ) > 0 ) ) {
				offset = 2;
			}
			else if ( ( tmw / 100 ) > 0 ) {
				offset = 1;
			}
			else {
				offset = 0;
			}
		}
	}
	else if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( mode >= BUFFER_SWITCH_FULLSWAP ) ) { // 2008.07.28
		overc = TRUE;
		mx = 1;
		offset = 2;
	}
	else {
		overc = FALSE;
		mx = 99999;
		offset = 0;
	}
	//==============================================================
	if ( ( orderoption == 2 ) || ( mx != 99999 ) ) { // 2003.06.02
		for ( i = sc ; i <= lc ; i++ ) {
			//
			if ( !overc ) {
				if ( mx < i ) continue; // 2007.11.08
			}
			//
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + ( ( offset == 1 ) ? mx : 0 ) ) <= 0 ) {
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + ( ( offset == 1 ) ? mx : 0 ) ) ) { // 2018.04.03
					if ( offset == 2 ) {
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i + mx ) <= 0 ) {
							if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + mx ) ) { // 2018.04.03
								*s1 = i;
								k++;
								break; // 2008.01.31
							}
						}
					}
					else {
						*s1 = i;
						k++;
						break; // 2008.01.31
					}
				}
			}
			else { // 2003.11.07
				if ( mode != BUFFER_SWITCH_BATCH_ALL ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) != side ) return 0; // 2003.11.14
//					m = 1; // 2003.11.14
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) m = 1;
				}
			}
			if ( overc ) i++;
		}
	}
	else {
		for ( i = lc ; i >= sc ; i-- ) {
			//
			if ( !overc ) {
				if ( mx < i ) continue; // 2007.11.08
			}
			//
	//		if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_READY_STATUS ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + ( ( offset == 1 ) ? mx : 0 ) ) <= 0 ) {
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + ( ( offset == 1 ) ? mx : 0 ) ) ) { // 2018.04.03
					if ( offset == 2 ) {
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i + mx ) <= 0 ) {
							if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i + mx ) ) { // 2018.04.03
								*s1 = i;
								k++;
							}
						}
					}
					else {
						*s1 = i;
						k++;
					}
				}
			}
			else { // 2003.11.07
				if ( mode != BUFFER_SWITCH_BATCH_ALL ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) != side ) return 0; // 2003.11.14
//					m = 1; // 2003.11.14
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) m = 1;
				}
			}
			if ( overc ) i++;
		}
	}
	//==============================================================
	// 2007.01.31
	//==============================================================
	if ( mode == BUFFER_SWITCH_MIDDLESWAP ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( ch );
	}
	//
	*inptr = -1;
	for ( i = sc ; i <= lc ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
					if ( *inptr == -1 ) {
						*insder = side;
						*inptr = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
					else {
						if ( *insder != side ) {
							*insder = side;
							*inptr = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
						else {
							if ( *inptr > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
								*inptr = _SCH_MODULE_Get_BM_POINTER( ch , i );
							}
						}
					}
				}
				else { // 2007.02.07
					if ( *inptr == -1 ) {
						*insder = _SCH_MODULE_Get_BM_SIDE( ch , i );
						*inptr = _SCH_MODULE_Get_BM_POINTER( ch , i );
					}
					else {
						if ( *insder == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
							if ( *inptr > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
								*insder = _SCH_MODULE_Get_BM_SIDE( ch , i );
								*inptr = _SCH_MODULE_Get_BM_POINTER( ch , i );
							}
						}
					}
				}
			}
		}
	}
	//==============================================================
	if ( mode == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
		if ( *inptr == -1 ) {
			if ( k > 0 ) {
				//
				oc = 0;
				for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
						if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) {
							oc++;
							break;
						}
					}
				}
				//
				if ( oc > 0 ) return 99;
				//
			}
		}
	}
	//==============================================================
	if ( ( k > 0 ) && ( m == 1 ) ) return 99;
	return k;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_FULLSWAP( int side , BOOL All , int ch , BOOL middleswap ) { // 2004.03.22
	int i , x = 0 , s = 0 , sc , lc;
	if ( middleswap ) { // 2006.02.22
		if ( All ) {
			for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
				}
			}
		}
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( All ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
					return FALSE;
				}
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) s++;
			}
			else {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
					x++;
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) s++;
				}
			}
		}
		if ( !All && ( x == 0 ) ) {
			//===============================================================================
			// 2007.04.02
			//===============================================================================
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
			for ( i = sc ; i <= lc ; i++ ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
						if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) return TRUE;
					}
				}
			}
			//===============================================================================
			return FALSE;
		}
		if ( s == 0 ) return FALSE;
		return TRUE;
	}
	else {
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( All ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) continue; // 2018.04.03
					return FALSE;
				}
				//
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) == side ) s++;
				//
			}
			else {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) {
						x++; // 2007.11.23
						if ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) == side ) s++; // 2007.11.23
					}
				}
			}
		}
		if ( !All && ( x == 0 ) ) return FALSE;
		if ( s == 0 ) return FALSE;
		return TRUE;
	}
	return FALSE;
}

int  SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCHALL( int side , int ch , int cnt , BOOL mdnck ) { // 2007.01.24
	int i , j = 0;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( !mdnck ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) {
					return FALSE;
				}
			}
			j++;
		}
	}
	if ( j >= cnt ) return TRUE;
	return FALSE;
}

int SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP1( int side , int ch , BOOL middleswap , BOOL dblcheck , int tmside , int otherarm ) {
	int i , j , sc , lc , pd , nc;
	if ( middleswap ) { // 2006.02.22
		//
		// 2011.10.20
		nc = FALSE;
		//
		if ( _SCH_PRM_GET_EIL_INTERFACE() <= 0 ) {
			if ( dblcheck && ( tmside != -1 ) && ( otherarm != -1 ) ) { // 2009.08.18
				if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) {
					if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( tmside ) % 10 ) != 0 ) {
						nc = TRUE;
					}
				}
			}
		}
		else { // 2011.10.20
//			if ( ( tmside != -1 ) && ( otherarm != -1 ) ) { // 2009.08.18 // 2012.11.20
			if ( dblcheck && ( tmside != -1 ) && ( otherarm != -1 ) ) { // 2009.08.18
				nc = TRUE;
			}
		}
		//
		if ( nc ) {
			//
			if ( _SCH_MODULE_Get_TM_WAFER( tmside , otherarm ) > 0 ) {
				//
				sc = _SCH_MODULE_Get_TM_SIDE( tmside , otherarm );
				lc = _SCH_MODULE_Get_TM_POINTER( tmside , otherarm );
				//
				if ( _SCH_CLUSTER_Get_PathRange( sc , lc ) == 1 ) { // 2012.11.20
					//
					pd = _SCH_CLUSTER_Get_PathDo( sc , lc );
					//
					if ( pd <= 1 ) {
						// 2009.09.21
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							pd = _SCH_CLUSTER_Get_PathProcessChamber( sc , lc , 0 , j );
							if ( pd > 0 ) {
								if ( _SCH_MODULE_GET_USE_ENABLE( pd , FALSE , sc ) ) {
//									if ( _SCH_MODULE_Get_PM_WAFER( pd , 0 ) <= 0 ) { // 2014.01.10
									if ( SCH_PM_IS_ABSENT( pd , sc , lc , 0 ) ) { // 2014.01.10
										if ( _SCH_MACRO_CHECK_PROCESSING_INFO( pd ) <= 0 ) break;
									}
								}
							}
						}
						//
						if ( j >= MAX_PM_CHAMBER_DEPTH ) { // 2009.09.21
							//
							j = 0;
							//
							SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
							for ( i = sc ; i <= lc ; i++ ) {
								if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
									if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
										if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
											if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
												j++;
												break;
											}
										}
									}
								}
								i++;
							}
							//
							if ( j == 0 ) {
								//
								SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
								for ( i = sc ; i <= lc ; i++ ) {
									if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
										if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
											if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
												if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) { // 2018.04.03
													j++;
													break;
												}
											}
										}
									}
									i++;
								}
								//
								if ( j == 0 ) return 99;
							}
							//
						}
					}
				}
			}
		}
		//
		j = 0;
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) {
					//===========================================================================================
					// 2007.09.06
					//===========================================================================================
//					sc = _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ); // 2015.12.01
//					lc = _SCH_MODULE_Get_BM_POINTER( ch , i + 1 ); // 2015.12.01
//					if ( _SCH_CLUSTER_Get_PathDo( sc , lc ) == PATHDO_WAFER_RETURN ) { // 2015.12.01
//						_SCH_MODULE_Set_BM_STATUS( ch , i + 1 , BUFFER_OUTWAIT_STATUS ); // 2015.09.02
//					} // 2015.12.01
//					else { // 2015.12.01
					//===========================================================================================
						return -1;
					//===========================================================================================
//					} // 2015.12.01
					//===========================================================================================
				}
			}
		}
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				j++;
			}
		}
		if ( j == ( lc - sc + 1 ) ) return 0;
		if ( j == 0 ) return 99;
		if ( dblcheck ) { // 2007.04.18
			if ( j == 1 ) return 99;
		}
		return 1;
	}
	else {
		j = 0;
		//
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) {
					//===========================================================================================
					// 2007.09.06
					//===========================================================================================
					sc = _SCH_MODULE_Get_BM_SIDE( ch , i + 1 );
					lc = _SCH_MODULE_Get_BM_POINTER( ch , i + 1 );
					if ( _SCH_CLUSTER_Get_PathDo( sc , lc ) == PATHDO_WAFER_RETURN ) {
						_SCH_MODULE_Set_BM_STATUS( ch , i + 1 , BUFFER_OUTWAIT_STATUS );
					}
					else {
					//===========================================================================================
						return -1;
					//===========================================================================================
					}
					//===========================================================================================
				}
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) continue; // 2018.04.03
				j++;
			}
		}
		if ( j == _SCH_PRM_GET_MODULE_SIZE( ch ) ) return 0;
		if ( j == 0 ) return 99;
		if ( dblcheck ) { // 2007.04.18
			if ( j == 1 ) return 99;
		}
		return 1;
	}
	return -1;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP2( int side , int ch , BOOL *k ) {
	int i , j = FALSE;
	*k = FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) == side ) ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) {
					j = TRUE;
				}
				else {
					*k = TRUE;
				}
			}
		}
	}
	return j;
}

void SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_RETURN_BATCH_ALL( int side , int ch ) { // 2006.08.02
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			_SCH_CLUSTER_Set_PathRange( side , _SCH_MODULE_Get_BM_POINTER( ch , i + 1 ) , 0 );
		}
	}
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_CROSS_OK_OUT_BATCH_ALL( int side , int ch ) { // 2006.08.04
	int i , c = 0;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			c = 1;
			break;
		}
	}
	if ( c == 0 ) return TRUE;
	if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) != side ) return TRUE;
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_BATCH_FULLSWAP3( int side , int mode , BOOL All , int ch ) {
	int i , x = 0 , sc , lc , sy , sx;
	if ( mode == 0 ) { // batch
		if ( _SCH_MODULE_Get_BM_SIDE( ch , 1 ) != side ) return FALSE;
	}
	if ( mode == 2 ) { // Middleswap 2006.02.22
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		sx = FALSE;
		sy = FALSE;
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				x++;
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) != side ) {
					sx = TRUE;
				}
				else {
					sy = TRUE;
				}
			}
		}
		if ( x == 0 ) return FALSE;
		if ( sx && !sy ) return FALSE;
		return TRUE;
	}
	else {
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( All ) {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
					if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) continue; // 2018.04.03
					return FALSE;
				}
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
				if ( mode != 0 ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) != side ) return FALSE;
				}
			}
			else {
				if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
					x++;
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
					if ( mode != 0 ) {
						if ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) != side ) return FALSE;
					}
				}
			}
		}
		if ( !All && ( x == 0 ) ) return FALSE;
		return TRUE;
	}
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP( int side , int ch , BOOL middleswap , BOOL fullswapfreeonly ) {
	int i , j = FALSE , sc , lc;

	if ( middleswap ) { // 2006.02.22
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_INWAIT_STATUS ) return FALSE;
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				j = TRUE;
			}
		}
	}
	else {
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				if ( !fullswapfreeonly ) { // 2007.11.23
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) != BUFFER_INWAIT_STATUS ) return FALSE;
				}
			}
			else {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				j = TRUE;
			}
		}
	}
	return j;

}

//BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( int ch , int oldch ) { // 2018.08.22
BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_FULLSWAP( int ch , int oldch , BOOL othercheck ) { // 2018.08.22
	int i , cio , cin;
	int coo , con; // 2013.10.01

	if ( othercheck ) { // 2018.08.22
		//
		cio = 0;
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch )  ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				cio++;
				break;
			}
		}
		//
		if ( cio == 0 ) return FALSE;
	}
	//
	if ( oldch <= 0 ) return TRUE;
	if ( ch == oldch ) return TRUE; // 2007.11.23

	cio = 0;
	coo = 0; // 2013.10.01
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( oldch )  ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( oldch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( oldch , i ) == BUFFER_INWAIT_STATUS ) {
				cio++;
			}
			else if ( _SCH_MODULE_Get_BM_STATUS( oldch , i ) == BUFFER_OUTWAIT_STATUS ) { // 2013.10.01
				coo++;
			}
		}
	}
	cin = 0;
	con = 0; // 2013.10.01
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch )  ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
				cin++;
			}
			else if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_OUTWAIT_STATUS ) { // 2013.10.01
				con++;
			}
		}
	}
	if ( cio < cin ) return TRUE;
	//
	if ( ( cio == 0 ) && ( cin == 0 ) && ( coo > 0 ) && ( con == 0 ) ) return TRUE; // 2013.10.01
	//
	return FALSE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_BATCH_FULLSWAP( int ch , BOOL middleswap , BOOL outcheck ) {
	int i , sc , lc;
	if ( middleswap ) { // 2006.02.22
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
				if ( outcheck ) { // 2007.11.23
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_INWAIT_STATUS ) return FALSE;
				}
			}
		}
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
				return FALSE;
			}
		}
	}
	else {
		for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( outcheck ) { // 2007.11.23
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_INWAIT_STATUS ) return FALSE;
			}
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) <= 0 ) {
				if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) continue; // 2018.04.03
				return FALSE;
			}
		}
	}
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_OR_HAS_OUT_MIDDLESWAP( int ch , int md ) {
	int i , sc , lc , ci , co;
	if ( md == -1 ) {
		return 0;
	}
	else {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc );
		ci = 0;
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) ci++;
		}
		if ( ci >= ( lc - sc + 1 ) ) return 1;
		if ( ci == 0 ) return 0;
		//
		co = 0;
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc );
		for ( i = sc ; i <= lc ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) co++;
		}
		if ( co > ci ) return 2;
	}
	return 0;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_FULLSWAP_AL_FREE_SWAP_OK( int side , int ch , BOOL middleswap ) { // 2013.10.01
	//
	if ( middleswap ) return FALSE;
	//
	if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return FALSE;
	//
	if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) return FALSE;
	//
	if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) || ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) ) return FALSE;
	//
	return TRUE;
}

int SCHEDULER_CONTROL_Chk_BM_FIRST_CHECK_MIDDLESWAP( int ch , int mode , int *rs , int *rp , BOOL midswap ) { // 2006.12.22
	int i , sc , lc , cim , ci , com , co;
	int cos , cop;
	int ICsts , ICsts2; // 2012.01.21

	cos = -1;
	cop = -1;

	if ( _SCH_MODULE_Get_BM_FULL_MODE( ch ) != BUFFER_FM_STATION ) return FALSE;

	if ( midswap ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc ); // in
		ci = 0;
		cim = 0;
		for ( i = sc ; i <= lc ; i++ ) {
			cim++;
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) ci++;
		}
		//
		co = 0;
		com = 0;
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , FALSE , &sc , &lc ); // out
		for ( i = sc ; i <= lc ; i++ ) {
			com++;
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				cos = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2007.04.11
				cop = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2007.04.11
				//
				if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2012.01.21
					if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( cos , cop , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) > 0 ) {
						co++;
					}
				}
				else {
					co++;
				}
				//
			}
		}
	}
	else { // 2007.11.30
		ci = 0;
		cim = 0;
		//
		co = 0;
		com = 0;
		//
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
					cim++;
					ci++;
				}
				else {
					com++;
					cos = _SCH_MODULE_Get_BM_SIDE( ch , i ); // 2007.04.11
					cop = _SCH_MODULE_Get_BM_POINTER( ch , i ); // 2007.04.11
					//
					if ( _SCH_MODULE_Need_Do_Multi_FIC() ) { // 2012.01.21
						if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( cos , cop , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) > 0 ) {
							co++;
						}
					}
					else {
						co++;
					}
					//
				}
			}
		}
	}
	//
	if ( mode == 0 ) {
		if ( midswap ) { // 2007.11.30
			if ( ( ci == cim ) && ( co == 1 ) ) {
				*rs = cos;
				*rp = cop;
				return TRUE;
			}
		}
		//
		if ( ( co > 0 ) && ( ci > 0 ) ) {
			*rs = cos;
			*rp = cop;
			return TRUE;
		}
	}
	else { // 2007.04.11
		if ( midswap ) { // 2007.11.30
			if ( ( ci == 0 ) && ( co > 0 ) ) {
				*rs = cos;
				*rp = cop;
				return TRUE;
			}
		}
	}
	//
	return FALSE;
}


//=======================================================================================================================================
BOOL SCHEDULER_CONTROL_Chk_BM_IN_HAS_SPACE_FULL_SWITCH( int ch , int mode ) { // 2014.01.23
	int i , sc , lc;
	//
	if ( ( mode == BUFFER_SWITCH_SINGLESWAP ) || ( mode == BUFFER_SWITCH_MIDDLESWAP ) ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( ch , TRUE , &sc , &lc ); // in
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( ch );
	}
	//
	for ( i = lc ; i >= sc ; i-- ) {
		//
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) continue; // 2018.04.03
			return TRUE;
		}
		//
	}
	//
	return FALSE;
}

//=======================================================================================================================================
//=======================================================================================================================================
// SINGLESWAP
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================

BOOL SCHEDULER_CONTROL_Chk_BM_FREE_IN_SINGLESWAP( int ch ) {
	int i;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_INWAIT_STATUS ) {
				return FALSE;
			}
		}
	}
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_BM_OTHER_WAIT_FOR_TM_SUPPLY_for_SINGLE_SWAP( int bmc ) { // 2011.04.28
	int c;
	//
	// 2016.06.28
	//
	int nc = 0 , s = -1;
	//
	if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
		s = _SCH_MODULE_Get_MFAL_SIDE();
	}
	if ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) {
		s = _SCH_MODULE_Get_FM_SIDE( TA_STATION );
	}
	if ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) {
		s = _SCH_MODULE_Get_FM_SIDE( TB_STATION );
	}
	//
	for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
		if ( c != bmc ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( c ) == 0 ) ) {
				//
				if ( s >= 0 ) { // 2016.06.28
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , c ) == MUF_00_NOTUSE ) continue;
					//
					nc++;
					//
				}
				//
				if ( !SCHEDULER_CONTROL_Chk_BM_FREE_IN_SINGLESWAP( c ) ) return TRUE;
				//
			}
		}
	}
	//
	if ( s >= 0 ) { // 2016.06.28
		if ( nc == 0 ) return TRUE;
	}
	//
	return FALSE;
}




int SCHEDULER_CONTROL_Chk_BM_HAS_OUT_FREE_SIDE_SINGLESWAP( int ch , int side , int *s1 , int orderoption , int *ipsdr , int *ipt , BOOL dblcheck ) { // 2003.11.07 // 2016.12.09
	int i , k = 0 , j = 0 , l = 0 , m = 0;
	*ipt = -1; // 2007.01.12
	*s1 = 0;
	if ( orderoption == 2 ) {
		for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					//
					if ( dblcheck ) { // 2016.12.09
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i+1 ) > 0 ) {
							//
							i++;
							//
							m++;
							if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
								if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
									l++;
									//==============================================
									// 2007.01.12
									//==============================================
									if ( *ipt == -1 ) {
										*ipsdr = side;
										*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
									}
									else {
										if ( *ipsdr != side ) {
											*ipsdr = side;
											*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
										}
										else {
											if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
												*ipsdr = side;
												*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
											}
										}
									}
									//==============================================
								}
								else { // 2007.02.07
									if ( *ipt == -1 ) {
										*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
										*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
									}
									else {
										if ( *ipsdr == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
											if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
												*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
												*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
											}
										}
									}
								}
								j++;
							}
							//
							continue;
							//
						}
						//
						//
					}
					//
					*s1 = i;
					k++;
					j++;
				}
			}
			else {
				m++;
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
						l++;
						//==============================================
						// 2007.01.12
						//==============================================
						if ( *ipt == -1 ) {
							*ipsdr = side;
							*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
						else {
							if ( *ipsdr != side ) {
								*ipsdr = side;
								*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
							}
							else {
								if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
									*ipsdr = side;
									*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
								}
							}
						}
						//==============================================
					}
					else { // 2007.02.07
						if ( *ipt == -1 ) {
							*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
							*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
						else {
							if ( *ipsdr == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
								if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
									*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
								}
							}
						}
					}
					j++;
				}
			}
			//
			if ( dblcheck ) i++; // 2016.12.09
			//
		}
	}
	else {
		for ( i = _SCH_PRM_GET_MODULE_SIZE( ch ) ; i >= 1 ; i-- ) {
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i ) <= 0 ) {
				//
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i ) ) { // 2018.04.03
					//
					if ( dblcheck ) { // 2016.12.09
						if ( _SCH_MODULE_Get_BM_WAFER( ch , i-1 ) > 0 ) {
							//
							i--;
							//
							m++;
							if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
								if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
									l++;
									//==============================================
									// 2007.01.12
									//==============================================
									if ( *ipt == -1 ) {
										*ipsdr = side;
										*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
									}
									else {
										if ( *ipsdr != side ) {
											*ipsdr = side;
											*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
										}
										else {
											if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
												*ipsdr = side;
												*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
											}
										}
									}
									//==============================================
								}
								else { // 2007.02.07
									if ( *ipt == -1 ) {
										*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
										*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
									}
									else {
										if ( *ipsdr == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
											if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
												*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
												*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
											}
										}
									}
								}
								j++;
							}
							//
							continue;
							//
						}
					}
					//
					//
					*s1 = i;
					k++;
					j++;
				}
			}
			else {
				m++;
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i ) == BUFFER_INWAIT_STATUS ) {
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i ) == side ) {
						l++;
						//==============================================
						// 2007.01.12
						//==============================================
						if ( *ipt == -1 ) {
							*ipsdr = side;
							*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
						else {
							if ( *ipsdr != side ) {
								*ipsdr = side;
								*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
							}
							else {
								if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
									*ipsdr = side;
									*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
								}
							}
						}
						//==============================================
					}
					else { // 2007.02.07
						if ( *ipt == -1 ) {
							*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
							*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
						}
						else {
							if ( *ipsdr == _SCH_MODULE_Get_BM_SIDE( ch , i ) ) {
								if ( *ipt > _SCH_MODULE_Get_BM_POINTER( ch , i ) ) {
									*ipsdr = _SCH_MODULE_Get_BM_SIDE( ch , i );
									*ipt = _SCH_MODULE_Get_BM_POINTER( ch , i );
								}
							}
						}
					}
					j++;
				}
			}
			//
			if ( dblcheck ) i--; // 2016.12.09
			//
		}
	}
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( ch ) == 1 ) { // 2007.10.04
		if ( k > 0 ) {
			if ( m == 0 ) return 99;
			if ( l != 0 ) return 98;
			return 1;
		}
	}
	else {
		if ( ( j >= 2 ) && ( k > 0 ) ) {
			if ( m == 0 ) return 99;
			if ( l != 0 ) return 98;
			return 1;
		}
	}
	return 0;
}

int SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP1( int side , int ch ) { // 2004.11.29
	int i , r = 0 , n = 0;
	// 0 - nothing
	// 1 - here
	// 2 - other
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) {
				if  ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) == side ) r = 1;
				else                                                        r = 2; 
			}
			else {
				n++;
			}
		}
		else {
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) continue; // 2018.04.03
			return 0;
		}
	}
	if ( n > 0 ) return r;
	return 0;
}

int SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP2( int side , int ch , BOOL pickmode , int tmside ) {
	int i , j = 0;

	if ( pickmode ) { // 2009.10.30
		if ( !_SCH_ROBOT_GET_FINGER_HW_USABLE( tmside,TA_STATION ) || !_SCH_ROBOT_GET_FINGER_HW_USABLE( tmside,TB_STATION ) ) {
			if ( _SCH_PRM_GET_MODULE_SIZE( ch ) <= 1 ) {
				if ( !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) || !_SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
					return -1;
				}
			}
		}
	}
	//
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) {
				return -1;
			}
		}
		else {
			j++;
		}
	}
	if ( j == _SCH_PRM_GET_MODULE_SIZE( ch ) ) return 0;
	if ( j == 1 ) return 1;
	return 99;
}



BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_OUT_SINGLESWAP3( int side , int ch , BOOL *k ) {
	int i , j = FALSE;
	*k = FALSE;
	for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( ch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			if ( ( side == -1 ) || ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) == side ) ) {
				if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) == BUFFER_OUTWAIT_STATUS ) {
					j = TRUE;
				}
				else {
					*k = TRUE;
				}
			}
		}
	}
	return j;
}

BOOL SCHEDULER_CONTROL_Chk_BM_FULL_ALL_FOR_IN_SINGLSWAP( int side , int ch , int *k ) {
	int i , j = FALSE , m = 0 , c , ho;
	//
	ho = 0; // 2011.06.24
	c = _SCH_PRM_GET_MODULE_SIZE( ch );
	//
	*k = -2;
	for ( i = 0 ; i < c ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( ch , i + 1 ) > 0 ) {
			m++;
			if ( _SCH_MODULE_Get_BM_STATUS( ch , i + 1 ) != BUFFER_INWAIT_STATUS ) {
				//
				ho++; // 2011.06.24
				//
				if ( _SCH_MODULE_Get_BM_SIDE( ch , i + 1 ) != side ) {
					*k = _SCH_MODULE_Get_BM_SIDE( ch , i + 1 );
				}
			}
		}
		else {
			if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( ch , i+1 ) ) continue; // 2018.04.03
			j = TRUE;
		}
	}
	//
	if ( ( !j ) && ( c == 1 ) && ( ho > 0 ) ) {
		*k = -2; // 2011.08.09
		j = TRUE; // 2011.06.24
	}
	//
	if ( ( *k == -2 ) && ( m == 0 ) ) *k = -1;
	return j;
}


//=======================================================================================================================================
//=======================================================================================================================================
int PM_LOCK_DENY_CONTROL;
int PM_LOCK_DENY_MODULE[MAX_PM_CHAMBER_DEPTH];

void SCHEDULING_SET_FEM_Pick_Deny_PM_LOCK_DENY_CONTROL( int data ) {
	PM_LOCK_DENY_CONTROL = data;
}

//0 = supply
//1 = not supply
//2 = maintain
/* // 2004.11.26
BOOL SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( int supplyside , int nomoreside , int pointer ) { // 2004.11.05
	int mch[MAX_CHAMBER] , i , j , m , nmside , p;
	nmside = -1;
	if ( nomoreside == -1 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i != supplyside ) {
				if ( _SCH_SYSTEM_USING_GET( i ) >= 9 ) {
					if ( ( _SCH_MODULE_Get_TM_OutCount( i ) - _SCH_MODULE_Get_TM_InCount( i ) ) == 1 ) {
						if ( _SCH_SYSTEM_MODE_END_GET( i ) != 0 ) {
							nmside = i;
							break;
						}
						else {
							if ( !_SCH_SUB_Remain_for_FM( i ) ) {
								nmside = i;
								break;
							}
						}
					}
				}
			}
		}
	}
	else {
		if ( nomoreside != supplyside ) {
			if ( _SCH_SYSTEM_USING_GET( nomoreside ) >= 9 ) {
				if ( ( _SCH_MODULE_Get_TM_OutCount( nomoreside ) - _SCH_MODULE_Get_TM_InCount( nomoreside ) ) == 1 ) {
					if ( _SCH_SYSTEM_MODE_END_GET( nomoreside ) != 0 ) {
						nmside = nomoreside;
					}
					else {
						if ( !_SCH_SUB_Remain_for_FM( nomoreside ) ) {
							nmside = nomoreside;
						}
					}
				}
			}
		}
	}

	if ( nmside == -1 ) return FALSE;
	
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) mch[i] = 0;

	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _SCH_CLUSTER_Get_PathRange( nmside , i ) >= 0 ) {
			if ( ( _SCH_CLUSTER_Get_PathStatus( nmside , i ) >= SCHEDULER_SUPPLY ) && ( _SCH_CLUSTER_Get_PathStatus( nmside , i ) <= SCHEDULER_WAITING ) ) {
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					m = _SCH_CLUSTER_Get_PathProcessChamber( nmside , i , 0 , j );
					if ( m > 0 ) mch[m] = 1;
				}
			}
		}
	}
	if ( pointer == -1 ) {
		if ( _SCH_SUB_Remain_for_FM( supplyside ) ) {
			p = _SCH_MODULE_Get_FM_DoPointer( supplyside );
		}
		else {
			return FALSE;
		}
	}
	else {
		p = pointer;
	}
	for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( supplyside , p , 0 , j );
		if ( m > 0 ) {
			if ( mch[m] == 1 ) return FALSE;
		}
	}
	return TRUE;
}
*/

int SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( int side , int *rcm , int *rpt , int mode ); // 2010.09.28


int SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap_Sub( int supplyside , int pointer , BOOL check ) { // 2004.11.05
	int p , j , m;
	int rcm , rpt;
	int bc; // 2016.10.13
	//----------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------
	if ( !_SCH_SUB_Check_Complete_Return( supplyside ) ) return 1; // 2004.12.03
	//----------------------------------------------------------------------------------------------------------

//	if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( supplyside , &rcm , &rpt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) == 2 ) return 2; // 2010.09.28 // 2018.12.20
	if ( SCHEDULER_CONTROL_POSSIBLE_PICK_FROM_FM_DETAIL( supplyside , &rcm , &rpt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1001 : 1010 ) == 2 ) return 2; // 2010.09.28 // 2018.12.20


	//----------------------------------------------------------------------------------------------------------
	// 2008.06.27
	//----------------------------------------------------------------------------------------------------------
	if ( Get_Prm_MODULE_BUFFER_SWITCH_MODE() == 2 ) return -2;
	//----------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------

//	if ( _SCH_MODULE_Get_Mdl_Use_Flag( supplyside , TM + 1 ) == MUF_01_USE ) return -3; // 2014.01.10 // 2014.02.04

	//----------------------------------------------------------------------------------------------------------
	// 2005.12.12
	//----------------------------------------------------------------------------------------------------------
	if ( check ) {
		p = TRUE;
		//
		bc = 0; // 2016.10.13
		//
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			//
			if ( SCHEDULING_ThisIs_BM_OtherChamber( j , 0 ) ) continue; // 2016.10.13
			//
//			if ( _SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) ) { // 2007.05.28
			if ( _SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( j ) == 0 ) ) { // 2007.05.28
				//
				bc++; // 2016.10.13
				//
				if      ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( j ) || ( _SCH_MODULE_Get_BM_FULL_MODE( j ) == BUFFER_TM_STATION ) ) { // pump
					p = FALSE;
					break;
				}
				else if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( j ) || ( _SCH_MODULE_Get_BM_FULL_MODE( j ) == BUFFER_FM_STATION  ) ) { // vent
					for ( m = 0 ; m < _SCH_PRM_GET_MODULE_SIZE( j ) ; m++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( j , m + 1 ) > 0 ) {
							p = FALSE;
							break;
						}
					}
				}
			}
		}
		//
		if ( p && ( bc <= 1 ) ) p = FALSE; // 2016.10.13
		//
		if ( p ) {
//printf( "[C]==================================================================\n" );
//printf( "[C]===>REJECT LOCK DENY FROM BM STATUS\n" );
//printf( "[C]===>REJECT LOCK DENY FROM BM STATUS\n" );
//printf( "[C]===>REJECT LOCK DENY FROM BM STATUS\n" );
//printf( "[C]==================================================================\n" );
			return -3;
		}
	}
	//----------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------
	if ( PM_LOCK_DENY_CONTROL ) {
		if ( pointer == -1 ) {
			if ( _SCH_SUB_Remain_for_FM( supplyside ) ) {
				p = _SCH_MODULE_Get_FM_DoPointer( supplyside );
			}
			else {
				return -4;
			}
		}
		else {
			p = pointer;
		}
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( supplyside , p , 0 , j );
			if ( m > 0 ) {
				//
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) == -1 ) continue; // 2016.11.17
				if ( _SCH_PRM_GET_MODULE_GROUP( m ) != 0 ) return -6; // 2017.03.07
				//
//				if ( PM_LOCK_DENY_MODULE[m-PM1] == 1 ) return FALSE; // 2009.10.05
				if ( PM_LOCK_DENY_MODULE[m-PM1] != 0 ) return -5; // 2009.10.05
			}
		}
//printf( "[T]==================================================================\n" );
//printf( "[T]===>REJECT LOCK DENY [%d,%d][%d]\n" , supplyside , p , pointer );
//printf( "[T]==================================================================\n" );
		return 2;
	}
	return -1;
}

BOOL SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap( int supplyside , int pointer , BOOL check ) { // 2009.10.05
	int Res;
	//
	EnterCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
	//
	Res = SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap_Sub( supplyside , pointer , check );
	//
	LeaveCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
	//
//printf( "==================================================================\n" );
//printf( "===> LOCK DENY [%d][%d]\n" , PM_LOCK_DENY_CONTROL , Res );
//printf( "==================================================================\n" );

	if ( Res > 0 ) return TRUE;
	return FALSE;
}

//=======================================================================================================================================

BOOL SCHEDULING_CHECK_Parallel_Single_Lock( int side , int pt ) { // 2010.04.10
	int c , k , m;
	char selid;
	//
	if ( _SCH_CLUSTER_Get_PathProcessParallel( side , pt , 0 ) == NULL ) return FALSE;
	//
	c = 0;
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , k );
		//
		if ( m > 0 ) {
			//
			if ( c == 1 ) return FALSE;
			//
			c++;
			//
//			selid = _SCH_CLUSTER_Get_PathProcessParallel( side , pt , 0 )[ m - PM1 ]; // 2015.04.10
			selid = _SCH_CLUSTER_Get_PathProcessParallel2( side , pt , 0 , ( m - PM1 ) ); // 2015.04.10
			//
			if ( selid <= 1 ) return FALSE;
			//
		}
		//
	}
	//
	return TRUE;
}

//void SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap_Sub( int side , int TMATM , int bmc ) { // 2004.11.26 // 2017.02.08
BOOL SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap_Sub( int side , int TMATM , int bmc , BOOL fmpump ) { // 2004.11.26 // 2017.02.08
	int i , j , k , s , p , ChkTrg , retstatus = 0;
	int Otherbmc , ArmFreeCount , ArmFreeCountR , bmgopmyes , tmgopmyes , hasoutsp;
	int tmgopm[MAX_PM_CHAMBER_DEPTH];
	int bmgopm[MAX_PM_CHAMBER_DEPTH];
	int pmlock[MAX_PM_CHAMBER_DEPTH];
	int bmrun[MAX_BM_CHAMBER_DEPTH];
	int clidxdiff = 0; // 2007.08.10
	int clidxdt = -1; // 2007.08.10
	int pmslot;
	int hasotherbmc; // 2014.10.20
	//
	int T_W[2] , T_S[2] , T_P[2]; // 2017.02.08
	int P_W[MAX_PM_CHAMBER_DEPTH] , P_S[MAX_PM_CHAMBER_DEPTH] , P_P[MAX_PM_CHAMBER_DEPTH]; // 2017.02.08


	//=============================================================================================
	// 2006.06.22
	//=============================================================================================
	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) != 0 ) return TRUE;
	//=============================================================================================

//_IO_CIM_PRINTF( "[V]======[START]=[SID=%d]=[TMATM=%d]=[BMC=%d]=[LC=%d]\n" , side , TMATM , bmc , PM_LOCK_DENY_CONTROL );

	ArmFreeCount = _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM );

	ArmFreeCountR = ArmFreeCount; // 2005.08.27

//	if ( ArmFreeCount == 0 ) return; // 2004.12.06

	if ( fmpump ) { // 2017.02.08
		//
		for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
			//
			T_W[i] = _SCH_MODULE_Get_TM_WAFER( TMATM , i );
			T_S[i] = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
			T_P[i] = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
		}
		//
		for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
			//
			P_W[i-PM1] = _SCH_MODULE_Get_PM_WAFER( i , 0 );
			P_S[i-PM1] = _SCH_MODULE_Get_PM_SIDE( i , 0 );
			P_P[i-PM1] = _SCH_MODULE_Get_PM_POINTER( i , 0 );
			//
		}
		//
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		pmlock[i] = 0;
		tmgopm[i] = 0;
		bmgopm[i] = 0;
	}
	bmgopmyes = 0;
	tmgopmyes = 0;

	hasoutsp = FALSE; // 2004.12.06
	hasotherbmc = FALSE; // 2014.10.20

	for ( Otherbmc = BM1 ; Otherbmc < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; Otherbmc++ ) {
		//
		bmrun[Otherbmc-BM1] = 0;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( Otherbmc , 0 ) ) continue; // 2016.10.13
		//
		if ( Otherbmc != bmc ) {
//			if ( _SCH_MODULE_GET_USE_ENABLE( Otherbmc , FALSE , -1 ) ) { // 2007.05.28
			if ( _SCH_MODULE_GET_USE_ENABLE( Otherbmc , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( Otherbmc ) == 0 ) ) { // 2007.05.28
				//
				hasotherbmc = TRUE; // 2014.10.20
				//
				if      ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Otherbmc ) || ( _SCH_MODULE_Get_BM_FULL_MODE( Otherbmc ) == BUFFER_TM_STATION ) ) { // pump
					//
					bmrun[Otherbmc-BM1] = 4;
					//
					for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( Otherbmc ) ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( Otherbmc , i + 1 ) > 0 ) {
							if ( _SCH_MODULE_Get_BM_STATUS( Otherbmc , i + 1 ) != BUFFER_OUTWAIT_STATUS ) {
								//
								bmrun[Otherbmc-BM1] = 5;
								//
								bmgopmyes = 1;
								s = _SCH_MODULE_Get_BM_SIDE( Otherbmc , i + 1 );
								p = _SCH_MODULE_Get_BM_POINTER( Otherbmc , i + 1 );
								//===================================================================================
								// 2007.08.10
								//===================================================================================
								if ( clidxdt == -1 ) { // 2007.08.10
									clidxdt = _SCH_CLUSTER_Get_ClusterIndex( s , p );
								}
								else {
									if ( clidxdiff == 0 ) {
										if ( clidxdt != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) clidxdiff = 1;
									}
								}
								//===================================================================================
								// 2010.04.10
								if ( clidxdiff == 0 ) {
									if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) clidxdiff = 2;
								}
								//===================================================================================
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									ChkTrg = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , j );
									if ( ChkTrg > 0 ) {
										//
										if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg ) != 0 ) continue; // 2017.03.07
										//
										if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) bmgopm[ChkTrg - PM1] = 1;
									}
								}
							}
						}
						else {
							if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( Otherbmc , i+1 ) ) continue; // 2018.04.03
							hasoutsp = TRUE; // 2004.12.06
						}
					}
				}
				else if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( Otherbmc ) || ( _SCH_MODULE_Get_BM_FULL_MODE( Otherbmc ) == BUFFER_FM_STATION ) ) { // vent
					//
					bmrun[Otherbmc-BM1] = 6;
					//
					hasoutsp = TRUE; // 2004.12.06
					bmgopmyes = 2;
					for ( i = 0 ; i < _SCH_PRM_GET_MODULE_SIZE( Otherbmc ) ; i++ ) {
						if ( _SCH_MODULE_Get_BM_WAFER( Otherbmc , i + 1 ) > 0 ) {
							if ( _SCH_MODULE_Get_BM_STATUS( Otherbmc , i + 1 ) != BUFFER_OUTWAIT_STATUS ) {
								//
								bmrun[Otherbmc-BM1] = 7;
								//
								bmgopmyes = 3;
								s = _SCH_MODULE_Get_BM_SIDE( Otherbmc , i + 1 );
								p = _SCH_MODULE_Get_BM_POINTER( Otherbmc , i + 1 );
								//===================================================================================
								// 2007.08.10
								//===================================================================================
								if ( clidxdt == -1 ) { // 2007.08.10
									clidxdt = _SCH_CLUSTER_Get_ClusterIndex( s , p );
								}
								else {
									if ( clidxdiff == 0 ) {
										if ( clidxdt != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) clidxdiff = 3;
									}
								}
								//===================================================================================
								// 2010.04.10
								if ( clidxdiff == 0 ) {
									if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) clidxdiff = 4;
								}
								//===================================================================================
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									ChkTrg = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , j );
									if ( ChkTrg > 0 ) {
										//
										if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg ) != 0 ) continue; // 2017.03.07
										//
										if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) bmgopm[ChkTrg - PM1] = 1;
									}
								}
							}
						}
					}
					for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
						if ( _SCH_MODULE_Get_FM_WAFER( i ) > 0 ) {
							s = _SCH_MODULE_Get_FM_SIDE( i );
							p = _SCH_MODULE_Get_FM_POINTER( i );
							//===================================================================================
							// 2007.08.10
							//===================================================================================
							if ( clidxdt == -1 ) { // 2007.08.10
								clidxdt = _SCH_CLUSTER_Get_ClusterIndex( s , p );
							}
							else {
								if ( clidxdiff == 0 ) {
									if ( clidxdt != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) clidxdiff = 5;
								}
							}
							//===================================================================================
							// 2010.04.10
							if ( clidxdiff == 0 ) {
								if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) clidxdiff = 6;
							}
							//===================================================================================
							if ( _SCH_CLUSTER_Get_PathDo( s , p ) == 0 ) {
								//
								bmrun[Otherbmc-BM1] = 8;
								//
								bmgopmyes = 3;
								//
								for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
									//
									ChkTrg = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , j );
									//
									if ( ChkTrg > 0 ) {
										//
										//
										if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg ) != 0 ) continue; // 2017.03.07
										//
										if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) bmgopm[ChkTrg - PM1] = 1;
									}
								}
							}
						}
					}
				}
				else {
					bmrun[Otherbmc-BM1] = 3;
				}
			}
			else {
				bmrun[Otherbmc-BM1] = 2;
			}
		}
		else {
			bmrun[Otherbmc-BM1] = 1;
		}
	}
	//============================================================================================================
	//
	if ( !hasotherbmc ) { // 2014.10.20
		bmgopmyes = 9;
	}
	//
	//============================================================================================================
	for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
		if ( _SCH_MODULE_Get_TM_WAFER( TMATM , i ) > 0 ) {
			s = _SCH_MODULE_Get_TM_SIDE( TMATM , i );
			p = _SCH_MODULE_Get_TM_POINTER( TMATM , i );
			//===================================================================================
			// 2007.08.10
			//===================================================================================
			if ( clidxdt == -1 ) { // 2007.08.10
				clidxdt = _SCH_CLUSTER_Get_ClusterIndex( s , p );
			}
			else {
				if ( clidxdiff == 0 ) {
					if ( clidxdt != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) clidxdiff = 7;
				}
			}
			//===================================================================================
			// 2010.04.10
			if ( clidxdiff == 0 ) {
				if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) clidxdiff = 8;
			}
			//===================================================================================
			if ( _SCH_CLUSTER_Get_PathDo( s , p ) <= _SCH_CLUSTER_Get_PathRange( s , p ) ) { // has next
				tmgopmyes = 1;
				for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
					ChkTrg = _SCH_CLUSTER_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) - 1 , j );
					if ( ChkTrg > 0 ) {
						//
						if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg ) != 0 ) continue; // 2017.03.07
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) tmgopm[ChkTrg - PM1] = 1;
					}
				}
			}
			else { // last // 2004.12.06
				if ( ArmFreeCount == 0 ) {
					if ( hasoutsp ) {
						ArmFreeCount = 1;
					}
				}
				//
			}
		}
	}
	//============================================================================================================
	if ( ArmFreeCount > 0 ) { // 2004.12.06
		for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
			//
//			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue; // 2017.03.07
			//

			if ( SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) { // 2014.01.10
				//
//				if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) ) { // 2014.01.10
				if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) { // 2014.01.10
					//
//					s = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
//					p = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
					//
					s = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2014.01.10
					p = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2014.01.10
					//===================================================================================
					// 2007.08.10
					//===================================================================================
					if ( clidxdt == -1 ) { // 2007.08.10
						clidxdt = _SCH_CLUSTER_Get_ClusterIndex( s , p );
					}
					else {
						if ( clidxdiff == 0 ) {
							if ( clidxdt != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) clidxdiff = 9;
						}
					}
					//===================================================================================
					// 2010.04.10
//					if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) { // 2015.10.29
//						clidxdiff = TRUE; // 2015.10.29
//					} // 2015.10.29
					//
					if ( clidxdiff == 0 ) { if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) clidxdiff = 10; } // 2015.10.29
					//
					if ( clidxdiff == 0 ) { if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , s ) ) clidxdiff = 11; } // 2015.10.29
					//
					//===================================================================================
					if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) { // last for out
						if ( tmgopm[i - PM1] == 0 ) {
							if ( ArmFreeCount == 2 ) {
								pmlock[i - PM1] = 2; // 2005.06.24
								ArmFreeCount = 1;
							}
							else {
								if ( bmgopmyes == 0 ) {
									pmlock[i - PM1] = 3; // 2005.06.24
									bmgopmyes = 1;
								}
								else {
									if ( bmgopm[i - PM1] == 0 ) {
										pmlock[i - PM1] = 11;
										//
										retstatus = 1;
										//
									}
								}
							}
						}
					}
					else {
						k = 0;
						for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
							ChkTrg = _SCH_CLUSTER_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , j );
							if ( ChkTrg > 0 ) {
								//
								if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg ) != 0 ) continue; // 2017.03.07
								//
								if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) {
									//
									tmgopm[ChkTrg - PM1] = 2;
									//
//									if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) k = 1; // 2014.01.10
									if ( SCH_PM_IS_ABSENT( ChkTrg , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) ) ) k = 1; // 2014.01.10
									//
								}
							}
						}
						if ( k == 0 ) { // has not a space
							if ( tmgopm[i - PM1] == 0 ) {
								if ( ArmFreeCount == 2 ) {
									pmlock[i - PM1] = 4; // 2005.06.24
									ArmFreeCount = 1;
								}
								else {
									if ( bmgopmyes == 0 ) {
										pmlock[i - PM1] = 5; // 2005.06.24
										bmgopmyes = 1;
									}
									else {
										if ( bmgopm[i - PM1] == 0 ) {
											pmlock[i - PM1] = 12;
											//
											retstatus = 2;
											//
										}
									}
								}
							}
						}
					}
				}
			}
			else { // 2005.06.24
				pmlock[i - PM1] = 1; // 2005.06.24
			}
		}
	}
	//============================================================================================================
	// 2005.08.27
	//============================================================================================================
	else {
		if ( ArmFreeCountR <= 0 ) {
			for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
				//
				//
				if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue; // 2017.03.07
				//
	//			if ( _SCH_MODULE_Get_PM_WAFER( i , 0 ) > 0 ) { // 2014.01.10

				if ( SCH_PM_HAS_ONLY_PICKING( i , &pmslot ) ) { // 2014.01.10
					//
	//				if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) ) { // 2014.01.10
					if ( _SCH_MODULE_GET_USE_ENABLE( i , TRUE , _SCH_MODULE_Get_PM_SIDE( i , pmslot ) ) ) { // 2014.01.10
						//
	//					s = _SCH_MODULE_Get_PM_SIDE( i , 0 ); // 2014.01.10
	//					p = _SCH_MODULE_Get_PM_POINTER( i , 0 ); // 2014.01.10
						//
						s = _SCH_MODULE_Get_PM_SIDE( i , pmslot ); // 2014.01.10
						p = _SCH_MODULE_Get_PM_POINTER( i , pmslot ); // 2014.01.10

						//===================================================================================
						// 2007.08.10
						//===================================================================================
						if ( clidxdt == -1 ) { // 2007.08.10
							clidxdt = _SCH_CLUSTER_Get_ClusterIndex( s , p );
						}
						else {
							if ( clidxdiff == 0 ) {
								if ( clidxdt != _SCH_CLUSTER_Get_ClusterIndex( s , p ) ) clidxdiff = 12;
							}
						}
						//===================================================================================
						// 2010.04.10
//						if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) { // 2015.10.29
//							clidxdiff = TRUE; // 2015.10.29
//						} // 2015.10.29
						//
						if ( clidxdiff == 0 ) { if ( SCHEDULING_CHECK_Parallel_Single_Lock( s , p ) ) clidxdiff = 13; } // 2015.10.29
						//
						if ( clidxdiff == 0 ) { if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , s ) ) clidxdiff = 14; } // 2015.10.29
						//
						//===================================================================================
						if ( _SCH_CLUSTER_Get_PathDo( s , p ) >= _SCH_CLUSTER_Get_PathRange( s , p ) ) { // last for out
							if ( tmgopm[i - PM1] == 0 ) {
								if ( bmgopmyes == 0 ) {
									pmlock[i - PM1] = 7;
									bmgopmyes = 1;
								}
								else {
									if ( bmgopm[i - PM1] == 0 ) {
										pmlock[i - PM1] = 13;
										//
										retstatus = 3;
										//
									}
								}
							}
						}
						else {
							k = 0;
							for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
								ChkTrg = _SCH_CLUSTER_Get_PathProcessChamber( s , p , _SCH_CLUSTER_Get_PathDo( s , p ) , j );
								if ( ChkTrg > 0 ) {
									//
									if ( _SCH_PRM_GET_MODULE_GROUP( ChkTrg ) != 0 ) continue; // 2017.03.07
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( ChkTrg , FALSE , s ) ) {
										tmgopm[ChkTrg - PM1] = 2;
//										if ( _SCH_MODULE_Get_PM_WAFER( ChkTrg , 0 ) <= 0 ) k = 1; // 2014.01.10
										if ( SCH_PM_IS_ABSENT( ChkTrg , s , p , _SCH_CLUSTER_Get_PathDo( s , p ) ) ) k = 1; // 2014.01.10
									}
								}
							}
							if ( k == 0 ) { // has not a space
								if ( tmgopm[i - PM1] == 0 ) {
									if ( bmgopmyes == 0 ) {
										pmlock[i - PM1] = 8;
										bmgopmyes = 1;
									}
									else {
										if ( bmgopm[i - PM1] == 0 ) {
											pmlock[i - PM1] = 14;
											//
											retstatus = 4;
											//
										}
									}
								}
							}
						}
					}
				}
				else {
					pmlock[i - PM1] = 6;
				}
			}
		}
	}
	//============================================================================================================
//	PM_LOCK_DENY_CONTROL = FALSE; // 2014.12.26
	//============================================================================================================
//	_IO_CIM_PRINTF( "[V]======[S:%d,T:%d,B:%d]=[LC=%d]=[retstatus:%d]=[ArmFree:%d,%d]=[clidxdiff:%d]=[hasoutsp:%d]===\n" , side , TMATM , bmc , PM_LOCK_DENY_CONTROL , retstatus , ArmFreeCount , ArmFreeCountR , clidxdiff , hasoutsp );
//	_IO_CIM_PRINTF( "[V]===> T[%d]  (P1=%d)(P2=%d)(P3=%d)(P4=%d)(P5=%d)(P6=%d)(P7=%d)(P8=%d) (P11=%d)\n" , tmgopmyes , tmgopm[0] , tmgopm[1] , tmgopm[2] , tmgopm[3] , tmgopm[4] , tmgopm[5] , tmgopm[6] , tmgopm[7] , tmgopm[10] );
//	_IO_CIM_PRINTF( "[V]===> B[%d]  (P1=%d)(P2=%d)(P3=%d)(P4=%d)(P5=%d)(P6=%d)(P7=%d)(P8=%d) (P11=%d)\n" , bmgopmyes , bmgopm[0] , bmgopm[1] , bmgopm[2] , bmgopm[3] , bmgopm[4] , bmgopm[5] , bmgopm[6] , bmgopm[7] , bmgopm[10] );
//	_IO_CIM_PRINTF( "[V]===> P[%d]  (P1=%d)(P2=%d)(P3=%d)(P4=%d)(P5=%d)(P6=%d)(P7=%d)(P8=%d) (P11=%d)\n" , retstatus , pmlock[0] , pmlock[1] , pmlock[2] , pmlock[3] , pmlock[4] , pmlock[5] , pmlock[6] , pmlock[7] , pmlock[10] );
//	_IO_CIM_PRINTF( "[V]===> bmrn  (B1=%d)(B2=%d)(B3=%d)(B4=%d)(B5=%d)(B6=%d)(B7=%d)(B8=%d)\n" , bmrun[0] , bmrun[1] , bmrun[2] , bmrun[3] , bmrun[4] , bmrun[5] , bmrun[6] , bmrun[7] );
	//============================================================================================================

	if ( fmpump ) { // 2017.02.08
		//
		if ( ArmFreeCountR != _SCH_ROBOT_GET_FINGER_FREE_COUNT( TMATM ) ) return FALSE;
		//
		for ( i = TA_STATION ; i <= TB_STATION ; i++ ) {
			//
			if ( T_W[i] != _SCH_MODULE_Get_TM_WAFER( TMATM , i ) ) return FALSE;
			if ( T_S[i] != _SCH_MODULE_Get_TM_SIDE( TMATM , i ) ) return FALSE;
			if ( T_P[i] != _SCH_MODULE_Get_TM_POINTER( TMATM , i ) ) return FALSE;
		}
		//
		for ( i = PM1 ; i < ( MAX_PM_CHAMBER_DEPTH + PM1 ) ; i++ ) {
			//
			if ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) continue; // 2017.03.07
			//
			if ( P_W[i-PM1] != _SCH_MODULE_Get_PM_WAFER( i , 0 ) ) return FALSE;
			if ( P_S[i-PM1] != _SCH_MODULE_Get_PM_SIDE( i , 0 ) ) return FALSE;
			if ( P_P[i-PM1] != _SCH_MODULE_Get_PM_POINTER( i , 0 ) ) return FALSE;
			//
		}
		//
	}
	//
	if ( clidxdiff == 0 ) {
		PM_LOCK_DENY_CONTROL = FALSE; // 2014.12.26
		return TRUE; // 2007.08.10
	}
	//============================================================================================================
	if ( retstatus > 0 ) {
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) PM_LOCK_DENY_MODULE[i] = pmlock[i];
		PM_LOCK_DENY_CONTROL = TRUE;
	}
	else {
		PM_LOCK_DENY_CONTROL = FALSE; // 2014.12.26
	}
	//============================================================================================================
//	_IO_CIM_PRINTF( "[V]===> L[%d]  (P1=%d)(P2=%d)(P3=%d)(P4=%d)(P5=%d)(P6=%d)(P7=%d)(P8=%d) (P11=%d)\n" , PM_LOCK_DENY_CONTROL , PM_LOCK_DENY_MODULE[0] , PM_LOCK_DENY_MODULE[1] , PM_LOCK_DENY_MODULE[2] , PM_LOCK_DENY_MODULE[3] , PM_LOCK_DENY_MODULE[4] , PM_LOCK_DENY_MODULE[5] , PM_LOCK_DENY_MODULE[6] , PM_LOCK_DENY_MODULE[7] , PM_LOCK_DENY_MODULE[10] );
//	_IO_CIM_PRINTF( "[V]=======================================\n" );
	//============================================================================================================
	return TRUE;
}

void SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap( int side , int TMATM , int bmc ) { // 2014.12.26
	//
	EnterCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
	//
	SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap_Sub( side , TMATM , bmc , FALSE );
	//
	LeaveCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
	//
}

void SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Pump_for_Switch_SingleSwap1( int side , int bmc ) { // 2017.02.08
	//
	if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) != 1 ) return;
	//
	EnterCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 );
	//
	while ( TRUE ) {
		//
		if ( SCHEDULING_CHECK_FEM_Make_NoMore_Supply_before_BM_Vent_for_Switch_SingleSwap_Sub( side , 0 , bmc , TRUE ) ) break;
		//
	}
	LeaveCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 );
	//
}
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
int Sch_NoWay_Option(); // 2010.12.08
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
BOOL SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( int side , int bmc , int mode ) { // 2004.03.10
	int i , ret_rcm , ret_pt;

	//==================================================================================================================
	// 2007.11.20
	//==================================================================================================================
	if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) == 1 ) {
		if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
			if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TA_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
				return FALSE;
			}
			if ( ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) > 0 ) && ( _SCH_MODULE_Get_FM_MODE( TB_STATION ) == FMWFR_PICK_CM_DONE_AL ) ) {
				return FALSE;
			}
		}
	}
	//==================================================================================================================
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
//			if ( _SCH_SYSTEM_USING_GET( i ) > 0 ) { // 2005.03.04
//			if ( ( _SCH_SYSTEM_USING_GET( i ) >= 9 ) || ( ( _SCH_SYSTEM_USING_GET( i ) > 0 ) && ( _SCH_SYSTEM_MODE_GET( i ) < 2 ) ) ) { // 2005.03.04 // 2005.07.29
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				//
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , bmc ) == MUF_00_NOTUSE ) continue; // 2014.05.30
				//
				if ( !SCHEDULER_FM_FM_Finish_Status( i ) ) {
					//==================================================================================================================
					// 2007.11.20
					//==================================================================================================================
//					if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) { // 2007.11.20 // 2018.12.20
					if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1001 : 1000 ) ) { // 2007.11.20 // 2018.12.20
					//==================================================================================================================
						//
//						if ( mode == 1 ) { // 2009.07.10 // 2010.12.08
						if ( ( mode == 1 ) || ( Sch_NoWay_Option() == -2 ) ) { // 2009.07.10 // 2010.12.08
							if ( _SCH_SUB_FM_Current_No_Way_Supply( i , ret_pt , Sch_NoWay_Option() ) ) continue; // 2009.07.10
						} // 2009.07.10
						//
						if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == 0 ) return FALSE;
						if ( bmc == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) ) return FALSE;
					//==================================================================================================================
					}
				}
			}
		}
	}
	return TRUE;
}
//=======================================================================================
BOOL SCHEDULING_CHECK_OtherBM_TMSIDE_and_Remain_One_for_Switch( int side , int bmc ) { // 2004.11.29
	int i , j , c , cc;
	//----------------------------------------------------------------------------------------------------------
	// 2009.08.22
	//----------------------------------------------------------------------------------------------------------
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , bmc ) == MUF_00_NOTUSE ) continue;
		//
		c = 0;
		//
		for ( j = BM1 ; j < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; j++ ) {
			//
			if ( j == bmc ) continue;
			//
			if ( !_SCH_MODULE_GET_USE_ENABLE( j , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( j ) != 0 ) ) continue;
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , j ) == MUF_00_NOTUSE ) continue;
			//
			c++;
			//
			break;
		}
		//
		if ( c == 0 ) {
			if ( !_SCH_MODULE_Chk_TM_Free_Status( i ) ) return FALSE;
		}
		//
	}
	//----------------------------------------------------------------------------------------------------------
	c = 0;
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( i != bmc ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2006.12.22
				if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
					//
					if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue; // 2014.06.10
					//
					if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == 0 ) {
						//
						// 2014.01.22
						if ( SCHEDULER_CONTROL_Chk_BM_FREE_COUNT( i ) <= 0 ) {
							if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) > 0 ) {
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_INWAIT_STATUS ) > 0 ) {
									continue;
								}
							}
						}
						//
						c = 1;
						//
						break;
					}
				}
			}
		}
	}
	cc = 0;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( _SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
			//
			if ( c == 1 ) { // 2019.05.29
				//
				if ( ( _SCH_SYSTEM_MODE_END_GET( i ) != 0 ) || _SCH_MODULE_Chk_FM_Finish_Status( i ) ) {
					//
					cc = cc + ( _SCH_MODULE_Get_TM_OutCount(i) - _SCH_MODULE_Get_TM_InCount(i) );
					//
				}
				//
			}
			else {
				cc = cc + ( _SCH_MODULE_Get_TM_OutCount(i) - _SCH_MODULE_Get_TM_InCount(i) );
			}
		}
	}
	//
	if ( cc > c ) return FALSE;
	//
	return TRUE;
}

//=======================================================================================================================================

BOOL SCHEDULING_CHECK_AllBM_Pump_and_hasPlaceSpace_for_Switch_SingleSwap() { // 2004.12.06
	int i , ok , j = 0 , f = FALSE;
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2007.05.28
			j++;
			ok = FALSE;
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2006.05.26
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) { // 2006.05.26
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( i , BUFFER_OUTWAIT_STATUS ) <= 0 ) ok = TRUE; // 2006.11.30
					if ( SCHEDULER_CONTROL_Chk_BM_FREE_ALL( i ) ) f = TRUE;

				}
			}
			if ( !ok ) return FALSE;
		}
	}
	if ( j == 0 ) return FALSE;
	if ( !f ) return FALSE;
	return TRUE;
}

//=======================================================================================
/*
//
// 2016.11.04
//
//
//int SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( int side , int bmc , int nextmode ) { // 2004.06.15
int SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( int side , int tmside , int bmc , int nextmode ) { // 2004.06.15 // 2016.10.20
	int i , j , s , c , SchPointer;
	int sm_supid , sm_supbm;

	if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmc , BUFFER_OUTWAIT_STATUS ) > 0 ) return -1; // 2008.01.23

//	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) return -2; // 2008.08.22 // 2016.10.20

	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) { // 2008.08.22 // 2016.10.20
		//
		// 2016.10.20
		//
		s = 0;
		//
		if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tmside ) > 0 ) {
			if ( _SCH_PRM_GET_DFIX_FM_DOUBLE_CONTROL() != 0 ) {
				if ( ( _SCH_PRM_GET_INTERLOCK_TM_SINGLE_RUN( tmside ) % 10 ) != 0 ) {
					s = 1;
				}
			}
			else { // 2016.10.26
				s = 2;
			}
		}
		//
		if ( s == 0 ) return -2;
		//
	}
	else {
		if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) > 1 ) return -3;
	}
	//
	sm_supbm = -1;
	sm_supid = -1;
	//
//	if ( _SCH_SYSTEM_MODE_GET( side ) != 0 ) { // 2011.04.08
		for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
			if ( s != side ) {
//				if ( _SCH_SYSTEM_USING_GET( s ) > 0 ) { // 2005.07.29
//				if ( ( _SCH_PRM_GET_MODULE_SIZE( bmc ) <= 1 ) || ( _SCH_SYSTEM_MODE_GET( side ) != 0 ) ) { // 2011.04.08 // 2016.10.20
					if ( _SCH_SYSTEM_USING_RUNNING( s ) ) { // 2005.07.29
						//
						if ( _SCH_SYSTEM_RUNORDER_GET( s ) < _SCH_SYSTEM_RUNORDER_GET( side ) ) {
							//
							for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
								if ( bmc != i ) {
	//								if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) { // 2007.05.28
									//
									if ( _SCH_MODULE_Get_Mdl_Use_Flag( s , i ) == MUF_00_NOTUSE ) continue; // 2014.06.10
									//
									if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2007.05.28
										//
//										if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2016.11.04
											//
											SchPointer = -1;
											//
											if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , s , &SchPointer , &j , TRUE , 0 , &c ) > 0 ) {
												//
												//
												return 1;
											}
//										} // 2016.11.04
									}
								}
							}
						}
					}
//				} // 2016.10.20
			}
			else { // 2011.04.07
				//
//				if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) <= 1 ) { // 2016.10.20
					//
					for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
						//
						if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) {
							//
							if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) {
								//
								SchPointer = -1;
								//
								if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , s , &SchPointer , &j , TRUE , 0 , &c ) > 0 ) {
									//
									j = _SCH_CLUSTER_Get_SupplyID( s , SchPointer );
									//
									if ( ( sm_supbm == -1 ) || ( sm_supid > j ) ) {
										sm_supbm = i;
										sm_supid = j;
									}
									//
								}
							}
						}
					}
					//
					if ( ( sm_supbm != -1 ) && ( sm_supbm != bmc ) ) return 2;
					//
//				} // 2016.10.20
			}
		}
//	} // 2011.04.18
	//
	if ( sm_supbm >= 0 ) return -sm_supbm;
	//
	return 0;
}
*/
//
//
// 2016.11.04
//
BOOL SCHEDULING_CHECK_Diff_Path( int s , int p , int src_s , int src_p , BOOL );

BOOL SCHEDULING_CHECK_NoWay_TMSide( int s , int p , int tmside ) { // 2017.02.07
	int i , m;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , i );
		//
		if  ( m > 0 ) {
			if ( _SCH_MODULE_GROUP_TPM_POSSIBLE( tmside , m , G_PLACE ) ) return FALSE;
		}
		//
	}
	//
	return TRUE;
	//
}

int SCHEDULING_CHECK_BM_Continue_Deny_for_Switch( int side , int tmside , int bmc , int nextmode ) { // 2004.06.15 // 2016.10.20
	int i , j , side2 , Pointer , Pointer2;
	BOOL NoWay_TMSide1 , NoWay_TMSide2; // 2017.02.07
	//
	if ( !_SCH_MODULE_GET_USE_ENABLE( bmc, FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( bmc ) != tmside ) ) return -1; // 2016.11.07
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() >= BUFFER_SWITCH_FULLSWAP ) {
//		if ( _SCH_PRM_GET_DFIX_TM_M_DOUBLE_CONTROL( tmside ) > 0 ) { // 2019.01.30
//		} // 2019.01.30
//		else { // 2019.01.30
//			return -2; // 2019.01.30
//		} // 2019.01.30
	}
	else {
		if ( _SCH_PRM_GET_MODULE_SIZE( bmc ) > 1 ) return -3;
	}
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( bmc ) && ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) ) return 1; // 2017.01.11
	if ( _SCH_PRM_GET_MODULE_BUFFER_FULLRUN( bmc ) && ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) ) return 1000; // 2017.01.11
	//
	Pointer = -1;
	//
//	if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( bmc , -1 , &Pointer , &j , TRUE , 0 , &side2 ) <= 0 ) return 2; // 2017.01.11
	if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( bmc , -1 , &Pointer , &j , TRUE , 0 , &side2 ) <= 0 ) return 2000; // 2017.01.11
	//
//	if ( side != side2 ) return 3; // 2017.01.11
	if ( side != side2 ) return 3000; // 2017.01.11
	//
	NoWay_TMSide1 = SCHEDULING_CHECK_NoWay_TMSide( side , Pointer , tmside ); // 2017.02.07
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( bmc == i ) continue;
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_WAIT_STATION ) continue;
		//
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == tmside ) ) {
			//
			if ( _SCH_PRM_GET_Getting_Priority( bmc ) != _SCH_PRM_GET_Getting_Priority( i ) ) continue; // 2016.11.24
			//
			Pointer2 = -1;
			//
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_IN_SIDE_POINTER( i , -1 , &Pointer2 , &j , TRUE , 0 , &side2 ) > 0 ) {
				//
				NoWay_TMSide2 = SCHEDULING_CHECK_NoWay_TMSide( side2 , Pointer2 , tmside ); // 2017.02.07
				//
				if ( ( NoWay_TMSide1 && NoWay_TMSide2 ) || ( !NoWay_TMSide1 && !NoWay_TMSide2 ) ) { // 2017.02.07
					//
	//				if ( !SCHEDULING_CHECK_Diff_Path( side , Pointer , side2 , Pointer2 ) ) { // 2016.11.19
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [C-1]= tmside=%d , side2 = %d , Pointer2 = %d\n" , side , tmside , side2 , Pointer2 ); // 2017.01.11
					if ( !SCHEDULING_CHECK_Diff_Path( side , Pointer , side2 , Pointer2 , TRUE ) ) { // 2016.11.19
						//
	_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [C-2]= tmside=%d , side2 = %d , Pointer2 = %d\n" , side , tmside , side2 , Pointer2 ); // 2017.01.11
						if ( _SCH_CLUSTER_Get_SupplyID( side , Pointer ) > _SCH_CLUSTER_Get_SupplyID( side2 , Pointer2 ) ) {
							//
	//						return i; // 2017.01.11
							return i * 1000; // 2017.01.11
						}
					}
					//
				}
				//
			}
			//
		}
_SCH_LOG_DEBUG_PRINTF( side , 1 , "TM  STEP 04 (k=%d) CHECK_PICK_from_BM [C-9]= tmside=%d , side2 = %d , Pointer2 = %d\n" , side , tmside , side2 , Pointer2 ); // 2017.01.11
	}
	//
	return 0;
}

//=======================================================================================

BOOL SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( int TMATM , BOOL );

//void SCHEDULING_CHECK_BM_Change_FMSIDE_for_Switch( int side ) { // 2014.08.24
void SCHEDULING_CHECK_BM_Change_FMSIDE_for_Switch( int tms , int side ) { // 2014.08.24
	int i , j , s , p , w;
	//
	int sc , lc; // 2015.09.02
	//
	//
//	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() <= BUFFER_SWITCH_BATCH_PART ) return; // 2014.08.24
	if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) <= BUFFER_SWITCH_BATCH_PART ) return; // 2014.08.24
	//
	if ( _SCH_PRM_GET_EIL_INTERFACE() > 0 ) { // 2011.07.26
//		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) return; // 2014.08.24
		if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP ) return; // 2014.08.24
	}
	//
	if ( SCHEDULING_CHECK_for_TM_ARM_Has_Return_Wafer( tms , FALSE ) ) return; // 2018.10.20
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue; // 2014.08.24
		if ( !_SCH_MODULE_GROUP_TBM_POSSIBLE( tms , i , G_PLACE , G_MCHECK_SAME ) ) continue; // 2014.08.24
		//
//		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) { // 2007.05.28 // 2014.08.24
		if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == tms ) ) { // 2007.05.28 // 2014.08.24
			//
			if ( !_SCH_PRM_GET_MODULE_BUFFER_FULLRUN( i ) || ( _SCH_MODULE_Get_BM_FULL_MODE( i ) == BUFFER_TM_STATION ) ) { // 2006.05.26
				//
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) <= 0 ) { // 2006.05.26
					//
					w = 0;
					//
					if ( _SCH_PRM_GET_MODULE_BUFFER_M_SWITCH_SWAPPING( tms ) == BUFFER_SWITCH_MIDDLESWAP ) { // 2015.09.02
						//
						SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , FALSE , &sc , &lc ); // out
						//
						for ( j = sc ; j <= lc ; j++ ) {
							//
							if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
								if ( _SCH_MODULE_Get_BM_STATUS( i , j ) == BUFFER_OUTWAIT_STATUS ) {
									s = _SCH_MODULE_Get_BM_SIDE( i , j );
									p = _SCH_MODULE_Get_BM_POINTER( i , j );
									if ( s == side ) w++;
								}
							}
							//
						}
						//
						SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( i , TRUE , &sc , &lc ); // in
						//
						for ( j = sc ; j <= lc ; j++ ) {
							//
							if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
								w = 0;
								break;
							}
							//
						}
					}
					else {
						for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
							if ( _SCH_MODULE_Get_BM_WAFER( i , j ) > 0 ) {
								if ( _SCH_MODULE_Get_BM_STATUS( i , j ) != BUFFER_OUTWAIT_STATUS ) {
									w = 0;
									break;
								}
								else {
									s = _SCH_MODULE_Get_BM_SIDE( i , j );
									p = _SCH_MODULE_Get_BM_POINTER( i , j );
									if ( s == side ) w++;
									if ( _SCH_CLUSTER_Get_PathDo( s , p ) != PATHDO_WAFER_RETURN ) {
										w = 0;
										break;
									}
								}
							}
						}
					}
					//
					if ( w > 0 ) {
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_FM_STATION , side , i , -1 , TRUE , 1 , 1001 );
					}
					//
				}
			}
		}
	}
}

int  SCHEDULING_CHECK_BM_HAS_DualExt_for_Switch( int ch , int side , int slot , int dualext , int armb , int *res ) { // 2006.02.08
	int i , offset , ok = FALSE;
	int pt , po , so , slot_real;
	int pt2;

	*res = FALSE;
	//==========================================================================================
	// 2006.07.24
	//==========================================================================================
	if ( armb ) {
		offset = 0;
		slot_real = slot;
	}
	else {
		offset = ( ( slot - 1 ) % ( dualext + 1 ) );
		slot_real = slot - offset;
	}
	//==========================================================================================
//	if ( ( slot + dualext ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) { // 2006.07.24
	if ( ( slot_real + dualext ) <= _SCH_PRM_GET_MODULE_SIZE( ch ) ) { // 2006.07.24
		pt = _SCH_MODULE_Get_BM_POINTER( ch , slot );
		po = _SCH_CLUSTER_Get_PathOut( side , pt );
		so = _SCH_CLUSTER_Get_SlotOut( side , pt );
//		for ( i = 0 ; i < dualext ; i++ ) { // 2006.07.24
		for ( i = offset ; i < dualext ; i++ ) { // 2006.07.24
//			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + slot + 1 ) > 0 ) { // 2006.07.24
			if ( _SCH_MODULE_Get_BM_WAFER( ch , i + slot_real + 1 ) > 0 ) { // 2006.07.24
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() > BUFFER_SWITCH_BATCH_PART ) {
//					if ( _SCH_MODULE_Get_BM_SIDE( ch , i + slot + 1 ) != side ) break; // 2006.07.24
					if ( _SCH_MODULE_Get_BM_SIDE( ch , i + slot_real + 1 ) != side ) break; // 2006.07.24
				}
				if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_SINGLESWAP ) {
//					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + slot + 1 ) != BUFFER_OUTWAIT_STATUS ) break; // 2006.07.24
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + slot_real + 1 ) != BUFFER_OUTWAIT_STATUS ) break; // 2006.07.24
				}
				else {
//					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + slot + 1 ) == BUFFER_READY_STATUS ) break; // 2006.07.24
					if ( _SCH_MODULE_Get_BM_STATUS( ch , i + slot_real + 1 ) == BUFFER_READY_STATUS ) break; // 2006.07.24
				}
//				pt2 = _SCH_MODULE_Get_BM_POINTER( ch , i + slot + 1 ); // 2006.07.24
				pt2 = _SCH_MODULE_Get_BM_POINTER( ch , i + slot_real + 1 ); // 2006.07.24
				if ( po != _SCH_CLUSTER_Get_PathOut( side , pt2 ) ) break;
//				if ( ( so + i ) != _SCH_CLUSTER_Get_SlotOut( side , pt2 ) ) break; // 2006.07.24
				if ( ( so + i - offset ) != ( _SCH_CLUSTER_Get_SlotOut( side , pt2 ) - 1 ) ) break; // 2006.07.24
			}
			//----------------------------------------------------------------------------
			if ( _SCH_IO_GET_MODULE_STATUS( po , so + i - offset + 1 ) != CWM_ABSENT ) break; // 2006.07.24
			//----------------------------------------------------------------------------
		}
		if ( i == dualext ) return 1;
	}
	if ( armb ) {
		*res = TRUE;
		return 2;
	}
	return -1;
}

BOOL SCHEDULING_CHECK_TM_GetOut_BM_Conflict_Check_for_Switch( int side ) { // 2006.05.24
	int bmc;
//	if ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) == 0 ) return FALSE; // 2007.09.27
//	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == 0 ) return FALSE; // 2007.09.27
	if ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER( side ) == 0 ) return TRUE; // 2007.09.27
	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == 0 ) return TRUE; // 2007.09.27
	//
	if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) == 0 ) {
		bmc = SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side );
	}
	else {
		bmc = SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side );
	}
	if ( _SCH_MODULE_Get_BM_FULL_MODE( bmc ) != BUFFER_TM_STATION ) {
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( bmc ) <= 0 ) {
			return TRUE;
		}
	}
	return FALSE;
}

//===========================================================================================
//===========================================================================================
//===========================================================================================

BOOL Scheduler_Separate_Check_for_Batch_All( int side ) {
	int i , sch , tch , count;

	//========================================================================
	if ( !_SCH_SYSTEM_USING_STARTING_ONLY( side ) ) return FALSE;
	//========================================================================
	SCHEDULER_CONTROL_Set_BM_Switch_Separate( 0 , 0 , 0 , 0 , 0 );
	//========================================================================
	if ( !USER_RECIPE_SP_N101_SEPSUPPLY( side , &count ) ) return FALSE;
	//========================================================================
	if ( count <= 0 ) return FALSE;
	if ( count >= _SCH_PRM_GET_DFIX_MAX_CASSETTE_SLOT() ) return FALSE;
	//========================================================================
	switch ( _SCH_PRM_GET_MODULE_SWITCH_BUFFER_ACCESS_TYPE( side ) / 2 ) {
	case 0 :
		sch = side + BM1;
		break;
	case 1 :
		sch = 0 + BM1;
		break;
	case 2 :
		sch = 1 + BM1;
		break;
	}
	if ( !_SCH_MODULE_GET_USE_ENABLE( sch , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( sch ) != 0 ) ) {
		sch = -1;
		for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) {
				sch = i;
				break;
			}
		}
		if ( sch == -1 ) return FALSE;
	}
	//===================================================================================================
	tch = -1;
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		if ( i != sch ) {
			if ( _SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) && ( _SCH_PRM_GET_MODULE_GROUP( i ) == 0 ) ) {
				tch = i;
				break;
			}
		}
	}
	if ( tch == -1 ) return FALSE;
	//===================================================================================================
	if ( SCHEDULER_CONTROL_Get_BM_Switch_SeparateMode() != 0 ) return FALSE;
	//===================================================================================================
	SCHEDULER_CONTROL_Set_BM_Switch_WhatCh( side , sch );
	SCHEDULER_CONTROL_Set_BM_Switch_CrossCh( side , 0 );
	//
//	SCHEDULER_CONTROL_Set_BM_Switch_UseSide( sch , side ); // 2008.11.24
//	SCHEDULER_CONTROL_Set_BM_Switch_UseSide( tch , side ); // 2008.11.24
	//===================================================================================================
	SCHEDULER_CONTROL_Set_BM_Switch_Separate( 1 , side , count , sch , tch );
	//===================================================================================================
	return TRUE;
}

BOOL Scheduler_Switch_Side_Check_for_TM_Place( int side , int ch ) { // 2008.07.01
//	if ( SCHEDULER_CONTROL_Get_BM_Switch_UseSide( ch ) != side ) return FALSE; // 2008.11.24
	//
//	if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() % 4 ) == 0 ) return TRUE; // 2008.07.12 // 2008.11.24
	//
	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == 0 ) return TRUE;
	//
	if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) == 0 ) {
		if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == ch ) return TRUE;
	}
	else {
		if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) == ch ) return TRUE;
	}
	return FALSE;
}


BOOL Scheduler_Supply_Waiting_for_Batch_All_has_Space( int side , int xbch ) { // 2009.12.23
	int i , j , f;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( i == xbch ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) continue;
		//
		f = FALSE;
		//
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			//
			if ( _SCH_SYSTEM_USING_GET( j ) <= 0 ) continue;
			if ( _SCH_SYSTEM_MODE_END_GET( j ) != 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( j ) == i ) {
				f = FALSE;
				break;
			}
			//
			f = TRUE;
		}
		//
		if ( f ) return TRUE;
		//
	}
	return FALSE;
}

BOOL Scheduler_Supply_Waiting_for_Batch_All( int side , BOOL mode , int bch ) { // 2006.09.07
	int i;
	//
	if ( mode ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i == side ) continue;
			if ( _SCH_SYSTEM_USING_GET( i ) <= 0 ) continue;
			if ( _SCH_SYSTEM_MODE_END_GET( i ) != 0 ) continue;
			//
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == 0 ) {
				if ( Scheduler_Supply_Waiting_for_Batch_All_has_Space( i , bch ) ) return TRUE; // 2009.12.23
				continue;
			}
			//
			if ( ( _SCH_MODULE_Get_FM_SwWait( i ) == 1 ) || ( _SCH_MODULE_Get_FM_SwWait( i ) == 2 ) ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == bch ) continue; // 2007.01.25
				return TRUE;
			}
		}
	}
	else {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			if ( i == side ) continue;
			if ( _SCH_SYSTEM_USING_GET( i ) <= 0 ) continue;
			if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == 0 ) continue;
			if ( ( _SCH_MODULE_Get_FM_SwWait( i ) == 1 ) || ( _SCH_MODULE_Get_FM_SwWait( i ) == 2 ) ) {
				if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) == 0 ) {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) ) == BUFFER_TM_STATION ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) ) ) {
							if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == bch ) continue; // 2007.01.25
							return TRUE;
						}
					}
				}
				else {
					if ( _SCH_MODULE_Get_BM_FULL_MODE( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) ) == BUFFER_TM_STATION ) {
						if ( !SCHEDULER_CONTROL_Chk_BM_FREE_ALL( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) ) ) {
							if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == bch ) continue; // 2007.01.25
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}



BOOL Scheduler_FEM_OtherBM_GetOut_ImPossible_MFIC_SingleSwap( int side , int bmc ) { // 2011.11.30
	int i , j , ICsts , ICsts2;

	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( i ) != 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_BM_FULL_MODE( i ) != BUFFER_FM_STATION ) continue;
		if ( _SCH_MACRO_CHECK_PROCESSING_INFO( i ) > 0 ) continue;
		//
		for ( j = 1 ; j <= _SCH_PRM_GET_MODULE_SIZE( i ) ; j++ ) {
			//
			if ( _SCH_MODULE_Get_BM_WAFER( i , j ) <= 0 ) continue;
			//
			if ( _SCH_MODULE_Get_BM_STATUS( i , j ) != BUFFER_OUTWAIT_STATUS ) continue;
			//
			if ( _SCH_MODULE_Get_BM_SIDE( i , j ) != side ) continue;
			//
			if ( _SCH_MODULE_Chk_MFIC_FREE_TYPE3_SLOT( side , _SCH_MODULE_Get_BM_POINTER( i , j ) , -1 , -1 , &ICsts , &ICsts2 , _SCH_PRM_GET_RB_FM_ARM_IS_DUAL_EXTEND_STYLE( 0 ) ) <= 0 ) return TRUE;
			//
		}
	}
	//
	return FALSE;
}

BOOL Scheduler_FEM_Other_GetOut_Possible_SingleSwap( int side , int bmc , int *cside ) {
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		*cside = i; // 2004.11.08
		if ( i != side ) {
//			if ( _SCH_SYSTEM_USING_GET( i ) >= 9 ) { // 2005.07.29
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				if ( _SCH_SYSTEM_MODE_END_GET( i ) == 0 ) { // 2003.11.09
					//
					if ( bmc > 0 ) { // 2014.06.10
						if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , bmc ) == MUF_00_NOTUSE ) continue;
					}
					//
					if ( Scheduler_FEM_OtherBM_GetOut_ImPossible_MFIC_SingleSwap( i , bmc ) ) continue; // 2011.11.30
					//
//					if ( _SCH_SUB_Remain_for_FM( i ) ) { // 2011.08.11
					if ( !SCHEDULER_FM_Current_No_More_Supply( i ) ) { // 2011.08.11
						//
						if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) == 0 ) {
							//=================================================================================
							if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( i , TRUE , 1 ) ) return TRUE; // 2004.05.25
							//=================================================================================
							//return TRUE; // 2004.03.10 2004.05.25
						}
						if ( bmc == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) ) {
							//=================================================================================
							if ( _SCH_CASSETTE_Check_Side_Monitor_Possible_Supply( i , TRUE , 1 ) ) return TRUE; // 2004.05.25
							//=================================================================================
							//return TRUE; // 2004.03.10 2004.05.25
						}
//						return TRUE; // 2004.03.10
					}
				}
			}
		}
	}
	return FALSE;
}
//
BOOL Scheduler_FEM_Will_Out_Check_for_Switch( int side , int bm ) { // 2004.02.19
	int i;
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
//			if ( _SCH_SYSTEM_USING_GET( i ) >= 9 ) { // 2005.07.29
			if ( _SCH_SYSTEM_USING_RUNNING( i ) ) { // 2005.07.29
				if ( _SCH_SYSTEM_MODE_END_GET( i ) == 0 ) {
					if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( i ) != 0 ) {
						if ( bm == SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( i ) ) return TRUE;
					}
				}
			}
		}
	}
	//========================================================================
	// 2006.12.18
	//========================================================================
	if ( _SCH_SYSTEM_MODE_END_GET( side ) != 0 ) return TRUE;
	if ( bm == SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) ) return TRUE;
	//========================================================================
	return FALSE;
}
//
BOOL Scheduler_FEM_GetIn_Properly_Check_for_Switch( int side , int ch ) {
	//
	if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( ch , G_PLACE , 0 ) ) return FALSE; // 2018.06.01
	//
	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == 0 ) return TRUE;
	if ( ch == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) ) return TRUE;
	return FALSE;
}
//
BOOL Scheduler_FEM_GetOut_Properly_Check_for_Switch( int side , int ch ) {
	//
	if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( ch , G_PICK , 0 ) ) return FALSE; // 2018.06.01
	//
	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) == 0 ) return TRUE;
	if ( SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) == 0 ) {
		if ( ch == SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( side ) ) return TRUE;
	}
	else {
		if ( ch == SCHEDULER_CONTROL_Get_BM_Switch_CrossCh( side ) ) return TRUE;
	}
	return FALSE;
}


BOOL Scheduler_Check_Possible_Pick_from_Other_CM_for_Switch_DIFF_BM( int side , int side2 ) { // 2014.05.30
	int i;
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_00_NOTUSE ) {
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side2 , i ) == MUF_01_USE ) {
				//
				return TRUE;
				//
			}
			//
		}
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( side2 , i ) == MUF_00_NOTUSE ) {
			//
			if ( _SCH_MODULE_Get_Mdl_Use_Flag( side , i ) == MUF_01_USE ) {
				//
				return TRUE;
				//
			}
			//
		}
		//
	}
	//
	return FALSE;
	//
}

int Scheduler_Check_Possible_Pick_from_Other_CM_for_Switch( int side , int *sider , int *sl , int *pt , int check_side , int check_pt ) {
	int i , x , Result , ret_rcm , ret_pt;
	int FM_Slot , FM_Slotx;
	int FM_Pointer , FM_Pointerx;
	int FM_HSide , FM_HSidex;
	int cpreject , retpointer1;
	int otheryes;
	int error;

	//
	// 2014.05.30
	//
	*pt = 0; // 2014.06.24
	error = 0; // 2014.06.24
	//
	if ( ( _SCH_CLUSTER_Get_PathHSideF(check_side, check_pt) / 1000000 ) != 0 ) return 0; // 2019.03.13
	//
	for ( i = BM1 ; i < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; i++ ) {
		//
		if ( SCHEDULING_ThisIs_BM_OtherChamber( i , 0 ) ) continue;
		//
		if ( !_SCH_MODULE_GET_USE_ENABLE( i , FALSE , -1 ) ) continue;
		//
		if ( !_SCH_MODULE_GROUP_FM_POSSIBLE( i , G_PICK , 0 ) ) continue;
		//
		if ( _SCH_MODULE_Get_Mdl_Use_Flag( check_side , i ) == MUF_00_NOTUSE ) {
			//
			*pt = 101; // 2014.06.24
			//
			return 0;
			//
		}
		//
	}
	//
	otheryes = FALSE;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//==================================================================================================
		if ( side == -1 ) {
			//==========================================================================================
			if ( ( check_side != -1 ) && ( check_side == i ) ) continue;
			//==========================================================================================
			_SCH_MACRO_OTHER_LOT_LOAD_WAIT( i , 3 ); // 2007.09.06
			//==========================================================================================
		}
		//==================================================================================================
		//
		if ( ( check_side != -1 ) && ( check_side != i ) ) { // 2014.05.30
			if ( Scheduler_Check_Possible_Pick_from_Other_CM_for_Switch_DIFF_BM( check_side , i ) ) continue;
		}
		//
		//==================================================================================================
		if ( _SCH_MACRO_FM_POSSIBLE_PICK_FROM_FM( i , &ret_rcm , &ret_pt , ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_BATCH_ALL ) ? 1 : 0 ) ) {
			//
			//
			if ( ( _SCH_CLUSTER_Get_PathHSideF( i , ret_pt ) / 1000000 ) != 0 ) continue; // 2019.03.13
			//
			//
			FM_Slot = 0;
			//======================================================================================
//			Result = SCHEDULING_CHECK_for_Enable_PICK_from_FM( 0 , i , &FM_Slot , &FM_Slotx , &FM_Pointer , &FM_Pointerx , &FM_HSide , &FM_HSidex , FALSE , 3 , &cpreject , &retpointer1 ); // 2008.11.01
			Result = _SCH_MACRO_FM_PICK_DATA_FROM_FM_DETAIL( 0 , FALSE , 3 , i , &FM_Slot , &FM_Slotx , &FM_Pointer , &FM_Pointerx , &retpointer1 , &FM_HSide , &FM_HSidex , &cpreject ); // 2008.11.01
			//======================================================================================
			//
			if ( ( Result >= 0 ) && ( FM_Slot > 0 ) && ( FM_Pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
				if ( _SCH_PRM_GET_INTERLOCK_FM_PICK_DENY_FOR_MDL( _SCH_CLUSTER_Get_PathIn( i , FM_Pointer ) ) == 3 ) {
					_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
					FM_Slot = 0;
					//
					error = ( i * 100 ) + 1;
					//
				}
			}
			else {
				if ( Result < 0 ) {
					FM_Slot = 0;
					//
					error = ( i * 100 ) + 2;
					//
				}
			}
			//
			if ( FM_Slot > 0 ) {
				//
				// 2014.06.25
				//
				/*
				if ( !_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( i , FM_Pointer ) ) {
					//
					_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
					FM_Slot = 0;
					//
					error = ( i * 100 ) + 3;
					//
				}
				*/
				//
				// 2014.06.25
				//
				for ( x = 0 ; x < 6 ; x++ ) {
					//
					if ( x != 0 ) Sleep(500);
					//
					if ( _SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( i , FM_Pointer ) ) break;
					//
				}
				//
				if ( x == 6 ) {
					//
					_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
					FM_Slot = 0;
					//
					error = ( i * 100 ) + 3;
					//
				}
				//
			}
			//======================================================================================
			if ( FM_Slot > 0 ) {
				if ( check_side >= 0 ) {
					if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( i , FM_Pointer , check_side , check_pt ) ) {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
						FM_Slot = 0;
						//
						error = ( i * 100 ) + 4;
						//
					}
					else { // 2008.01.16
						if ( i != check_side ) {
							if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) {
								_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
								FM_Slot = 0;
								//
								error = ( i * 100 ) + 5;
								//
							}
						}
					}
				}
			}
			//======================================================================================
			if ( FM_Slot > 0 ) {
				if ( side == -1 ) {
					*sider = i;
					*sl = FM_Slot;
					*pt = FM_Pointer;
					return 1;
				}
				else {
					if ( side == i ) {
						*sider = i;
						*sl = FM_Slot;
						*pt = FM_Pointer;
						return 1;
					}
					else {
						_SCH_MODULE_Set_FM_DoPointer( i , retpointer1 );
						otheryes = TRUE;
					}
				}
			}
			//======================================================================================
		}
		else {
			error = ( i * 100 ) + 6;
		}
	}
	if ( otheryes ) {
		//
		*pt = 102; // 2014.06.24
		*sider = error; // 2014.06.24
		//
		return 2;
	}
	//
	*pt = 103; // 2014.06.24
	*sider = error; // 2014.06.24
	//
	return 0;
}

BOOL SCHEDULER_CONTROL_Chk_SWAP_PRE_PICK_POSSIBLE_in_MIDDLESWAP( int bmd ) {
//	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) return FALSE; // 2013.10.01
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2013.10.01
		if ( !_SCH_MODULE_Need_Do_Multi_FAL() ) return FALSE; // 2013.10.01
	}
	//
	if ( _SCH_MODULE_Need_Do_Multi_FAL() ) { // 2014.03.06
		if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) {
			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
				return TRUE;
			}
		}
	}
	//
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) return FALSE;
	if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) {
		if ( ( _SCH_MODULE_Get_FM_WAFER( TA_STATION ) <= 0 ) && ( _SCH_MODULE_Get_FM_WAFER( TB_STATION ) <= 0 ) ) {
			if ( bmd == BUFFER_TM_STATION ) {
				return TRUE;
			}
		}
	}
	else { // 2008.09.01
		if ( _SCH_MODULE_Need_Do_Multi_FAL() ) { // 2008.09.01
			if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) {
				if ( bmd == BUFFER_TM_STATION ) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


//------------------------------------------------------------------------------------------
int SCHEDULER_CONTROL_Get_BUFFER_One_What_for_Switch( int mode , int Side , int Pointer , int OtherYesBM ) {
	int i , c , s , sc , lc;
	//-------------------------------------------------------------------------------
	if ( SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( Side ) != 0 ) {
		return SCHEDULER_CONTROL_Get_BM_Switch_WhatCh( Side );
	}
	//-------------------------------------------------------------------------------
	for ( i = 0 ; i < 4 ; i++ ) {
		for ( c = BM1 ; c < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; c++ ) {
			if ( c == OtherYesBM ) continue;
			if ( !_SCH_MODULE_GET_USE_ENABLE( c , FALSE , -1 ) || ( _SCH_PRM_GET_MODULE_GROUP( c ) != 0 ) ) continue;
			if      ( i == 0 ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_TM_STATION ) continue;
				if ( _SCH_MACRO_CHECK_PROCESSING_INFO( c ) > 0 ) continue;
			}
			else if ( i == 1 ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( c ) == BUFFER_TM_STATION ) continue;
			}
			else if ( i == 3 ) {
				return c;
			}
			//================================================================================
			if ( mode == BUFFER_SWITCH_MIDDLESWAP ) {
				SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( c , TRUE , &sc , &lc );
				for ( s = sc ; s <= lc ; s++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( c , s ) <= 0 ) {
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( c , s ) ) continue; // 2018.04.03
						return c;
					}
				}
			}
			else {
				for ( s = _SCH_PRM_GET_MODULE_SIZE( c ) ; s >= 1 ; s-- ) {
					if ( _SCH_MODULE_Get_BM_WAFER( c , s ) <= 0 ) {
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( c , s ) ) continue; // 2018.04.03
						return c;
					}
				}
			}
			//================================================================================
		}
	}
	return -1;
}


//------------------------------------------------------------------------------------------
//int SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( int side ) { // 2007.05.21 // 2014.06.10
int SCHEDULER_CHECKING_Another_Supply_Deny_for_FullSwap( int side , int bmc ) { // 2007.05.21 // 2014.06.10
	int i , c = FALSE , Res = -9999;
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		if ( i != side ) {
			//
			if ( bmc > 0 ) { // 2014.06.10
				if ( _SCH_MODULE_Get_Mdl_Use_Flag( i , bmc ) == MUF_00_NOTUSE ) continue;
			}
			//
			if ( !SCHEDULER_FM_Current_No_More_Supply2( i ) ) {
				//
				c = TRUE;
				//
				EnterCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
				//
				Res = SCHEDULING_CHECK_FEM_Pick_Deny_for_Switch_SingleSwap_Sub( i , -1 , TRUE );
				//
				LeaveCriticalSection( &CR_FEM_TM_DENYCHECK_STYLE_0 ); // 2014.12.26
				//
				if ( Res < 0 ) {
					break;
				}
			}
		}
	}
	if ( i == MAX_CASSETTE_SIDE ) {
		if ( c ) {
			return 1;
		}
		else {
			return -9998;
		}
		//return c;
	}
	if ( Res < 0 ) return Res;
//	return FALSE;
	return -9997;
}

BOOL SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Extend_Switch( int side , int bmc ) { // 2007.05.03
	int RemainCount , ssd , ssp , ssw , bmslot;
	if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) ) {
		//
		RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmc , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmc ) ) );
		//
		if ( RemainCount <= 0 ) return TRUE;
		if ( bmslot <= 0 ) return TRUE;
		if ( ( RemainCount % 2 ) == 0 ) return SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( side , bmc , 0 );
		if ( ssw <= 0 ) return SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( side , bmc , 0 );
		if ( SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( side , bmc , 0 ) ) return TRUE;

		for ( ssw = 0 ; ssw < MAX_PM_CHAMBER_DEPTH ; ssw++ ) {
			RemainCount = _SCH_CLUSTER_Get_PathProcessChamber( ssd , ssp , 0 , ssw );
			if ( RemainCount > 0 ) {
				if ( _SCH_MODULE_GET_USE_ENABLE( RemainCount , FALSE , ssd ) ) {
					if ( !SCHEDULER_FM_Another_No_More_Supply2( side , RemainCount , _SCH_CLUSTER_Get_ClusterIndex( ssd , ssp ) ) ) return FALSE;
				}
			}
		}
		return TRUE;
	}
	else {
		return SCHEDULING_CHECK_FM_Another_No_More_Supply_for_Switch( side , bmc , 0 );
	}
}


BOOL SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP_No_Space( int tms , int bmch ) { // 2008.07.28
	int i , sc , lc;
	//==============================================================
	for ( i = 1 ; i <= _SCH_PRM_GET_MODULE_SIZE( bmch ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_OUTWAIT_STATUS ) {
				return FALSE;
			}
		}
	}
	//==============================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmch , FALSE , &sc , &lc );
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( bmch );
	}
	//==============================================================
	for ( i = sc ; i <= lc ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i + 1 ) <= 0 ) {
				if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , i ) ) { // 2018.04.03
					if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , i+1 ) ) { // 2018.04.03
						return FALSE;
					}
				}
			}
		}
		i++;
	}
	//==============================================================
	return TRUE;
}


BOOL SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP( int tms , int bmch , BOOL outonly ) {
	int i , j , xoff , sc , lc , crm;

	if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() != 2 ) {
		if ( ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() > 0 ) && outonly ) { // 2008.07.28
			return SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Full_in_FULLSWAP_No_Space( tms , bmch );
		}
		return FALSE;
	}
	//==============================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
		SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmch , FALSE , &sc , &lc );
	}
	else {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( bmch );
	}
	//==============================================================
	if ( ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) && ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) ) {
		xoff = _SCH_PRM_GET_MODULE_SIZE( bmch ) / 2;
		crm = TRUE;
	}
	else {
		xoff = 1;
		crm = FALSE;
	}
	//==============================================================
	for ( i = sc ; i <= ( crm ? xoff : lc ) ; i++ ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) <= 0 ) {
			if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , i ) ) { // 2018.04.03
				if ( _SCH_MODULE_Get_BM_WAFER( bmch , i + xoff ) <= 0 ) {
					if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , i + xoff ) ) { // 2018.04.03
						return FALSE;
					}
				}
				else {
					if ( _SCH_MODULE_Get_BM_STATUS( bmch , i + xoff ) != BUFFER_OUTWAIT_STATUS ) {
						return FALSE;
					}
				}
			}
		}
		else {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_OUTWAIT_STATUS ) {
				return FALSE;
			}
		}
		if ( !crm ) i++;
	}
	//==============================================================
	if ( outonly && crm ) { // 2007.11.15
		for ( i = xoff + 1 ; i <= lc; i++ ) {
			if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
				if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_OUTWAIT_STATUS ) {
					return FALSE;
				}
			}
		}
	}
	//==============================================================
	//------------------------------------------------------------------------------------------------
	// 2008.01.10
	//------------------------------------------------------------------------------------------------
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		if ( _SCH_MODULE_Get_TM_TYPE( tms , i ) != SCHEDULER_MOVING_OUT ) continue;
		//
		if      ( ( ( _SCH_MODULE_Get_TM_WAFER( tms , i ) % 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( tms , i ) / 100 ) <= 0 ) ) {
			if ( crm ) {
				for ( j = sc ; j <= xoff; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( bmch , j ) <= 0 ) {
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , j ) ) continue; // 2018.04.03
						return FALSE;
					}
				}
			}
			else {
				for ( j = sc ; j <= lc; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( bmch , j ) <= 0 ) {
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , j ) ) { // 2018.04.03
							return FALSE;
						}
					}
					j++;
				}
			}
		}
		else if ( ( ( _SCH_MODULE_Get_TM_WAFER( tms , i ) / 100 ) > 0 ) && ( ( _SCH_MODULE_Get_TM_WAFER( tms , i ) % 100 ) <= 0 ) ) {
			if ( crm ) {
				for ( j = xoff + 1 ; j <= lc; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( bmch , j ) <= 0 ) {
						if ( _SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , j ) ) continue; // 2018.04.03
						return FALSE;
					}
				}
			}
			else {
				for ( j = sc + 1 ; j <= lc; j++ ) {
					if ( _SCH_MODULE_Get_BM_WAFER( bmch , j ) <= 0 ) {
						if ( !_SCH_PRM_GET_DFIM_SLOT_NOTUSE( bmch , j ) ) { // 2018.04.03
							return FALSE;
						}
					}
					j++;
				}
			}
		}
		else {
			continue;
		}
	}
	//------------------------------------------------------------------------------------------------
	return TRUE;
}


int SCHEDULER_Select_OneChamber_and_OtherBuffering( int side , int pointer , int cldepth , int ch , int ch2 ) { // 2008.09.26
	int i , m , mode = -1;
	int pmc[MAX_PM_CHAMBER_DEPTH];
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pointer < 0 ) || ( pointer >= 100 ) ) return 2;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmc[i] = FALSE;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , cldepth , i );
		//
		if      ( m > 0 ) {
			pmc[m-PM1] = TRUE;
		}
		else if ( m < 0 ) {
			pmc[-m-PM1] = TRUE;
		}
		//
		if ( ch != -1 ) {
			if ( ( m != 0 ) && ( ( m == ch ) || ( -m == ch ) ) ) {
				if      ( mode == -1 ) mode = 1;
				else if ( mode ==  0 ) mode = 0;
				else if ( mode ==  1 ) mode = 1;
				else if ( mode ==  2 ) mode = 0;
			}
		}
		if ( ch2 != -1 ) {
			if ( ( m != 0 ) && ( ( m == ch2 ) || ( -m == ch2 ) ) ) {
				if      ( mode == -1 ) mode = 2;
				else if ( mode ==  0 ) mode = 0;
				else if ( mode ==  1 ) mode = 0;
				else if ( mode ==  2 ) mode = 2;
			}
		}
	}
	//
	if ( mode == -1 ) return 3;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if      ( mode == 0 ) {
			if      ( pmc[i] && !pmc[i+1] ) {
				pmc[i] = FALSE;
			}
			else if ( !pmc[i] && pmc[i+1] ) {
				pmc[i+1] = FALSE;
			}
		}
		else if ( mode == 1 ) {
			if      ( pmc[i+1] ) {
				pmc[i+1] = FALSE;
			}
		}
		else if ( mode == 2 ) {
			if      ( pmc[i] ) {
				pmc[i] = FALSE;
			}
		}
		i++;
	}
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , cldepth , i );
		//
		if      ( m > 0 ) {
			if ( pmc[m-PM1] ) continue;
		}
		else if ( m < 0 ) {
			if ( pmc[-m-PM1] ) continue;
		}
		else {
			continue;
		}
		//
		_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , m );
		_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , cldepth , i );
	}
	//
	return 0;
}

/*
// 2013.03.26
int SCHEDULER_Select_OneChamber_and_OtherBuffering_OVERTMS( int side , int pointer , int cldepth , int tms ) { // 2013.02.07
	int i , m;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return 1;
	if ( ( pointer < 0 ) || ( pointer >= 100 ) ) return 2;
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		//
		m = _SCH_CLUSTER_Get_PathProcessChamber( side , pointer , cldepth , i );
		//
		if      ( m > 0 ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( m ) == tms ) {
				_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , m );
				_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , cldepth , i );
			}
		}
		else if ( m < 0 ) {
			if ( _SCH_PRM_GET_MODULE_GROUP( -m ) == tms ) {
				_SCH_CLUSTER_Set_PathProcessChamber( side , pointer , MAX_CLUSTER_DEPTH - 1 , i , m );
				_SCH_CLUSTER_Set_PathProcessChamber_Delete( side , pointer , cldepth , i );
			}
		}
		//
	}
	//
	return 0;
}
*/

void SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( int s , int p , int bs , int bp , int pmod ) {
//	int i;

printf( "MAKE ONE => [%d,%d][%d,%d] [%d]\n" , s , p , bs , bp , pmod );

//	return; // 2008.01.21

	if ( s  != -1 ) {
//		_SCH_CLUSTER_Select_OneChamber_and_OtherBuffering( s , p , 0 , pmod + PM1 , pmod + PM1 + 1 ); // 2008.01.21 // 2008.09.26
		SCHEDULER_Select_OneChamber_and_OtherBuffering( s , p , 0 , pmod + PM1 , pmod + PM1 + 1 ); // 2008.01.21 // 2008.09.26
	}
	if ( bs != -1 ) {
//		_SCH_CLUSTER_Select_OneChamber_and_OtherBuffering( bs , bp , 0 , pmod + PM1 , pmod + PM1 + 1 ); // 2008.01.21 // 2008.09.26
		SCHEDULER_Select_OneChamber_and_OtherBuffering( bs , bp , 0 , pmod + PM1 , pmod + PM1 + 1 ); // 2008.01.21 // 2008.09.26
	}

/*	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		if ( i != pmod ) {
			if ( s  != -1 ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( s , p , 0 , i + PM1 );
				_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( s , p , 0 , i + PM1 + 1 );
			}
			if ( bs != -1 ) {
				_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( bs , bp , 0 , i + PM1 );
				_SCH_CLUSTER_Set_PathProcessChamber_Select_This_Disable( bs , bp , 0 , i + PM1 + 1 );
			}
		}
		i++;
	}
*/
	_SCH_ONESELECT_Set_LastSelectPM( pmod + 2 );
}


BOOL SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( BOOL secondlayer , int s , int p , int bs , int bp , int *pmod , int oldpt ) {
	int i , m , si , grpone;
	int pmc[MAX_PM_CHAMBER_DEPTH];
	int bmc[MAX_PM_CHAMBER_DEPTH];

	*pmod = -1;
	//=============================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) pmc[i] = -1;
	//=============================================================================
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		m = _SCH_CLUSTER_Get_PathProcessChamber( s , p , 0 , i );
		if ( m > 0 ) pmc[ m - PM1 ] = i;
	}
	//=============================================================================
	if ( bs == -1 ) {
		//=============================================================================
		if ( oldpt == -1 ) {
			si = _SCH_ONESELECT_Get_LastSelectPM();
		}
		else {
			si = oldpt + 2;
		}
		if ( si == -1 ) si = 0;
		//=============================================================================
		for ( i = si ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( ( pmc[i] != -1 ) || ( pmc[i+1] != -1 ) ) {
				if ( secondlayer ) {
					if ( pmc[i+1] != -1 ) {
						*pmod = i;
						return TRUE;
					}
					else if ( pmc[i] != -1 ) {
						return FALSE;
					}
				}
				else {
					if ( pmc[i] != -1 ) {
						*pmod = i;
						return TRUE;
					}
					else if ( pmc[i+1] != -1 ) {
						return FALSE;
					}
				}
			}
			i++;
		}
		for ( i = 0 ; i < si ; i++ ) {
			if ( ( pmc[i] != -1 ) || ( pmc[i+1] != -1 ) ) {
				if ( secondlayer ) {
					if ( pmc[i+1] != -1 ) {
						*pmod = i;
						return TRUE;
					}
					else if ( pmc[i] != -1 ) {
						return FALSE;
					}
				}
				else {
					if ( pmc[i] != -1 ) {
						*pmod = i;
						return TRUE;
					}
					else if ( pmc[i+1] != -1 ) {
						return FALSE;
					}
				}
			}
			i++;
		}
	}
	else {
		//=============================================================================
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) bmc[i] = -1;
		//=============================================================================
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			m = _SCH_CLUSTER_Get_PathProcessChamber( bs , bp , 0 , i );
			if ( m > 0 ) bmc[ m - PM1 ] = i;
		}
		//=============================================================================
		grpone = -1;
		//=============================================================================
		for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if ( ( bmc[i] != -1 ) || ( bmc[i+1] != -1 ) ) {
				if ( grpone == -1 ) {
					grpone = i;
				}
				else {
					grpone = -2;
				}
			}
			i++;
		}
		//=============================================================================
		if ( grpone >= 0 ) {
			si = grpone;
		}
		else {
			if ( oldpt == -1 ) {
				si = _SCH_ONESELECT_Get_LastSelectPM();
			}
			else {
				si = oldpt + 2;
			}
			if ( si == -1 ) si = 0;
		}
//printf( "CHECK ONE => [%d,%d][%d,%d] [si=%d][grpone=%d]\n" , s , p , bs , bp , si , grpone );
		//=============================================================================
		for ( i = si ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
			if (
				( ( pmc[i] != -1 ) || ( pmc[i+1] != -1 ) ) &&
				( ( bmc[i] != -1 ) || ( bmc[i+1] != -1 ) )
				) {
				if ( secondlayer ) {
					if ( pmc[i+1] == -1 ) {
						i++;
						continue;
					}
					if ( bmc[i+1] == -1 ) {
						i++;
						continue;
					}
				}
				else {
					if ( pmc[i] == -1 ) {
						i++;
						continue;
					}
					if ( bmc[i] == -1 ) {
						i++;
						continue;
					}
				}
				//
				*pmod = i;
				return TRUE;
				//
			}
			//
			i++;
			//
		}
		for ( i = 0 ; i < si ; i++ ) {
			if (
				( ( pmc[i] != -1 ) || ( pmc[i+1] != -1 ) ) &&
				( ( bmc[i] != -1 ) || ( bmc[i+1] != -1 ) )
				) {
				if ( secondlayer ) {
					if ( pmc[i+1] == -1 ) {
						i++;
						continue;
					}
					if ( bmc[i+1] == -1 ) {
						i++;
						continue;
					}
				}
				else {
					if ( pmc[i] == -1 ) {
						i++;
						continue;
					}
					if ( bmc[i] == -1 ) {
						i++;
						continue;
					}
				}
				//
				*pmod = i;
				return TRUE;
				//
			}
			//
			i++;
			//
		}
		//=============================================================================
	}
	return FALSE;
}

int Double_Cross_Res;
int Double_Cross_S1;
int Double_Cross_S2;



int SCHEDULER_CONTROL_Free_Double_BM( int bmch , int i , BOOL outchk , BOOL all ) {
	if ( _SCH_MODULE_Get_BM_WAFER( bmch , i ) > 0 ) {
		if ( outchk ) {
			if ( _SCH_MODULE_Get_BM_STATUS( bmch , i ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
		}
		else {
			return FALSE;
		}
	}
	if ( all ) {
		if ( _SCH_MODULE_Get_BM_WAFER( bmch , i + 1 ) > 0 ) {
			if ( outchk ) {
				if ( _SCH_MODULE_Get_BM_STATUS( bmch , i + 1 ) != BUFFER_OUTWAIT_STATUS ) return FALSE;
			}
			else {
				return FALSE;
			}
		}
	}
	return TRUE;
}

//

int SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP_Sub( int cmpick , int bmch , int s1 , int p1 , int w1 , int s2 , int p2 , int w2 , int *ps1 , int *ps2 , int *pos , BOOL nextcross ) {
	int i , xoff , invalid , pmod1 , pmod2 , sc , lc;
	//
	if ( nextcross ) {
		sc = 1;
		lc = _SCH_PRM_GET_MODULE_SIZE( bmch );
		xoff = _SCH_PRM_GET_MODULE_SIZE( bmch ) / 2;
	}
	else {
		if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
			SCHEDULER_CONTROL_Chk_BM_SLOTRANGE_MIDDLESWAP( bmch , TRUE , &sc , &lc );
			xoff = 1;
		}
		else {
			sc = 1;
			lc = _SCH_PRM_GET_MODULE_SIZE( bmch );
			xoff = 1;
		}
		//
	}
	//
	//======================================================================================================================================================================
	*pos = 0;
	//======================================================================================================================================================================
	// group 1
	//======================================================================================================================================================================
	if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
		for ( i = sc ; i <= ( nextcross ? xoff : lc ) ; i++ ) {
			if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , TRUE ) ) {
				if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i + xoff , TRUE , TRUE ) ) {
					if ( nextcross ) {
						if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
							if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s2 , p2 , -1 , -1 , &pmod2 , pmod1 ) ) {
								//
								if ( cmpick == 1 ) {
									SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , -1 , -1 , pmod1 );
									SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s2 , p2 , -1 , -1 , pmod2 );
								}
								//
								*ps1 = i;
								*ps2 = i + 1;
								*pos = 1;
								return 0;
							}
						}
					}
					else {
						if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
							if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s2 , p2 , -1 , -1 , &pmod2 , -1 ) ) {
								if ( pmod1 == pmod2 ) {
									if ( cmpick == 1 ) {
										SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , s2 , p2 , pmod1 );
									}
									//
									*ps1 = i;
									*ps2 = i + 1;
									*pos = 1;
									return 0;
								}
							}
						}
					}
				}
			}
			i++;
		}
		//
		if ( nextcross ) {
			for ( i = xoff + 1 ; i <= lc ; i++ ) {
				if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , TRUE ) ) {
					if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i - xoff , TRUE , TRUE ) ) {
						if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
							if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s2 , p2 , -1 , -1 , &pmod2 , pmod1 ) ) {
								//
								if ( cmpick == 1 ) {
									SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , -1 , -1 , pmod1 );
									SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s2 , p2 , -1 , -1 , pmod2 );
								}
								//
								*ps1 = i;
								*ps2 = i + 1;
								*pos = 2;
								return 0;
							}
						}
					}
				}
				i++;
			}
			//==================================================================
			// 2008.01.08
			//==================================================================
			if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmch , BUFFER_OUTWAIT_STATUS ) <= 0 ) {
				if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmch , BUFFER_INWAIT_STATUS ) > 0 ) {
					for ( i = sc ; i <= xoff; i++ ) {
						if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , TRUE ) ) {
							if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i + xoff , FALSE , TRUE ) ) {
								if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
									if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s2 , p2 , -1 , -1 , &pmod2 , pmod1 ) ) {
										//
										if ( cmpick == 1 ) {
											SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , -1 , -1 , pmod1 );
											SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s2 , p2 , -1 , -1 , pmod2 );
										}
										//
										*ps1 = i;
										*ps2 = i + xoff;
										*pos = 3;
										return 0;
									}
								}
							}
						}
						i++;
					}
				}
			}
			//==================================================================
		}
	}
	else {
		//
		for ( i = sc ; i <= ( nextcross ? xoff : lc ); i++ ) {
			if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , FALSE ) ) {
				if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i + xoff , TRUE , FALSE ) ) {
					if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
						//
						if ( cmpick == 1 ) {
							SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , -1 , -1 , pmod1 );
						}
						//
						*ps1 = i;
						*ps2 = 0;
						*pos = 4;
						return 0;
					}
				}
			}
			if ( !nextcross ) i++;
		}
		//
		for ( i = xoff + 1 ; i <= lc ; i++ ) {
			if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , FALSE ) ) {
				if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i - xoff , TRUE , FALSE ) ) {
					if ( SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
						//
						if ( cmpick == 1 ) {
							SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , -1 , -1 , pmod1 );
						}
						//
						*ps1 = i;
						*ps2 = 0;
						*pos = 5;
						return 0;
					}
				}
			}
			if ( !nextcross ) i++;
		}
		//
	}
	//======================================================================================================================================================================
	// group 2
	//======================================================================================================================================================================
	if ( nextcross ) {
		if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
			for ( i = sc ; i <= xoff; i++ ) {
				if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , TRUE ) ) {
					invalid = FALSE;
					if ( !SCHEDULER_CONTROL_Free_Double_BM( bmch , i + xoff , TRUE , FALSE ) ) {
						if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i + xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i + xoff ) , &pmod1 , -1 ) ) {
							invalid = TRUE;
						}
					}
					else {
						if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
							invalid = TRUE;
						}
					}
					if ( !invalid ) {
						if ( !SCHEDULER_CONTROL_Free_Double_BM( bmch , i + xoff + 1 , TRUE , FALSE ) ) {
							if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s2 , p2 , _SCH_MODULE_Get_BM_SIDE( bmch , i + xoff + 1 ) , _SCH_MODULE_Get_BM_POINTER( bmch , i + xoff + 1 ) , &pmod2 , pmod1 ) ) {
								invalid = TRUE;
							}
						}
						else {
							if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s2 , p2 , -1 , -1 , &pmod2 , pmod1 ) ) {
								invalid = TRUE;
							}
						}
					}
					if ( !invalid ) {
						//
						if ( cmpick == 1 ) {
							SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i + xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i + xoff ) , pmod1 );
							SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s2 , p2 , _SCH_MODULE_Get_BM_SIDE( bmch , i + xoff + 1 ) , _SCH_MODULE_Get_BM_POINTER( bmch , i + xoff + 1 ) , pmod2 );
						}
						//
						*ps1 = i;
						*ps2 = i + 1;
						*pos = 6;
						return 0;
					}
				}
				i++;
			}
			//
			for ( i = xoff + 1 ; i <= lc ; i++ ) {
				if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , TRUE ) ) {
					invalid = FALSE;
					if ( !SCHEDULER_CONTROL_Free_Double_BM( bmch , i - xoff , TRUE , FALSE ) ) {
						if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i - xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i - xoff ) , &pmod1 , -1 ) ) {
							invalid = TRUE;
						}
					}
					else {
						if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s1 , p1 , -1, -1 , &pmod1 , -1 ) ) {
							invalid = TRUE;
						}
					}
					if ( !invalid ) {
						if ( !SCHEDULER_CONTROL_Free_Double_BM( bmch , i - xoff + 1 , TRUE , FALSE ) ) {
							if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s2 , p2 , _SCH_MODULE_Get_BM_SIDE( bmch , i - xoff + 1 ) , _SCH_MODULE_Get_BM_POINTER( bmch , i - xoff + 1 ) , &pmod2 , pmod1 ) ) {
								invalid = TRUE;
							}
						}
						else {
							if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s2 , p2 , -1 , -1 , &pmod2 , pmod1 ) ) {
								invalid = TRUE;
							}
						}
					}
					if ( !invalid ) {
						//
						if ( cmpick == 1 ) {
							SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i - xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i - xoff ) , pmod1 );
							SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s2 , p2 , _SCH_MODULE_Get_BM_SIDE( bmch , i - xoff + 1 ) , _SCH_MODULE_Get_BM_POINTER( bmch , i - xoff + 1 ) , pmod2 );
						}
						//
						*ps1 = i;
						*ps2 = i + 1;
						*pos = 7;
						return 0;
					}
				}
				i++;
			}
		}
	}
	//============================
	//
	for ( i = sc ; i <= ( nextcross ? xoff : lc ); i++ ) {
		if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , FALSE ) ) {
			invalid = FALSE;
			if ( !SCHEDULER_CONTROL_Free_Double_BM( bmch , i + xoff , TRUE , FALSE ) ) {
				if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i + xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i + xoff ) , &pmod1 , -1 ) ) {
					invalid = TRUE;
				}
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( FALSE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
					invalid = TRUE;
				}
			}
			if ( !invalid ) {
				//
				if ( cmpick == 1 ) {
					SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i + xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i + xoff ) , pmod1 );
				}
				//
				*ps1 = i;
				*ps2 = 0;
				*pos = 8;
				return ( ( w1 > 0 ) && ( w2 > 0 ) ) ? 2 : 0;
			}
		}
		if ( !nextcross ) i++;
	}
	//
	for ( i = xoff + 1 ; i <= lc ; i++ ) {
		if ( SCHEDULER_CONTROL_Free_Double_BM( bmch , i , FALSE , FALSE ) ) {
			invalid = FALSE;
			if ( !SCHEDULER_CONTROL_Free_Double_BM( bmch , i - xoff , TRUE , FALSE ) ) {
				if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i - xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i - xoff ) , &pmod1 , -1 ) ) {
					invalid = TRUE;
				}
			}
			else {
				if ( !SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Same_FULLSWAP( TRUE , s1 , p1 , -1 , -1 , &pmod1 , -1 ) ) {
					invalid = TRUE;
				}
			}
			if ( !invalid ) {
				//
				if ( cmpick == 1 ) {
					SCHEDULER_CONTROL_Chk_Place_BM_Double_Cross_Make_One_FULLSWAP( s1 , p1 , _SCH_MODULE_Get_BM_SIDE( bmch , i - xoff ) , _SCH_MODULE_Get_BM_POINTER( bmch , i - xoff ) , pmod1 );
				}
				//
				*ps1 = i;
				*ps2 = 0;
				*pos = 9;
				return ( ( w1 > 0 ) && ( w2 > 0 ) ) ? 2 : 0;
			}
		}
		if ( !nextcross ) i++;
	}
	//
	//======================================================================================================================================================================
	return 1;
	//======================================================================================================================================================================
}

//
int SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( int cmpick , int bmch , int s1 , int p1 , int w1 , int s2 , int p2 , int w2 , int *ps1 , int *ps2 , BOOL nextcross ) {
	int res , pos;
	//
	if ( ( w1 <= 0 ) && ( w2 > 0 ) ) { // 2016.06.08
		//
		s1 = s2;
		p1 = p2;
		w1 = w2;
		//
		w2 = 0;
		//
	}
	//
	if      ( cmpick == 1 ) {
		res = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP_Sub( cmpick , bmch , s1 , p1 , w1 , s2 , p2 , w2 , ps1 , ps2 , &pos , nextcross );
		Double_Cross_Res = res;
		Double_Cross_S1 = *ps1;
		Double_Cross_S2 = *ps2;
	}
	else if ( cmpick == 0 ) {
		*ps1 = Double_Cross_S1;
		*ps2 = Double_Cross_S2;
		res = Double_Cross_Res;
	}
	else if ( cmpick == -1 ) {
		res = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP_Sub( cmpick , bmch , s1 , p1 , w1 , s2 , p2 , w2 , ps1 , ps2 , &pos , nextcross );
	}
//	printf( "=============================================================================\n" );
//	if      ( cmpick == 1 ) {
//		printf( "=[CMPICK] [RESULT=%d] [A=%d,%d,%d] [B=%d,%d,%d] [SL=%d,%d] [POS=%d]\n" , res , w1 , s1 , p1 , w2 , s2 , p2 , *ps1 , *ps2 , pos );
//	}
//	else if ( cmpick == 0 ) {
//		printf( "=[BMPLCS] [RESULT=%d] [A=%d,%d,%d] [B=%d,%d,%d] [SL=%d,%d] [POS=%d]\n" , res , w1 , s1 , p1 , w2 , s2 , p2 , *ps1 , *ps2 , pos );
//	}
//	else {
//		printf( "=[CHECK ] [RESULT=%d] [A=%d,%d,%d] [B=%d,%d,%d] [SL=%d,%d] [POS=%d]\n" , res , w1 , s1 , p1 , w2 , s2 , p2 , *ps1 , *ps2 , pos );
//	}
//	printf( "=============================================================================\n" );

	return res;
}
//

int SCHEDULER_CONTROL_Chk_Place_BM_ImPossible_for_Double_Switch( int CHECKING_SIDE , int bmch1 , int s1 , int p1 , int w1 , int bmch2 , int s2 , int p2 , int w2 , int *log ) { // 2007.05.01
	int RemainCount , ssd , ssp , ssw , bmslot;
	//
	*log = 0;
	//
	//===============================================================================================
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() < BUFFER_SWITCH_BATCH_PART ) return 0; // 2008.01.04
	//===============================================================================================
	if ( _SCH_PRM_GET_DFIX_TM_DOUBLE_CONTROL() == 0 ) return 0;
	//===============================================================================================
	//
	if ( _SCH_MODULE_Need_Do_Multi_FAL() ) { // 2014.03.06
		if ( _SCH_PRM_GET_MODULE_SIZE( F_AL ) >= 2 ) {
//			if ( _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TA_STATION ) && _SCH_ROBOT_GET_FM_FINGER_HW_USABLE( TB_STATION ) ) { // 2016.06.26
				//
				// 2014.05.14
				//
				if ( _SCH_MODULE_Get_MFALS_WAFER(2) > 0 ) {
					ssd = _SCH_MODULE_Get_MFALS_SIDE(2);
					ssp = _SCH_MODULE_Get_MFALS_POINTER(2);
					ssw = 1;
				}
				else if ( _SCH_MODULE_Get_MFAL_WAFER() > 0 ) {
					ssd = _SCH_MODULE_Get_MFAL_SIDE();
					ssp = _SCH_MODULE_Get_MFAL_POINTER();
					ssw = 1;
				}
				else {
					ssw = 0;
				}
				//
				if ( ssw > 0 ) {
					//
					//
					//
					// 2018.09.19
					//
					if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_A_SIDE_ONLY ) return 1;
					if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_B_SIDE_ONLY ) return 1;
					//
					//
					if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , ssd , ssp ) ) {
							return 1;
						}
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s2 , p2 , ssd , ssp ) ) {
							return 2;
						}
					}
					else if ( w1 > 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , ssd , ssp ) ) {
							return 1;
						}
					}
					else if ( w2 > 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s2 , p2 , ssd , ssp ) ) {
							return 1;
						}
					}
					//
				}
				//
//				return 0; // 2016.07.06
//			} // 2016.06.26
		}
	}
	//
	//===============================================================================================
	//
	// 2016.10.06
	//
	if ( ( w1 > 0 ) && ( bmch1 > 0 ) ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch1 ) != BUFFER_FM_STATION ) return 0;
	}
	if ( ( w2 > 0 ) && ( bmch2 > 0 ) ) {
		if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch2 ) != BUFFER_FM_STATION ) return 0;
	}
	//
	//===============================================================================================
	//
	if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_MIDDLESWAP ) {
		if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
			//
			*log = 1;
			//
			RemainCount = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( 1 , bmch1 , s1 , p1 , w1 , s2 , p2 , w2 , &ssd , &ssp , FALSE );
			//
			if ( RemainCount == 1 ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch1 ) != BUFFER_TM_STATION ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmch1 , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2007.11.23
						//=====================================================================
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmch1 , -1 , TRUE , 0 , 188 );
						//=====================================================================
					}
				}
			}
			//
			return RemainCount;
		}
		else {
			if ( ( w1 > 0 ) && ( w2 > 0 ) ) {
				if ( bmch2 <= 0 ) {
					//
					*log = 11;
					//
					RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmch1 , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmch1 ) ) );
					//
					//
					//
					// 2018.09.19
					//
					if ( ssw > 0 ) {
						if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_A_SIDE_ONLY ) RemainCount = 0;
						if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_B_SIDE_ONLY ) RemainCount = 0;
					}
					//
					//
					if ( RemainCount <= 0 ) return 1; // 2007.10.29
					//
					if ( ( RemainCount % 2 ) == 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , s2 , p2 ) ) { // 2007.07.29
							return 2;
						}
						else { // 2008.01.16
							if ( s1 != s2 ) {
								if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 2;
							}
						}
					}
					else {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , ssd , ssp ) ) { // 2007.07.29
							return 1;
						}
						else { // 2008.01.16
							if ( s1 != ssd ) {
								if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 1;
							}
						}
						return 2;
					}
				}
				else {
					//
					*log = 12;
					//
					RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmch1 , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmch1 ) ) );
					//
					//
					//
					// 2018.09.19
					//
					if ( ssw > 0 ) {
						if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_A_SIDE_ONLY ) RemainCount = 0;
						if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_B_SIDE_ONLY ) RemainCount = 0;
					}
					//
					//
					if ( RemainCount <= 0 ) return 1; // 2007.10.29
					//
					if ( ( RemainCount % 2 ) != 0 ) {
						if ( ssw > 0 ) {
							if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , ssd , ssp ) ) { // 2007.07.29
								return 1;
							}
							else { // 2008.01.16
								if ( s1 != ssd ) {
									if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 1;
								}
							}
						}
					}
					//
					RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmch2 , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmch2 ) ) );
					//
					//
					//
					// 2018.09.19
					//
					if ( ssw > 0 ) {
						if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_A_SIDE_ONLY ) RemainCount = 0;
						if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_B_SIDE_ONLY ) RemainCount = 0;
					}
					//
					//
					if ( RemainCount <= 0 ) return 1; // 2007.10.29
					//
					if ( ( RemainCount % 2 ) != 0 ) {
						if ( ssw > 0 ) {
							if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s2 , p2 , ssd , ssp ) ) { // 2007.07.29
								return 2;
							}
							else { // 2008.01.16
								if ( s2 != ssd ) {
									if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 2;
								}
							}
						}
					}
					//
				}
			}
			//
			/*
			//
			// 2016.06.08
			else {
				RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmch1 , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmch1 ) ) );
				//
				if ( RemainCount <= 0 ) return 1; // 2007.10.29
				//
				if ( ( RemainCount % 2 ) != 0 ) {
					if ( ssw > 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , ssd , ssp ) ) { // 2007.07.29
							return 1;
						}
						else { // 2008.01.16
							if ( s1 != ssd ) {
								if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 1;
							}
						}
					}
				}
			}
			//
			*/
			//
			//
			// 2016.06.08
			//
			else if ( w1 > 0 ) {
				//
				*log = 13;
				//
				RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmch1 , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmch1 ) ) );
				//
				//
				//
				// 2018.09.19
				//
				if ( ssw > 0 ) {
					if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_A_SIDE_ONLY ) RemainCount = 0;
					if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_B_SIDE_ONLY ) RemainCount = 0;
				}
				//
				//
				if ( RemainCount <= 0 ) return 1; // 2007.10.29
				//
				if ( ( RemainCount % 2 ) != 0 ) {
					if ( ssw > 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s1 , p1 , ssd , ssp ) ) { // 2007.07.29
							return 1;
						}
						else { // 2008.01.16
							if ( s1 != ssd ) {
								if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 1;
							}
						}
					}
				}
			}
			else if ( w2 > 0 ) {
				//
				*log = 14;
				//
				RemainCount = SCHEDULER_CONTROL_Chk_BM_SUPPLY_INFO_IN_MIDDLESWAP( bmch1 , &ssd , &ssp , &ssw , &bmslot , SCHEDULER_CONTROL_Check_BM_ORDER_CONTROL( CHECKORDER_FOR_FM_PLACE , _SCH_PRM_GET_MODULE_BUFFER_LASTORDER( bmch1 ) ) );
				//
				//
				//
				// 2018.09.19
				//
				if ( ssw > 0 ) {
					if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_A_SIDE_ONLY ) RemainCount = 0;
					if ( _SCH_CLUSTER_Get_PathHSide( ssd , ssp ) == HANDLING_B_SIDE_ONLY ) RemainCount = 0;
				}
				//
				//
				if ( RemainCount <= 0 ) return 1; // 2007.10.29
				//
				if ( ( RemainCount % 2 ) != 0 ) {
					if ( ssw > 0 ) {
						if ( !_SCH_SUB_Check_ClusterIndex_CPJob_Same( s2 , p2 , ssd , ssp ) ) { // 2007.07.29
							return 1;
						}
						else { // 2008.01.16
							if ( s2 != ssd ) {
								if ( ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 1 ) || ( _SCH_PRM_GET_DIFF_LOT_NOTSUP_MODE() == 2 ) ) return 1;
							}
						}
					}
				}
			}
			//
			//===============================================================================================
		}
	}
	else if ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) { // 2007.11.08
		//===============================================================================================
		// 2007.11.09
		//===============================================================================================
		if ( SCHEDULER_CONTROL_Check_Process_2Module_Same_Body() == 2 ) {
			//
			*log = 21;
			//
			RemainCount = SCHEDULER_CONTROL_Chk_Place_BM_Get_Double_Cross_FULLSWAP( 1 , bmch1 , s1 , p1 , w1 , s2 , p2 , w2 , &ssd , &ssp , ( SCHEDULER_CONTROL_Check_Process_2Module_NextCrossOnly() == 2 ) && ( _SCH_PRM_GET_MODULE_BUFFER_SWITCH_SWAPPING() == BUFFER_SWITCH_FULLSWAP ) );
			//
			if ( RemainCount == 1 ) {
				if ( _SCH_MODULE_Get_BM_FULL_MODE( bmch1 ) != BUFFER_TM_STATION ) {
					if ( SCHEDULER_CONTROL_Chk_BM_HAS_COUNT( bmch1 , BUFFER_OUTWAIT_STATUS ) <= 0 ) { // 2007.11.23
						//=====================================================================
						_SCH_MACRO_SPAWN_WAITING_BM_OPERATION( BUFFER_TM_STATION , CHECKING_SIDE , bmch1 , -1 , TRUE , 0 , 188 );
						//=====================================================================
					}
				}
			}
			//
			return RemainCount;
		}
		//===============================================================================================
	}
	return 0;
}


void SCHEDULER_CONTROL_INIT_SWITCH_PART_DATA( int apmode , int side ) {
	if ( ( apmode == 0 ) || ( apmode == 3 ) ) {
		//------------------------------------------------------------------------------------
		SCHEDULING_SET_FEM_Pick_Deny_PM_LOCK_DENY_CONTROL( FALSE ); // 2007.08.10
		//------------------------------------------------------------------------------------
	}
}
