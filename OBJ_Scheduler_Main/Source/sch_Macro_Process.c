#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "Dll_Interface.h"
#include "Equip_Handling.h"
#include "User_Parameter.h"
#include "sch_PrePrcs.h"
#include "sch_prepost.h"
#include "sch_Sub.h"
#include "sch_EIL.h"
#include "iolog.h"
#include "IO_Part_Log.h"
#include "Timer_Handling.h"
#include "sch_processmonitor.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"
#include "sch_prm.h"
#include "FA_Handling.h"

#include "sch_estimate.h" // 2016.10.22

#include "INF_sch_CommonUser.h"
//================================================================================
extern Scheduling_Path	_SCH_INF_CLUSTER_DATA_AREA[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ];
//================================================================================
int SCH_MACRO_PROCESS_MOVE[MAX_CHAMBER]; // 2012.11.09
int SCH_MACRO_PROCESS_STS[MAX_CHAMBER];
int SCH_MACRO_PROCESS_RID[MAX_CHAMBER]; // 2015.04.30


int GetRealMessageFunction( int ch , int Slot ) { // 2016.04.26
	int i , index;
	//
	index = 0;
	//
	for ( i = PM1 ; i < ch ; i++ ) {
		index = index + _i_SCH_PRM_GET_MODULE_SIZE(i);
	}
	index = PM1 + index + Slot - 1;
	//
	return index;
}

int GetRealDispFunction( int ch , int *Slot ) { // 2016.04.26
	int i , x , index;
	//
	for ( x = PM1 ; x < MAX_PM_CHAMBER_DEPTH ; x++ ) {
		//
		index = 0;
		//
		for ( i = PM1 ; i < x ; i++ ) {
			index = index + _i_SCH_PRM_GET_MODULE_SIZE(i);
		}
		//
		for ( i = 1 ; i <= _i_SCH_PRM_GET_MODULE_SIZE(x) ; i++ ) {
			if ( ch == ( PM1 + index + i - 1 ) ) {
				*Slot = i;
				return x - PM1;
			}
		}
		//
	}
	//
	*Slot = 1;
	return ch-PM1;
}


int _SCH_MACRO_MAIN_PROCESS( int side , int pointer ,
//								  int ch , int rch , // 2014.11.09
								  int ch0 , int rch , // 2014.11.09
								  int slot , int pslot , int arm , int incm ,
								  char *recipe , int tmruntime , char *NextPMStr , int mintime ,
								  int prctag1 , int prctag2 , int prctag3 ,
								  int dide , int dointer , int pmstrtopt , int multiwfr , 
								  int actionoption , int optdata ) {
	// actionoption
	//		Fail  Prset wafercheck
	// 0 :    O     O       O
	// 1 :    X     O       O
	// 2 :    O     X       O
	// 3 :    X     X       O
	// 4 :    O     O       X
	// 5 :    X     O       X
	// 6 :    O     X       X
	// 7 :    X     X       X

	int dual , pathin , cutxindex , cutorder , runorder , l , l2 , dummode , dx;
	char MsgSltchar[16]; /* 2013.05.23 */
//	int ch , chp; // 2014.11.09 // 2016.04.26
	int ch , chp , chs; // 2014.11.09 // 2016.04.26
	int rid; // 2015.04.23

//	printf( "Side=%d\n" , side );
//	printf( "pointer=%d\n" , pointer );
//	printf( "ch=%d\n" , ch );
//	printf( "rch=%d\n" , rch );
//	printf( "slot=%d\n" , slot );
//	printf( "pslot=%d\n" , pslot );
//	printf( "arm=%d\n" , arm );
//	printf( "incm=%d\n" , incm );
//	printf( "recipe=%s\n" , recipe );
//	printf( "tmruntime=%d\n" , tmruntime );
//	printf( "NextPMStr=%s\n" , NextPMStr );
//	printf( "mintime=%d\n" , mintime );
//	printf( "prctag1=%d\n" , prctag1 );
//	printf( "prctag2=%d\n" , prctag2 );
//	printf( "prctag3=%d\n" , prctag3 );
//	printf( "dide=%d\n" , dide );
//	printf( "dointer=%d\n" , dointer );
//	printf( "dointer=%d\n" , pmstrtopt );
//	printf( "multiwfr=%d\n" , multiwfr );
//	printf( "actionoption=%d\n" , actionoption );
//	printf( "optdata=%d\n" , optdata );
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS Failed because invalid side(%d)\n" , side );
		return -3;
	}
	//
	//===========================================================
	//
	// 2016.11.04
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , pointer ) ) return -3;
	//
	//===========================================================
	//
	if ( ch0 >= 1000000 ) { // 2016.04.26
		chs = ch0 / 1000000;			// Slot
		chp = ( ch0 % 1000000 ) / 1000;	// ch_Param
		ch =    ch0 % 1000;				// Function
		//
		if ( ch <= 0 ) ch = GetRealMessageFunction( chp , chs ); // Real Function
		//
	}
	else {
		chs = 0;			// Slot // 2016.05.03
		if ( ch0 >= 1000 ) { // 2014.11.09
			ch = ch0 % 1000;
			chp = ch0 / 1000;
		}
		else {
			ch = ch0;
			chp = 0;
		}
	}
	//
	if ( ( ch < PM1 ) || ( ch >= AL ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS Failed because invalid ch(%d)\n" , ch );
		return -3;
	}
	//
	SCH_MACRO_PROCESS_STS[ch] = 100;
	//
	SCH_MACRO_PROCESS_RID[ch] = 0; // 2015.04.30
	//
	//============================================================================================================
	if ( _i_SCH_SYSTEM_MOVEMODE_GET( side ) != 0 ) { // 2013.09.03
		//
		_i_SCH_MODULE_SET_MOVE_MODE( ch , 1 );
		//
		if ( _i_SCH_PRM_GET_MODULE_MODE( FM ) ) {
			_i_SCH_MODULE_Inc_FM_InCount( side );
		}
		//
		_i_SCH_MODULE_Inc_TM_InCount( side );
		//
		if ( recipe == NULL ) {
			SCH_MACRO_PROCESS_STS[ch] = -101;
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
		}
		//
		if ( recipe[0] == 0 ) {
			SCH_MACRO_PROCESS_STS[ch] = -102;
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
		}
		//
		if ( recipe[0] == '?' ) {
			SCH_MACRO_PROCESS_STS[ch] = -103;
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
		}
		//
	}
	//============================================================================================================
	if ( chs > 0 ) { // 2016.04.26
		if ( EQUIP_MAIN_PROCESS_SKIP_RUN( chp ) ) {
			SCH_MACRO_PROCESS_STS[ch] = -100;
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
		}
	}
	else {
		if ( EQUIP_MAIN_PROCESS_SKIP_RUN( ch ) ) {
			SCH_MACRO_PROCESS_STS[ch] = -100;
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
		}
	}
	//
	if ( multiwfr == 0 ) { // 2017.04.18
		//
		if ( _i_SCH_MODULE_Get_PM_WAFER( ch , -1 ) <= 0 ) { // 2017.02.17
			//
//			SCH_MACRO_PROCESS_STS[ch] = -111; // 2017.04.18
			SCH_MACRO_PROCESS_STS[ch] = -110; // 2017.04.18
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
			//
		}
	}
	//
	if ( ( pointer >= 0 ) && ( pointer < MAX_CASSETTE_SLOT_SIZE ) ) {
		//
		if ( _i_SCH_CLUSTER_Get_Extra_Flag( side , pointer , 3 ) == 1 ) { // 2011.04.15
			//
			switch( SCHEDULER_CONTROL_Get_Mdl_Run_AbortWait_Flag( ch ) ) {
			case ABORTWAIT_FLAG_ABORT :
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_ABORTWAIT );
				break;
			case ABORTWAIT_FLAG_WAIT :
			case ABORTWAIT_FLAG_ABORTWAIT :
				break;
			default :
				SCHEDULER_CONTROL_Set_Mdl_Run_AbortWait_Flag( ch , ABORTWAIT_FLAG_WAIT );
				break;
			}
			//
			SCH_MACRO_PROCESS_STS[ch] = -112; // 2017.02.17
			//
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
			//
			return 0;
			//
		}
		//
		if ( incm == -1 ) {
			pathin = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
		}
		else {
			pathin = incm;
		}
	}
	else {
		pathin = 0;
	}
	//============================================================================================================
	//
	SCH_MACRO_PROCESS_STS[ch] = 101;
	//
	// 2012.12.03
	if ( ( dide >= 0 ) && ( dide < MAX_CASSETTE_SIDE ) ) {
		if ( ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( ( slot / 100 ) <= 0 ) {
				dide = -1;
			}
		}
		else {
			dide = -1;
		}
	}
	else {
		dide = -1;
	}
	//
	/*
	// 2012.12.03
	if ( dide != -1 ) {
		//
		if ( incm == -1 ) { // 2012.02.01
			//
			if ( ( slot / 100 ) > 0 ) {
				//
				if ( ( dide >= 0 ) && ( dide < MAX_CASSETTE_SIDE ) ) {
					if ( ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) {
						//
						pathin = ( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) * 100 ) + pathin;
						//
					}
				}
				//
			}
			//
		}
		//
		switch( _SCH_COMMON_PM_2MODULE_SAME_BODY() ) { // 2007.10.07
		case 1 :
			dual = 3; // 2W_2Module_Same_Body
			break;
		default :
			if ( dide != side ) {
				if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) {
					//
					if ( ( dide < 0 ) || ( dide >= MAX_CASSETTE_SIDE ) ) {
						_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS Failed because invalid dside(%d)\n" , dide );
						return -3;
					}
					//
					dual = 1; // 2W_DiffSide
				}
				else {
					dual = 0; // 1W
				}
			}
			else {
				if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) {
					//
					if ( ( dide < 0 ) || ( dide >= MAX_CASSETTE_SIDE ) ) {
						_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS Failed because invalid dside(%d) 2\n" , dide );
						return -3;
					}
					//
					if ( ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) {
						if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , 1 ) ) { // 2009.01.21
							dual = 2; // 2W_1Dummy
						}
						else {
//							dual = 0; // 2008.07.16
//							dual = 1; // 2008.07.16 // 2008.09.03
							if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 0 ) { // 2008.09.03
								dual = 0; // 2008.07.16 // 2008.09.03 // 2W_OneBody_as_1W
							}
							else {
								dual = 1; // 2008.07.16 // 2008.09.03 // 2W_SameBody
							}
						}
					}
					else {
						dual = 0; // 2W_1Dummy_as_1W
					}
				}
				else {
					dual = 0; // 1W
				}
			}
			break;
		}
	}
	else {
		dual = 0; // 1W
	}
	*/

	// 2012.12.03
	if ( dide != -1 ) {
		//
		if ( incm == -1 ) pathin = ( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) * 100 ) + pathin;
		//
		switch( _SCH_COMMON_PM_2MODULE_SAME_BODY() ) { // 2007.10.07
		case 1 :
		case 3 : // 2015.10.27
			dual = 3; // 2W_2Module_Same_Body
			break;
		default :
			if ( dide != side ) {
				if ( ( slot % 100 ) > 0 ) {
					dual = 1; // 2W_DiffSide
				}
				else {
					dual = 0; // 1W
				}
			}
			else {
				if ( ( slot % 100 ) > 0 ) {
					if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , 1 ) ) { // 2009.01.21
						dual = 2; // 2W_1Dummy
					}
					else {
						if ( _SCH_COMMON_PM_2MODULE_SAME_BODY() == 0 ) {
							dual = 0; // // 2W_OneBody_as_1W
						}
						else {
							dual = 1; // 2W_SameBody
						}
					}
				}
				else {
					dual = 0; // 1W
				}
			}
			break;
		}
	}
	else {
		dual = 0; // 1W
	}
	//
	SCH_MACRO_PROCESS_STS[ch] = 102;
	//
	cutxindex = -1;
	//
	for ( l = 0 ; l < _i_SCH_CLUSTER_Get_PathRun( side , pointer , 20 , 2 ) ; l++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 2 ) == ( ch - PM1 + 1 ) ) { // 2008.11.20
			//
			SCH_MACRO_PROCESS_STS[ch] = 103;
			//
			cutorder = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 3 ) - 1;
			//
			if ( ( cutorder < 0 ) || ( ( cutorder - _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) ) == _i_SCH_CLUSTER_Get_PathDo( side , pointer ) - 1 ) ) {
				//
				SCH_MACRO_PROCESS_STS[ch] = 104;
				//
				cutxindex = _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 4 ) - 1;
				runorder  = _i_SCH_CLUSTER_Get_PathDo( side , pointer );
				//
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 20 , 4 , 0 ); // 2010.02.01
				//
				if ( cutxindex != -1 ) {
					//
					SCH_MACRO_PROCESS_STS[ch] = 105;
					//
					if ( runorder < PATHDO_WAFER_RETURN ) {
						//
						SCH_MACRO_PROCESS_STS[ch] = 106;
						//
						strcpy( NextPMStr , "" );
						//
						l2 = _i_SCH_CLUSTER_Get_PathRange( side , pointer );
						//
						SCH_MACRO_PROCESS_STS[ch] = 107;
						//
						if ( USER_RECIPE_PROCESS_FINISH_FOR_DUMMY( FALSE , side , pointer , ch , runorder , l2 , cutorder , cutxindex , l , recipe , &_SCH_INF_CLUSTER_DATA_AREA[ side ][ pointer ] ) == 0 ) { // 2009.05.28
							//
							SCH_MACRO_PROCESS_STS[ch] = 108;
							//
							if ( _i_SCH_CLUSTER_Check_Possible_UsedPost2( side , pointer , ch , runorder - 1 , &dx ) ) { // 2010.02.01
								_i_SCH_CLUSTER_Set_PathRun( side , pointer , 20 , 4 , runorder ); // 2010.02.01
							}
							else {
								_i_SCH_CLUSTER_Set_PathDo( side , pointer , l2 );
							}
						}
						//
						SCH_MACRO_PROCESS_STS[ch] = 109;
						//
					}
				}
				//
				//
				SCH_MACRO_PROCESS_STS[ch] = 110;
				//
				if ( _i_SCH_CLUSTER_Get_PathRun( side , pointer , 21 + l , 5 ) == 1 ) {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-PROCESS1\t[S=%d][P=%d][PM%d][%d,%d,%d][PathDo=%d][Offset=%d][l=%d]\n" , side , pointer , ch - PM1 + 1 , cutorder , cutxindex , runorder , _i_SCH_CLUSTER_Get_PathDo( side , pointer ) , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) , l );
//------------------------------------------------------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + l , 2 , 0 ); // Check PM
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + l , 3 , 0 ); // Check ClusterDepth
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + l , 4 , 0 ); // Check MultiIndex
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + l , 5 , 0 );
				}
				else {
//------------------------------------------------------------------------------------------------------------------
_IO_CONSOLE_PRINTF( "FLOWCTL-PROCESS2\t[S=%d][P=%d][PM%d][%d,%d,%d][PathDo=%d][Offset=%d][l=%d]\n" , side , pointer , ch - PM1 + 1 , cutorder , cutxindex , runorder , _i_SCH_CLUSTER_Get_PathDo( side , pointer ) , _i_SCH_CLUSTER_Get_PathRun( side , pointer , 8 , 2 ) , l );
//------------------------------------------------------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PathRun( side , pointer , 21 + l , 2 , 99 );
				}
				//
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 20 , 3 , l + 1 );
				//
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 19 , 2 , ch ); // 2009.02.02
				_i_SCH_CLUSTER_Set_PathRun( side , pointer , 19 , 3 , runorder ); // 2009.02.13
				//
				break;
				//
			}
		}
		//
	}

/*
{
	FILE *fpt;
	char strdata[256];
	sprintf( strdata , "S%d_P%d_%d.log" , side , ch - PM1 + 1 , slot );
	fpt = fopen( strdata , "a" );

	fprintf( fpt , "side        = %d\n" , side );
	fprintf( fpt , "pointer     = %d\n" , pointer );
	fprintf( fpt , "ch          = PM%d\n" , ch - PM1 + 1 );
	fprintf( fpt , "rch         = %d\n" , rch );
	fprintf( fpt , "slot        = %d\n" , slot );
	fprintf( fpt , "pslot       = %d\n" , pslot );
	fprintf( fpt , "arm         = %d\n" , arm );
	fprintf( fpt , "incm        = %d\n" , incm );
	fprintf( fpt , "recipe      = %s\n" , recipe );
	fprintf( fpt , "tmruntime   = %d\n" , tmruntime );
	fprintf( fpt , "NextPMStr   = %s\n" , NextPMStr );
	fprintf( fpt , "mintime     = %d\n" , mintime );
	fprintf( fpt , "prctag1     = %d\n" , prctag1 );
	fprintf( fpt , "prctag2     = %d\n" , prctag2 );
	fprintf( fpt , "prctag3     = %d\n" , prctag3 );
	fprintf( fpt , "dide        = %d\n" , dide );
	fprintf( fpt , "dointer     = %d\n" , dointer );
	fprintf( fpt , "pmstrtopt   = %d\n" , pmstrtopt );
	fprintf( fpt , "multiwfr    = %d\n" , multiwfr );
	fprintf( fpt , "actionoption= %d\n" , actionoption );
	fprintf( fpt , "optdata     = %d\n" , optdata );

	fclose( fpt );
}
*/


	//
	//========================================================================================================
	//
	if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , ch ) == MUF_98_USE_to_DISABLE_RAND ) { // 2017.11.01
		_i_SCH_MODULE_Set_Mdl_Use_Flag( side , ch , MUF_01_USE );
	}
	//
	//========================================================================================================
	//
	SCH_MACRO_PROCESS_STS[ch] = 111;
	//
//	if ( _i_SCH_EQ_SPAWN_PROCESS( side , pointer , ( rch <= 0 ) ? ch : rch , pathin , slot , arm , recipe , 0 , tmruntime , NextPMStr , mintime , cutxindex ) ) { // 2014.11.09
//	if ( _i_SCH_EQ_SPAWN_PROCESS( side , pointer , ( ( rch <= 0 ) ? ch : rch ) + ( chp * 1000 ) , pathin , slot , arm , recipe , 0 , tmruntime , NextPMStr , mintime , cutxindex ) ) { // 2014.11.09 // 2015.04.23
//	if ( _i_SCH_EQ_SPAWN_PROCESS_SUB( side , pointer , ( ( rch <= 0 ) ? ch : rch ) + ( chp * 1000 ) , pathin , slot , arm , recipe , 0 , tmruntime , NextPMStr , mintime , cutxindex , &rid ) ) { // 2014.11.09 // 2015.04.23 // 2016.04.26
	if ( _i_SCH_EQ_SPAWN_PROCESS_SUB( side , pointer , ( ( rch <= 0 ) ? ch : rch ) + ( chp * 1000 )  + ( chs * 1000000 ) , pathin , slot , arm , recipe , 0 , tmruntime , NextPMStr , mintime , cutxindex , &rid ) ) { // 2014.11.09 // 2015.04.23 // 2016.04.26
		//
		EST_PM_PROCESS_START( ch ); // 2016.10.22
		//
		SCH_MACRO_PROCESS_RID[ch] = rid; // 2015.04.30
		//
		SCH_MACRO_PROCESS_STS[ch] = 112;
		//
		if      ( dual == 1 ) {
			//
			SCH_MACRO_PROCESS_STS[ch] = 113;
			//
			_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
			//-----------------------------------------------------------------------
			if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
				//
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
				//
				if ( chs > 0 ) { // 2016.04.26
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:111%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot % 100 , recipe , 9 , chp - PM1 + 1 , chs , slot % 100 , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
				}
				else {
	//				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:111%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , recipe , 9 , ch - PM1 + 1 , slot % 100 , recipe , pointer , optdata , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:111%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , recipe , 9 , ch - PM1 + 1 , slot % 100 , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
				}
				//
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar );
				//
				if ( chs > 0 ) { // 2016.04.26
					_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s:%d(%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:112%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot / 100 , recipe , 9 , chp - PM1 + 1 , chs , slot / 100 , recipe , dointer , ( optdata * 10000 ) + rid , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
				}
				else {
	//				_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:112%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot / 100 , recipe , dointer , optdata , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:112%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot / 100 , recipe , dointer , ( optdata * 10000 ) + rid , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
				}
				//
//				_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot % 100 , ch - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04
//				_i_SCH_LOG_TIMER_PRINTF( dide , TIMER_PM_START , slot / 100 , ch - PM1 + 1 , dointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04

				if ( chs > 0 ) { // 2016.04.26
					sprintf( MsgSltchar , "%d_%d" , chp - PM1 + 1 , chs );
					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot % 100 , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , MsgSltchar ); // 2014.02.04
					_i_SCH_LOG_TIMER_PRINTF( dide , TIMER_PM_START , slot / 100 , ch - PM1 + 1 , dointer , pmstrtopt , prctag1 , recipe , MsgSltchar ); // 2014.02.04
				}
				else {
					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot % 100 , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
					_i_SCH_LOG_TIMER_PRINTF( dide , TIMER_PM_START , slot / 100 , ch - PM1 + 1 , dointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
				}
			}
			//-----------------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) > 0 ) {
				PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , side , pointer , slot % 100 );
				PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , dide , dointer , slot / 100 );
			}
			//-----------------------------------------------------------------------
			if      ( ( actionoption == 0 ) || ( actionoption == 1 ) ) PROCESS_MONITOR_Setting2( ch , chp , chs , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , dide , dointer , FALSE ); // 2016.04.26
			else if ( ( actionoption == 4 ) || ( actionoption == 5 ) ) PROCESS_MONITOR_Setting2( ch , chp , chs , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , dide , dointer , TRUE ); // 2016.04.26
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( side , pointer , ch ); // 2013.05.06
			_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( dide , dointer , ch ); // 2013.05.06
			//-----------------------------------------------------------------------
		}
		else if ( dual == 2 ) {
			//
			SCH_MACRO_PROCESS_STS[ch] = 114;
			//
			_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
			//-----------------------------------------------------------------------
			if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
				//
				FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
				//
//				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:121%c%d%s\n"   , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , recipe , 9 , ch - PM1 + 1 , slot % 100 , recipe , pointer , optdata , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:121%c%d%s\n"   , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , recipe , 9 , ch - PM1 + 1 , slot % 100 , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
				//
				FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar );
				//
//				_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:1:D%d:%s:%d:%d:122%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot / 100 , recipe , dointer , optdata , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
				_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:1:D%d:%s:%d:%d:122%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot / 100 , recipe , dointer , ( optdata * 10000 ) + rid , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
				//
	//			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START       , slot % 100 , ch - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2007.11.07
	//			_i_SCH_LOG_TIMER_PRINTF( dide , TIMER_PM_DUMMY_START , slot / 100 , ch - PM1 + 1 , dointer , pmstrtopt , -1 , recipe , "" ); // 2007.11.07
//				_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START       , slot , ch - PM1 + 1 , pointer , ( pmstrtopt <= 0 ) ? 4 : pmstrtopt , -1 , recipe , "" ); // 2007.11.07 // 2014.02.04
				_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START       , slot , ch - PM1 + 1 , pointer , ( pmstrtopt <= 0 ) ? 4 : pmstrtopt , prctag1 , recipe , "" ); // 2007.11.07 // 2014.02.04
			}
			//-----------------------------------------------------------------------
			if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) > 0 ) PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , side , pointer , slot % 100 );
			//-----------------------------------------------------------------------
			if      ( ( actionoption == 0 ) || ( actionoption == 1 ) ) PROCESS_MONITOR_Setting_with_Dummy( ch , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , FALSE , 0 ); // 2016.04.26
			else if ( ( actionoption == 4 ) || ( actionoption == 5 ) ) PROCESS_MONITOR_Setting_with_Dummy( ch , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , TRUE , 0 ); // 2016.04.26
			//-----------------------------------------------------------------------
			_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( side , pointer , ch ); // 2013.05.06
			_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( dide , dointer , ch ); // 2013.05.06
			//-----------------------------------------------------------------------
		}
		else if ( dual == 3 ) {
			//
			SCH_MACRO_PROCESS_STS[ch] = 115;
			//
			if ( ( slot % 100 ) > 0 ) {
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
				//-----------------------------------------------------------------------
				if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
					//
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
					//
//					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:131%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , recipe , 9 , ch - PM1 + 1 , slot % 100 , recipe , pointer , optdata , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:131%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , recipe , 9 , ch - PM1 + 1 , slot % 100 , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot % 100 , MsgSltchar ); // 2015.04.23
					//
//					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot % 100 , ch - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04
					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot % 100 , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
				}
				//-----------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) > 0 ) {
					PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , side , pointer , slot % 100 );
				}
				//-----------------------------------------------------------------------
				if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) PROCESS_MONITOR_Setting_with_WfrNotCheck( ch , 0 , 0 , ( pslot <= 0 ) ? slot % 100 : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 );
				//-----------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( side , pointer , ch ); // 2013.05.06
				//-----------------------------------------------------------------------
			}
			//
			SCH_MACRO_PROCESS_STS[ch] = 116;
			//
			if ( ( slot / 100 ) > 0 ) {
				if ( ( slot % 100 ) > 0 ) {
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch + 1 );
					//-----------------------------------------------------------------------
					if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
						//
						FA_SIDE_TO_SLOT_GET_L( dide , dointer , MsgSltchar );
						//
//						_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:141%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , slot / 100 , recipe , 9 , ch + 1 - PM1 + 1 , slot / 100 , recipe , dointer , optdata , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( dide , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:141%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , slot / 100 , recipe , 9 , ch + 1 - PM1 + 1 , slot / 100 , recipe , dointer , ( optdata * 10000 ) + rid , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
						//
//						_i_SCH_LOG_TIMER_PRINTF( dide , TIMER_PM_START , slot / 100 , ch + 1 - PM1 + 1 , dointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04
						_i_SCH_LOG_TIMER_PRINTF( dide , TIMER_PM_START , slot / 100 , ch + 1 - PM1 + 1 , dointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
					}
					//-----------------------------------------------------------------------
					if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch + 1 ) > 0 ) {
//						PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , dide , dointer , slot / 100 ); // 2013.05.06
						PROCESS_INTERFACE_MODULE_APPEND_DATA( ch + 1 , dide , dointer , slot / 100 ); // 2013.05.06
					}
					//-----------------------------------------------------------------------
					if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) PROCESS_MONITOR_Setting_with_WfrNotCheck( ch + 1 , 0 , 0 , ( pslot <= 0 ) ? slot / 100 : pslot , dide , dointer , _i_SCH_CLUSTER_Get_PathIn( dide , dointer ) , prctag1 , prctag2 , recipe , prctag3 );
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( dide , dointer , ch ); // 2013.05.06
					//-----------------------------------------------------------------------
				}
				else { // 2013.05.06
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
					//-----------------------------------------------------------------------
					if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
						//
						FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
						//
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:142%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , slot / 100 , recipe , 9 , ch + 1 - PM1 + 1 , slot / 100 , recipe , pointer , optdata , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:142%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch + 1 ) , slot / 100 , recipe , 9 , ch + 1 - PM1 + 1 , slot / 100 , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot / 100 , MsgSltchar ); // 2015.04.23
						//
//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot / 100 , ch + 1 - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04
						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot / 100 , ch + 1 - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
					}
					//-----------------------------------------------------------------------
					if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) > 0 ) {
						PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , side , pointer , slot / 100 );
					}
					//-----------------------------------------------------------------------
					if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) PROCESS_MONITOR_Setting_with_WfrNotCheck( ch , 0 , 0 , ( pslot <= 0 ) ? slot : pslot , side , pointer , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , prctag1 , prctag2 , recipe , prctag3 );
					//-----------------------------------------------------------------------
					_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( side , pointer , ch ); // 2013.05.06
					//-----------------------------------------------------------------------
				}
			}
		}
		else {
			//
			SCH_MACRO_PROCESS_STS[ch] = 117;
			//
			dummode = -1; // 2009.02.02
			//
			if ( multiwfr == 2 ) {
				//
				SCH_MACRO_PROCESS_STS[ch] = 118;
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch + 1 );
				if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
					//
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
					//
//					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:151%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ( ch + 1 ) ) , slot , recipe , 9 , ( ch + 1 ) - PM1 + 1 , slot , recipe , pointer , optdata , 9 , slot , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:151%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ( ch + 1 ) ) , slot , recipe , 9 , ( ch + 1 ) - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2015.04.23
//					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ( ch + 1 ) - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04
					_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ( ch + 1 ) - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
				}
				//-----------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch + 1 ) > 0 ) PROCESS_INTERFACE_MODULE_APPEND_DATA( ch + 1 , side , pointer , slot );
				//-----------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( side , pointer , ch ); // 2013.05.06
				//-----------------------------------------------------------------------
			}
			else {
				//
				SCH_MACRO_PROCESS_STS[ch] = 119;
				//
				if ( multiwfr == 0 ) {
//					dummode = SCH_Inside_ThisIs_BM_OtherChamberD( pathin , 1 ); // 2017.09.11
					dummode = SCH_Inside_ThisIs_DummyMethod( pathin , 1 , 10 , side , pointer ); // 2017.09.11
				}
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch );
				//
				if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) {
					//
					SCH_MACRO_PROCESS_STS[ch] = 120;
					//
					FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
					//
					if ( chs > 0 ) { // 2016.04.26
						if      ( dummode == -1 ) {
							if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { // 2012.06.14
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(%d,%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:161%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot % 100 , slot / 100 , recipe , 9 , chp - PM1 + 1 , chs , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
							}
							else if ( ( slot / 100 ) > 0 ) {
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(B:%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:162%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot / 100 , recipe , 9 , chp - PM1 + 1 , chs , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
							}
							else {
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:163%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot , recipe , 9 , chp - PM1 + 1 , chs , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
							}
							//
							sprintf( MsgSltchar , "%d_%d" , chp - PM1 + 1 , chs );
							//
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , MsgSltchar ); // 2014.02.04
						}
						else if ( dummode == 0 ) {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(D%d)[%s]%cPROCESS_START PM%d:%d:D%d:%s:%d:%d:164%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot , recipe , 9 , chp - PM1 + 1 , chs , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2009.03.09 // 2015.04.23
							//
							sprintf( MsgSltchar , "%d_%d" , chp - PM1 + 1 , chs );
							//
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , prctag1 , recipe , MsgSltchar ); // 2014.02.04
						}
						else {
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(D%d-%d)[%s]%cPROCESS_START PM%d:%d:D%d-%d:%s:%d:%d:165%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , dummode , slot , recipe , 9 , chp - PM1 + 1 , chs , dummode , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , dummode , slot , MsgSltchar ); // 2007.10.02 // 2009.03.09 // 2015.04.23
							//
							sprintf( MsgSltchar , "%d_%d" , chp - PM1 + 1 , chs );
							//
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , ( dummode * 1000 ) + ( ( prctag1 >= 0 ) ? prctag1 : 0 ) , recipe , MsgSltchar ); // 2010.11.30 // 2014.02.04
						}
					}
					else {
						if      ( dummode == -1 ) {
							if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { // 2012.06.14
	//							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d,%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:161%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , optdata , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d,%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:161%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot % 100 , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
							}
							else if ( ( slot / 100 ) > 0 ) {
	//							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(B:%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:162%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , optdata , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(B:%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:162%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot / 100 , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
							}
							else {
	//							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:163%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , optdata , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:163%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2015.04.23
							}
	//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ch - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
						}
						else if ( dummode == 0 ) {
	//						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:1:D%d:%s:%d:%d:164%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , optdata , 9 , slot , MsgSltchar ); // 2007.10.02 // 2009.03.09 // 2015.04.23
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:1:D%d:%s:%d:%d:164%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2007.10.02 // 2009.03.09 // 2015.04.23
	//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , -1 , recipe , "" ); // 2014.02.04
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , prctag1 , recipe , "" ); // 2014.02.04
						}
						else {
	//						_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(D%d-%d)[%s]%cPROCESS_START PM%d:1:D%d-%d:%s:%d:%d:165%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , slot , recipe , 9 , ch - PM1 + 1 , dummode , slot , recipe , pointer , optdata , 9 , dummode , slot , MsgSltchar ); // 2007.10.02 // 2009.03.09 // 2015.04.23
							_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(D%d-%d)[%s]%cPROCESS_START PM%d:1:D%d-%d:%s:%d:%d:165%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , slot , recipe , 9 , ch - PM1 + 1 , dummode , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , dummode , slot , MsgSltchar ); // 2007.10.02 // 2009.03.09 // 2015.04.23
	//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , -1 , recipe , "" ); // 2010.11.30
	//						_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , dummode , recipe , "" ); // 2010.11.30 // 2014.02.04
							_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( slot * 100 ) , ch - PM1 + 1 , pointer , 5 , ( dummode * 1000 ) + ( ( prctag1 >= 0 ) ? prctag1 : 0 ) , recipe , "" ); // 2010.11.30 // 2014.02.04
						}
					}
				}
				//-----------------------------------------------------------------------
				if ( _i_SCH_PRM_GET_MODULE_INTERFACE( ch ) > 0 ) PROCESS_INTERFACE_MODULE_APPEND_DATA( ch , side , pointer , slot );
				//-----------------------------------------------------------------------
				_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( side , pointer , ch ); // 2013.05.06
				//
				if ( ( ( slot / 100 ) > 0 ) && ( ( slot % 100 ) > 0 ) ) { // 2013.05.06
					//
					if ( ( dide >= 0 ) && ( dide < MAX_CASSETTE_SIDE ) && ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) {
						_i_SCH_CLUSTER_Set_PrcsID_LastPrcsGroup( dide , dointer , ch );
					}
					//
				}
				//-----------------------------------------------------------------------
			}
			//-----------------------------------------------------------------------
			if      ( ( actionoption == 0 ) || ( actionoption == 1 ) ) {
				//
				SCH_MACRO_PROCESS_STS[ch] = 121;
				//
				if ( dummode >= 0 ) {
					PROCESS_MONITOR_Setting_with_Dummy( ch , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , FALSE , dummode + 1 ); // 2009.03.09
				}
				else {
					if ( ( dide >= 0 ) && ( dide < MAX_CASSETTE_SIDE ) && ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) { // 2009.09.24
						PROCESS_MONITOR_Setting2_One( ch , chp , chs , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , dide , dointer , FALSE ); // 2011.12.29 // 2012.06.12
//						PROCESS_MONITOR_Setting2( ch , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , dide , dointer , FALSE ); // 2011.12.29 // 2012.06.12
					}
					else {
//						PROCESS_MONITOR_Setting1( ch , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , FALSE ); // 2009.02.02 // 2014.02.14
						PROCESS_MONITOR_Setting1( ch , chp , chs , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , FALSE , ( _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) ); // 2009.02.02 // 2014.02.14
					}
				}
			}
			else if ( ( actionoption == 4 ) || ( actionoption == 5 ) ) {
				//
				SCH_MACRO_PROCESS_STS[ch] = 122;
				//
				PROCESS_MONITOR_Setting_with_WfrNotCheck( ch , chp , chs , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 ); // 2009.02.02
			}
			//-----------------------------------------------------------------------
			if ( ( multiwfr == 1 ) || ( multiwfr == 2 ) ) {
				//
				SCH_MACRO_PROCESS_STS[ch] = 123;
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 0 , ch + 1 );
				if      ( ( actionoption == 0 ) || ( actionoption == 1 ) ) {
					if ( ( dide >= 0 ) && ( dide < MAX_CASSETTE_SIDE ) && ( dointer >= 0 ) && ( dointer < MAX_CASSETTE_SLOT_SIZE ) ) { // 2009.09.24
						PROCESS_MONITOR_Setting2_One( ch + 1 , 0 , 0 , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , dide , dointer , FALSE );
					}
					else {
//						PROCESS_MONITOR_Setting1( ch + 1 , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , FALSE ); // 2014.02.13 
						PROCESS_MONITOR_Setting1( ch + 1 , 0 , 0 , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 , FALSE , ( _i_SCH_MODULE_Get_PM_WAFER( ch + 1 , 0 ) <= 0 ) ); // 2014.02.13 
					}
				}
				else if ( ( actionoption == 4 ) || ( actionoption == 5 ) ) {
					PROCESS_MONITOR_Setting_with_WfrNotCheck( ch + 1 , 0 , 0 , ( pslot <= 0 ) ? slot : pslot , side , pointer , pathin , prctag1 , prctag2 , recipe , prctag3 );
				}
			}
			//
		}
		//
		SCH_MACRO_PROCESS_STS[ch] = -111;
		//
		_SCH_COMMON_MESSAGE_CONTROL_FOR_USER( TRUE , 41 , side , pointer , ch , rch , slot , pslot , arm , incm , dide , dointer , 0 , 0 , 0 , recipe , NextPMStr , NULL ); // 2012.12.04
		//
		//
		if ( ( actionoption == 0 ) || ( actionoption == 1 ) || ( actionoption == 4 ) || ( actionoption == 5 ) ) { // 2018.10.20
			_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 );
		}
		//
		return 1;
	}
	else {
		//
		SCH_MACRO_PROCESS_RID[ch] = rid; // 2015.04.30
		//
		SCH_MACRO_PROCESS_STS[ch] = 124;
		//
		if ( ( multiwfr == 0 ) && ( ( actionoption == 0 ) || ( actionoption == 2 ) || ( actionoption == 4 ) || ( actionoption == 6 ) ) ) {
			//
			FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
			//
			if ( chs > 0 ) { // 2016.04.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s:%d(%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:171%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot , recipe , 9 , chp - PM1 + 1 , chs , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2015.04.23
			}
			else {
	//			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:171%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata , 9 , slot , MsgSltchar ); // 2015.04.23
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:1:%d:%s:%d:%d:171%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2015.04.23
			}
		}
		//---------------------------------------------------------
//		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ch - PM1 + 1 , pointer , pmstrtopt , -1 , recipe , "" ); // 2014.02.04

		if ( chs > 0 ) { // 2016.04.26
			//
			sprintf( MsgSltchar , "%d_%d" , chp - PM1 + 1 , chs );
			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , MsgSltchar ); // 2014.02.04
			//
		}
		else {
			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , slot , ch - PM1 + 1 , pointer , pmstrtopt , prctag1 , recipe , "" ); // 2014.02.04
		}
		//-----------------------------------------------------------------------
		PROCESS_MONITOR_Set_Finish_Status( side , ch , -1 );
		//---------------------------------------------------------
		if ( ( multiwfr == 0 ) && ( ( actionoption == 0 ) || ( actionoption == 2 ) || ( actionoption == 4 ) || ( actionoption == 6 ) ) ) LOG_MTL_ADD_FAIL_COUNT( ch , 1 );
		//---------------------------------------------------------
		_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_END , ch - PM1 + 1 , slot , pointer , 1 , 0 , recipe , "" );
		//---------------------------------------------------------
		if ( ( multiwfr == 0 ) && ( ( actionoption == 0 ) || ( actionoption == 2 ) || ( actionoption == 4 ) || ( actionoption == 6 ) ) ) {
			//
			FA_SIDE_TO_SLOT_GET_L( side , pointer , MsgSltchar );
			//
			if ( chs > 0 ) { // 2016.04.26
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Recipe Fail at %s:%d(%d)[%s]%cPROCESS_FAIL PM%d:%d:%d:%s:%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( chp ) , chs , slot , recipe , 9 , chp - PM1 + 1 , chs , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2015.04.23
			}
			else {
	//			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Recipe Fail at %s(%d)[%s]%cPROCESS_FAIL PM%d:1:%d:%s:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata , 9 , slot , MsgSltchar ); // 2015.04.23
				_i_SCH_LOG_LOT_PRINTF( side , "PM Process Recipe Fail at %s(%d)[%s]%cPROCESS_FAIL PM%d:1:%d:%s:%d:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , pointer , ( optdata * 10000 ) + rid , 9 , slot , MsgSltchar ); // 2015.04.23
			}
		}
		//---------------------------------------------------------
		//
		SCH_MACRO_PROCESS_STS[ch] = -112;
		//
		_SCH_COMMON_MESSAGE_CONTROL_FOR_USER( TRUE , 42 , side , pointer , ch , rch , slot , pslot , arm , incm , dide , dointer , 0 , 0 , 0 , recipe , NextPMStr , NULL ); // 2012.12.04
		//
		_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
		//
		return -1;
	}
}

//int _SCH_MACRO_MAIN_PROCESS_PART_SIMPLE( int side , int pointer , int pathdo , int ch , int arm , int optdata ) { // 2014.11.09
int _SCH_MACRO_MAIN_PROCESS_PART_SIMPLE( int side , int pointer , int pathdo , int ch0 , int arm , int optdata ) { // 2014.11.09
	char NextPM[256];
	int order , wid , rpd;
//	int ch , chp; // 2014.11.09 // 2016.04.26
	int ch , chp , chs; // 2014.11.09 // 2016.04.26
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_SIMPLE Failed because invalid side(%d)\n" , side );
		return -3;
	}
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_SIMPLE Failed because invalid pointer(%d)\n" , pointer );
		return -3;
	}
	//
	if ( ch0 >= 1000000 ) { // 2016.04.26
		chs = ch0 / 1000000;			// slot
		chp = ( ch0 % 1000000 ) / 1000; // ch_param
		ch =    ch0 % 1000;				// ch_function
	}
	else {
		chs = 0; // 2016.04.26
		if ( ch0 >= 1000 ) { // 2014.11.09
			ch = ch0 % 1000;
			chp = ch0 / 1000;
		}
		else {
			ch = ch0;
			chp = 0;
		}
	}
	//
	if ( chs > 0 ) { // 2016.04.26
		if ( ( chp < PM1 ) || ( chp >= AL ) ) {
			_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_SIMPLE Failed because invalid ch(%d)\n" , chp );
			return -3;
		}
	}
	else {
		if ( ( ch < PM1 ) || ( ch >= AL ) ) {
			_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_SIMPLE Failed because invalid ch(%d)\n" , ch );
			return -3;
		}
	}
	//
	if ( pathdo == -1 ) { // 2008.04.01
		rpd = _i_SCH_CLUSTER_Get_PathDo( side , pointer ) - 1; // 2008.04.01
	} // 2008.04.01
	else { // 2008.04.01
		rpd = pathdo; // 2008.04.01
	} // 2008.04.01
	//
//	if ( ( rpd < 0 ) || ( rpd >= ( PATHDO_WAFER_RETURN - 1 ) ) ) return 2; // 2008.04.01 // 2008.09.30
	if ( ( rpd < 0 ) || ( rpd >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_SIMPLE Failed because invalid rpd(%d)\n" , rpd );
		return -2; // 2008.04.01 // 2008.09.30
	}
	//
	_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_RUNNING2 );
	_i_SCH_CLUSTER_Get_Next_PM_String( side , pointer , rpd + 1 , NextPM , 255 );
	//
	wid = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
	//
	if ( chs > 0 ) { // 2016.04.26
		order = _i_SCH_CLUSTER_Get_PathOrder( side , pointer , rpd , chp );
	}
	else {
		order = _i_SCH_CLUSTER_Get_PathOrder( side , pointer , rpd , ch );
	}
	//
	return _SCH_MACRO_MAIN_PROCESS( side , pointer ,
//		  ch , -1 , // 2014.11.09
//		  ch + ( chp * 1000 ) , -1 , // 2014.11.09 // 2016.04.26
		  ch + ( chp * 1000 ) + ( chs * 1000000 ) , -1 , // 2014.11.09 // 2016.04.26
//		  wid , -1 , arm , -1 , // 2014.02.13
		  wid , -1 , ( arm < 0 ) ? 0 : arm , -1 , // 2014.02.13
		  _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) ,
//		  _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , arm ) , NextPM , PROCESS_DEFAULT_MINTIME , // 2014.02.13
		  ( arm < 0 ) ? 0 : _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , arm ) , NextPM , PROCESS_DEFAULT_MINTIME , // 2014.02.13
//		  rpd , order , ( rpd > _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , // 2008.07.13
		  rpd , order , ( ( rpd + 1 ) >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , // 2008.07.13
		  -1 , -1 , -1 , 0 ,
		  0 , optdata );
}


//int _SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( int side , int pointer , int pathdo , int ch , int arm , int firstnotuse_Option , int optdata ) { // 2014.11.09
int _SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE( int side , int pointer , int pathdo , int ch0 , int arm , int firstnotuse_Option , int optdata ) { // 2014.11.09
	char NextPM[256];
	int pt2 , order , wid , rpd , firstnotuse , Option , pin , sc , pc , wc , Res;
	int wfrSideList[MAX_CASSETTE_SIDE] , wfrSideListChk[MAX_CASSETTE_SIDE] , SideID , PtID, WfrID , FirstWfrID; // 2019.01.22
	char MsgSltchar[16]; /* 2013.05.23 */
	BOOL WfrNotCheck; // 2014.02.13
	int ch , chp; // 2014.11.09
	BOOL firstmsg; // 2017.01.11
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE Failed because invalid side(%d)\n" , side );
		return -3;
	}
	if ( ( pointer < 0 ) || ( pointer >= MAX_CASSETTE_SLOT_SIZE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE Failed because invalid pointer(%d)\n" , pointer );
		return -3;
	}
	//
	if ( ch0 >= 1000 ) { // 2014.11.09
		ch = ch0 % 1000;
		chp = ch0 / 1000;
	}
	else {
		ch = ch0;
		chp = 0;
	}
	//
	if ( ( ch < PM1 ) || ( ch >= AL ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE Failed because invalid ch(%d)\n" , ch );
		return -3;
	}
	//
	if ( pathdo == -1 ) { // 2008.04.01
		rpd = _i_SCH_CLUSTER_Get_PathDo( side , pointer ) - 1; // 2008.04.01
	} // 2008.04.01
	else { // 2008.04.01
		rpd = pathdo; // 2008.04.01
	} // 2008.04.01
	//
//	if ( ( rpd < 0 ) || ( rpd >= ( PATHDO_WAFER_RETURN - 1 ) ) ) return 2; // 2008.04.01 // 2008.09.30
	if ( ( rpd < 0 ) || ( rpd >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_MAIN_PROCESS_PART_MULTIPLE Failed because invalid rpd(%d)\n" , rpd );
		return -2; // 2008.04.01 // 2008.09.30
	}
	//
	if ( firstnotuse_Option >= 0 ) { // 2013.03.11
		Option = firstnotuse_Option / 10; // 2012.11.29
		//
		if ( !( firstnotuse_Option % 10 ) ) { // 2010.05.08
			_i_SCH_CLUSTER_Set_PathStatus( side , pointer , SCHEDULER_RUNNING2 );
		}
	//	for ( pt2 = 1 ; pt2 <= 5 ; pt2++ ) { // 2009.08.06
		for ( pt2 = 1 ; pt2 < MAX_PM_SLOT_DEPTH ; pt2++ ) { // 2009.08.06
			if ( _i_SCH_MODULE_Get_PM_WAFER( ch , pt2 ) > 0 ) {
				_i_SCH_CLUSTER_Set_PathStatus( _i_SCH_MODULE_Get_PM_SIDE( ch , pt2 ) , _i_SCH_MODULE_Get_PM_POINTER( ch , pt2 ) , SCHEDULER_RUNNING2 );
			}
		}
	}
	else { // 2013.03.11
		//
		Option = -1;
		//
		for ( pt2 = 0 ; pt2 < MAX_PM_SLOT_DEPTH ; pt2++ ) { // 2009.08.06
			if ( _i_SCH_MODULE_Get_PM_WAFER( ch , pt2 ) > 0 ) {
				_i_SCH_CLUSTER_Set_PathStatus( _i_SCH_MODULE_Get_PM_SIDE( ch , pt2 ) , _i_SCH_MODULE_Get_PM_POINTER( ch , pt2 ) , SCHEDULER_RUNNING2 );
			}
		}
	}
	//
	_i_SCH_CLUSTER_Get_Next_PM_String( side , pointer , rpd + 1 , NextPM , 255 );
	//
	order = _i_SCH_CLUSTER_Get_PathOrder( side , pointer , rpd , ch );
	wid = _i_SCH_CLUSTER_Get_SlotIn( side , pointer );
	//
/*
// 2013.05.13
	if ( _SCH_MACRO_MAIN_PROCESS( side , pointer ,
		  ch , -1 ,
		  wid , -1 , arm , -1 ,
		  _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) ,
		  _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , arm ) , NextPM , PROCESS_DEFAULT_MINTIME ,
//		  rpd , order , ( rpd > _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , // 2008.07.13
		  rpd , order , ( ( rpd + 1 ) >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , // 2008.07.13
		  -1 , -1 , -1 , 0 ,
		  2 , optdata ) == -1 ) return -1;
*/
// 2013.05.13
	Res = _SCH_MACRO_MAIN_PROCESS( side , pointer ,
//		  ch , -1 , // 2014.11.09
		  ch + ( chp * 1000 ) , -1 , // 2014.11.09
//		  wid , -1 , arm , -1 , // 2014.02.13
		  wid , -1 , ( arm < 0 ) ? 0 : arm , -1 , // 2014.02.13
		  _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) ,
//		  _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , arm ) , NextPM , PROCESS_DEFAULT_MINTIME , // 2014.02.13
		  ( arm < 0 ) ? 0 : _i_SCH_TIMER_GET_ROBOT_TIME_RUNNING( 0 , arm ) , NextPM , PROCESS_DEFAULT_MINTIME , // 2014.02.13
		  rpd , order , ( ( rpd + 1 ) >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , // 2008.07.13
		  -1 , -1 , -1 , 0 ,
		  2 , optdata );
	//
	if ( Res == -1 ) return -1;
	if ( Res == 0 ) return 0;
	//
	/*
	// 2013.03.07
	if ( _i_SCH_MODULE_Get_PM_WAFER( ch , 0 ) <= 0 ) { // 2010.06.10
		//
		firstnotuse = 1;
		//
	}
	else {
		if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , 1 ) ) { // 2010.05.08
			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(D%d)[%s]%cPROCESS_START PM%d:D%d:%s:%d:211%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , optdata , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side , pointer ) );
//			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( wid * 100 ) + wid , ch - PM1 + 1 , pointer , 99 , -1 , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , "" ); // 2010.07.08
			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( wid * 100 ) + wid , ch - PM1 + 1 , pointer , 5 , -1 , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , "" ); // 2010.07.08
			//
			firstnotuse = 2; // 2010.05.08
			//
		}
		else {
			//
			widT = wid;
			//
			switch ( Option ) {
			case 1 : // 2012.11.29
				pin = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
				if ( ( pin >= CM1 ) && ( pin < PM1 ) ) {
					widT = pin - CM1 + 1;
				}
				break;
			}
			//
			_i_SCH_LOG_LOT_PRINTF( side , "PM Process Start at %s(%d)[%s]%cPROCESS_START PM%d:%d:%s:%d:212%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , optdata , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side , pointer ) );
			_i_SCH_LOG_TIMER_PRINTF( side , TIMER_PM_START , ( wid * 100 ) + widT , ch - PM1 + 1 , pointer , 99 , -1 , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , "" );
			//
			firstnotuse = ( firstnotuse_Option % 10 ); // 2010.05.08
			//
		}
	}
	*/
	//
	WfrNotCheck = TRUE; // 2014.02.13
	firstmsg = FALSE; // 2017.01.11
	//
	sc = side; // 2013.03.11
	pc = pointer; // 2013.03.11
	wc = wid; // 2013.03.11
	//
	for ( pt2 = 0 ; pt2 < MAX_CASSETTE_SIDE ; pt2++ ) {	wfrSideList[pt2] = wid;	wfrSideListChk[pt2] = FALSE;	} // 2019.01.22
	//
//	for ( pt2 = 1 ; pt2 <= 5 ; pt2++ ) { // 2009.08.06
//	for ( pt2 = 1 ; pt2 < MAX_PM_SLOT_DEPTH ; pt2++ ) { // 2009.08.06
	for ( pt2 = 0 ; pt2 < MAX_PM_SLOT_DEPTH ; pt2++ ) { // 2009.08.06 // 2013.03.07
		//
		if ( _i_SCH_MODULE_Get_PM_WAFER( ch , pt2 ) > 0 ) {
			//
			WfrNotCheck = FALSE; // 2014.02.13
			//
			// 2019.01.22
			//
			SideID = _i_SCH_MODULE_Get_PM_SIDE( ch , pt2 );
			PtID = _i_SCH_MODULE_Get_PM_POINTER( ch , pt2 );
			WfrID = _i_SCH_MODULE_Get_PM_WAFER( ch , pt2 );
			//
			if ( sc == SideID ) {
				FirstWfrID = wid;
			}
			else {
				//
				if ( ( SideID >= 0 ) && ( SideID < MAX_CASSETTE_SIDE ) ) {
					//
					if ( !wfrSideListChk[SideID] ) {
						wfrSideListChk[SideID] = TRUE;
						wfrSideList[SideID] = WfrID;
					}
					//
					FirstWfrID = wfrSideList[SideID];
					//
				}
				else {
					FirstWfrID = wid;
				}
			}
			//
			//
			//
			if ( pt2 == 0 ) { // 2013.03.11
				sc = SideID;
				pc = PtID;
				wc = _i_SCH_CLUSTER_Get_SlotIn( sc , pc );
			}
			//
			if ( SCH_Inside_ThisIs_BM_OtherChamber( _i_SCH_CLUSTER_Get_PathIn( SideID , PtID ) , 1 ) ) { /* 2009.01.21 */
				//
				FA_SIDE_TO_SLOT_GET_L( SideID , PtID , MsgSltchar );
				//
				_i_SCH_LOG_LOT_PRINTF( SideID , "PM Process Start at %s(%d:D%d)[%s]%cPROCESS_START PM%d:%d:D%d:%s:%d:%d:221:%d%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , pt2+1 , WfrID , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , 9 , ch - PM1 + 1 , pt2+1 , WfrID , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , PtID , ( optdata * 10000 ) + SCH_MACRO_PROCESS_RID[ch] , _i_SCH_MODULE_Get_PM_STATUS(ch,pt2) , 9 , WfrID , MsgSltchar );
				//
				_i_SCH_LOG_TIMER_PRINTF( SideID , TIMER_PM_START , FirstWfrID + ( WfrID * 100 ) , ch - PM1 + 1 , PtID , ((pt2+1)*100) + 5 , rpd , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , "" ); // 2014.02.04
				//
				if ( pt2 == 0 ) firstnotuse = 2; // 2013.03.07
				//
			}
			else {
				//
				switch ( Option ) {
				case 1 : // 2012.11.29
					pin = _i_SCH_CLUSTER_Get_PathIn( SideID , PtID );
					if ( ( pin >= CM1 ) && ( pin < PM1 ) ) {
						FirstWfrID = pin - CM1 + 1;
					}
					break;
				}
				//
				FA_SIDE_TO_SLOT_GET_L( SideID , PtID , MsgSltchar );
				//
				_i_SCH_LOG_LOT_PRINTF( SideID , "PM Process Start at %s(%d:%d)[%s]%cPROCESS_START PM%d:%d:%d:%s:%d:%d:222:%d%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , pt2+1 , WfrID , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , 9 , ch - PM1 + 1 , pt2+1 , WfrID , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , PtID , ( optdata * 10000 ) + SCH_MACRO_PROCESS_RID[ch] , _i_SCH_MODULE_Get_PM_STATUS(ch,pt2) , 9 , WfrID , MsgSltchar );
				//
				if ( !firstmsg || ( ( _i_SCH_PRM_GET_FA_SINGLE_CASS_MULTI_RUN() / 2 ) == 0 ) ) {// 2017.01.11
					//
					firstmsg = TRUE;
					//
					_i_SCH_LOG_TIMER_PRINTF( SideID , TIMER_PM_START , FirstWfrID + ( _i_SCH_MODULE_Get_PM_WAFER( ch , pt2 ) * 100 ) , ch - PM1 + 1 , PtID , ((pt2+1)*100) + 99 , rpd , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , "" ); // 2014.02.04
					//
				}
				//
				if ( pt2 == 0 ) {
					if ( firstnotuse_Option < 0 ) {
						firstnotuse = 0; // 2013.03.11
					}
					else {
						firstnotuse = ( firstnotuse_Option % 10 ); // 2013.03.07
					}
				}
				//
			}
		}
		else { // 2013.03.07
			//
			if ( pt2 == 0 ) firstnotuse = 1;
			//
		}
	}
//	PROCESS_MONITOR_SettingM( ch , wid , side , pointer , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , ( rpd > _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , firstnotuse , // 2008.08.28
//	PROCESS_MONITOR_SettingM( ch , wid , side , pointer , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , ( ( rpd + 1 ) >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , firstnotuse ); // 2008.08.28 // 2013.03.11
	PROCESS_MONITOR_SettingM( ch , ( firstnotuse == 2 ) ? ( wc * 100 ) + wid : wid , sc , pc , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pointer , rpd , order , 0 ) , ( ( rpd + 1 ) >= _i_SCH_CLUSTER_Get_PathRange( side , pointer ) ) , ( ( firstnotuse == 1 ) && ( WfrNotCheck ) ) ? 0 : firstnotuse , WfrNotCheck ); // 2008.08.28 // 2013.03.11 // 2014.02.13
	//
	_i_SCH_MODULE_Set_PM_PICKLOCK( ch , -1 , 0 ); // 2018.10.20
	//
	return 1;
}

void _SCH_MACRO_PROCESS_PART_TAG_SETTING( int ch , int status , int side , int pointer , int no1 , int no2 , char *recipe , int last ) {
//	PROCESS_MONITOR_Setting1( ch , status , side , pointer , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , no1 , no2 , recipe , last , FALSE ); // 2014.02.13
	PROCESS_MONITOR_Setting1( ch , 0 , 0 , status , side , pointer , _i_SCH_CLUSTER_Get_PathIn( side , pointer ) , no1 , no2 , recipe , last , FALSE , FALSE ); // 2014.02.13
}

//void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch ); // 2015.08.25 // 2018.06.22
void Scheduler_PreCheck_for_ProcessRunning_Part( int CHECKING_SIDE , int ch , char *recipe , int mode ); // 2015.08.25 // 2018.06.22

int _SCH_MACRO_PROCESS_APPEND_OPERATION( int side , int pt , int ch , int pathin , int wid , int arm , char *rcp , int mode , int puttime , char *nextpm , int mintime , int mindex , BOOL forceappendcheck , int mon_status , int mon_no1 , int mon_no2 , int mon_last , BOOL mon_append ) { // 2012.12.10
	//
	if ( forceappendcheck || ( EQUIP_STATUS_PROCESS( side , ch ) == SYS_RUNNING ) ) {
		//
		if ( PROCESS_MONITOR_SPAWN_APPENDING( side , pt , ch , pathin , wid , arm , rcp , mode , puttime , nextpm , mintime , mindex , mon_status , mon_no1 , mon_no2 , mon_last , mon_append ) ) {
			return 2;
		}
		else {
			//
			if ( forceappendcheck || ( EQUIP_STATUS_PROCESS( side , ch ) == SYS_RUNNING ) ) {
				//
//				Scheduler_PreCheck_for_ProcessRunning_Part( side , ch ); // 2015.08.25 // 2018.06.22
				Scheduler_PreCheck_for_ProcessRunning_Part( side , ch , rcp , 0 ); // 2015.08.25 // 2018.06.22
				//
				if ( _i_SCH_EQ_SPAWN_EVENT_PROCESS( side , pt , ch , pathin , wid , arm , rcp , mode , puttime , nextpm , mintime , mindex ) ) {
					return 1;
				}
				//
			}
			//
		}
		//
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	if ( _i_SCH_EQ_SPAWN_PROCESS( side , pt , ch , pathin , wid , arm , rcp , mode , puttime , nextpm , mintime , mindex ) ) {
		return 0;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	//
	return -1;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------
}



int _SCH_MACRO_LOTPREPOST_PROCESS_OPERATION( int side ,
								 int ch ,
								 int incm ,
								 int slot ,
								 int arm ,
								 char *recipe ,
								 int modeid ,
								 int puttime , char *nextpm , int mintime ,
								 int mode , int optdata ) {
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION Failed because invalid side(%d)\n" , side );
		return -3;
	}
	if ( ( ch < PM1 ) || ( ch >= AL ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION Failed because invalid ch(%d)\n" , ch );
		return -3;
	}
	//
	if ( EQUIP_STATUS_PROCESS( side , ch ) == SYS_RUNNING ) { // 2010.02.02
		//
		if ( PROCESS_MONITOR_SPAWN_APPENDING( side , -1 , ch , incm , slot , arm , recipe , modeid , puttime , nextpm , mintime , -1 , ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) ? PROCESS_INDICATOR_PRE : PROCESS_INDICATOR_POST , -1 , -1 , 1 , TRUE ) ) { // 2011.03.02
			return 2;
		}
		else {
			//
			if ( EQUIP_STATUS_PROCESS( side , ch ) == SYS_RUNNING ) { // 2011.03.02
				//
				if ( _i_SCH_EQ_SPAWN_EVENT_PROCESS( side , -1 , ch , incm , slot , arm , recipe , modeid , puttime , nextpm , mintime , -1 ) ) {
					//-----------------------------------------------------------------------------------------------------------------------------------
					if ( ( mode == 0 ) || ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) ) {
						if ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) { // pre
							if ( slot <= 0 ) {
//								_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Append at %s[%s]%cPROCESS_PRE_START PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Append at %s[%s]%cPROCESS_PRE_START PM%d:::%s::%d:101\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
							}
							else {
//								_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Append at %s(D%d)[%s]%cPROCESS_PRE_START PM%d:D%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Append at %s(D%d)[%s]%cPROCESS_PRE_START PM%d::D%d:%s::%d:102\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
							}
						}
						else {
							if ( slot <= 0 ) {
//								_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Append at %s[%s]%cPROCESS_POST_START PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Append at %s[%s]%cPROCESS_POST_START PM%d:::%s::%d:101\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
							}
							else {
//								_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Append at %s(D%d)[%s]%cPROCESS_POST_START PM%d:D%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
								_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Append at %s(D%d)[%s]%cPROCESS_POST_START PM%d::D%d:%s::%d:102\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
							}
						}
					}
					//-----------------------------------------------------------------------------------------------------------------------------------
					if ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) { // pre
						_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
					}
					else {
						_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch );
					}
					//-----------------------------------------------------------------------------------------------------------------------------------
//					PROCESS_MONITOR_Setting1( ch , ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) ? PROCESS_INDICATOR_PRE : PROCESS_INDICATOR_POST , side , -1 , -1 , -1 , -1 , recipe , 1 , TRUE ); // 2014.02.13
					PROCESS_MONITOR_Setting1( ch ,0 , 0 , ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) ? PROCESS_INDICATOR_PRE : PROCESS_INDICATOR_POST , side , -1 , -1 , -1 , -1 , recipe , 1 , TRUE , FALSE ); // 2014.02.13
					//----------------------------------------------------------------------------
					return 2;
				}
			}
		}
	}
	//
	if ( _i_SCH_EQ_SPAWN_PROCESS( side , -1 , ch , incm , slot , arm , recipe , modeid , puttime , nextpm , mintime , -1 ) ) {
		//-----------------------------------------------------------------------------------------------------------------------------------
		if ( ( mode == 0 ) || ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) ) {
			if ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) { // pre
				if ( slot <= 0 ) {
//					_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Start at %s[%s]%cPROCESS_PRE_START PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Start at %s[%s]%cPROCESS_PRE_START PM%d:::%s::%d:111\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
				}
				else {
//					_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Start at %s(D%d)[%s]%cPROCESS_PRE_START PM%d:D%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Start at %s(D%d)[%s]%cPROCESS_PRE_START PM%d::D%d:%s::%d:112\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
				}
			}
			else {
				if ( slot <= 0 ) {
//					_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Start at %s[%s]%cPROCESS_POST_START PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Start at %s[%s]%cPROCESS_POST_START PM%d:::%s::%d:111\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata ); // 2015.04.23
				}
				else {
//					_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Start at %s(D%d)[%s]%cPROCESS_POST_START PM%d:D%d:%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Start at %s(D%d)[%s]%cPROCESS_POST_START PM%d::D%d:%s::%d:112\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , slot , recipe , 9 , ch - PM1 + 1 , slot , recipe , optdata ); // 2015.04.23
				}
			}
		}
		//-----------------------------------------------------------------------------------------------------------------------------------
		if ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) { // pre
			_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
		}
		else {
			_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch );
		}
		//-----------------------------------------------------------------------------------------------------------------------------------
//		PROCESS_MONITOR_Setting1( ch , ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) ? PROCESS_INDICATOR_PRE : PROCESS_INDICATOR_POST , side , -1 , -1 , -1 , -1 , recipe , 1 , FALSE ); // 2014.02.13
		PROCESS_MONITOR_Setting1( ch , 0 , 0 , ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) ? PROCESS_INDICATOR_PRE : PROCESS_INDICATOR_POST , side , -1 , -1 , -1 , -1 , recipe , 1 , FALSE , FALSE ); // 2014.02.13
		//----------------------------------------------------------------------------
		return 1;
	}
	else {
		if ( mode == 0 ) {
			if ( ( modeid == 2 ) || ( modeid == 11 ) || ( modeid == 21 ) ) { // pre
				_i_SCH_LOG_LOT_PRINTF( side , "PM PRE Process Fail at %s[%s]%cPROCESS_PRE_FAIL PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side , "PM POST Process Fail at %s[%s]%cPROCESS_POST_FAIL PM%d::%s:%d\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , recipe , 9 , ch - PM1 + 1 , recipe , optdata );
			}
		}
		else {
			_IO_CIM_PRINTF( "_SCH_MACRO_LOTPREPOST_PROCESS_OPERATION Failed because invalid action\n" );
		}
		return -1;
	}
	return 0;
}


//================================================================================================================
//================================================================================================================
//================================================================================================================
//================================================================================================================

// return -3 : Process Fail (Invalid Parameter Value)
//        -2 : Process Fail (Invalid PathDo)
//        -1 : need Abort
//         0 : need skip (Run Lot Pre or Wafer Pre)
//         1 : need skip (Run Lot Pre and Wafer Pre)
//         2 : return next

int _SCH_MACRO_PRE_PROCESS_OPERATION( int side , int pt , int arm , int ch0 ) {
	char processname[1024+1];
	int processtype; // 2017.10.18
	int ch; // 2008.12.07
	int order , wid , rpd , res; // 2009.01.14
	int lotpre = 0; // 2007.10.04
	int pathin , dummode; // 2011.09.29
	char buffer[32]; // 2012.12.10
	char MsgSltchar[16]; /* 2013.05.23 */
	int chx , cho; // 2013.10.10
	BOOL OnlyuseSelectPM; // 2013.10.10
	int s; // 2016.12.06
	BOOL LotPreOnly; // 2017.09.07
	int extmode; // 2018.06.22
	//
	ch  = ch0 % 100;
	cho = ch0 / 100000;
	chx = ( ch0 % 100000 ) / 100;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_PRE_PROCESS_OPERATION Failed because invalid side(%d)\n" , side );
		return -3;
	}
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_PRE_PROCESS_OPERATION Failed because invalid pt(%d)\n" , pt );
		return -3;
	}
	if ( ( ch < PM1 ) || ( ch >= AL ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_PRE_PROCESS_OPERATION Failed because invalid ch(%d)\n" , ch );
		return -3;
	}
	//
	//===========================================================
	//
	// 2016.11.04
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , pt ) ) return -3;
	//
	//===========================================================
	//
	if ( _i_SCH_CLUSTER_Get_Extra_Flag( side , pt , 3 ) == 1 ) { // 2013.05.13
		return 2;
	}
	//
	switch( cho ) { // 2013.10.10 // option
	case 1 :
		OnlyuseSelectPM = FALSE;
		LotPreOnly = FALSE;
		break;
	case 2 : // 2017.09.07
		OnlyuseSelectPM = ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) == 0 );
		LotPreOnly = TRUE;
		break;
	default :
		OnlyuseSelectPM = ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) == 0 );
		LotPreOnly = FALSE;
		break;
	}
	//
	if ( chx >= 10 ) { // 2013.06.01 // fixed
		rpd = chx - 10;
	}
	else {
		switch( chx ) {
		case 9 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 5 ;	break; // 2013.06.01
		case 8 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 4 ;	break; // 2013.06.01
		case 7 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 3 ;	break; // 2013.06.01
		case 6 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 2 ;	break; // 2013.06.01
		case 5 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) + 4 ;	break; // 2013.06.01
		case 4 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) + 3 ;	break; // 2013.06.01
		case 3 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) + 2 ;	break; // 2013.06.01
		case 2 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) + 1 ;	break; // 2012.12.10
		case 1 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt );		break;
		default :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 1;	break;
		}
	}
	//
	if ( ( rpd < 0 ) || ( rpd >= _i_SCH_CLUSTER_Get_PathRange( side , pt ) ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_PRE_PROCESS_OPERATION Failed because invalid rpd(%d)\n" , rpd );
		return -2;
	}
	//
	wid = _i_SCH_CLUSTER_Get_SlotIn( side , pt );
	pathin = _i_SCH_CLUSTER_Get_PathIn( side , pt ); // 2011.09.29
//	dummode = SCH_Inside_ThisIs_BM_OtherChamberD( pathin , 1 ); // 2011.09.29 // 2017.09.11
	dummode = SCH_Inside_ThisIs_DummyMethod( pathin , 1 , 10 , side , pt ); // 2011.09.29 // 2017.09.11
	//
	// 2011.07.18
	//
	if ( !EQUIP_POST_PROCESS_SKIP_RUN( ch ) && _i_SCH_SUB_Check_Last_And_Current_CPName_is_Different( side , pt , ch ) ) {
		//
		if ( _i_SCH_PREPOST_Get_inside_READY_RECIPE_USE( side , ch ) == 0 ) {
			_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 3 );
		}
		else {
			_i_SCH_PREPRCS_FirstRunPre_Set_PathProcessData( ( side * 1000 ) + ch , 4 );
		}
	}
	//
	//
	// 2016.12.06
	//
	//
	if ( !EQUIP_POST_PROCESS_SKIP_RUN( ch ) && ( -1 != SIGNAL_MODE_FMTMSIDE_GET( FALSE , ch ) ) ) {
		//
		strcpy( processname , SIGNAL_MODE_FMTMRECIPE_GET( ch ) );
		//
		order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , rpd , ch );
		//
		res = _SCH_MACRO_PROCESS_APPEND_OPERATION( side , pt , ch , pathin , wid , arm , processname , 14 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 , FALSE , PROCESS_INDICATOR_PRE + wid , rpd , order , 0 , FALSE );
		//
		s =	SIGNAL_MODE_FMTMPRESKIPSIDE_GET( ch );
		//
		extmode = SIGNAL_MODE_FMTMEXTMODE_GET( ch ); // 2018.06.22
		//
		if ( extmode == 1 ) { // 2018.06.22
			if ( dummode < 0 ) dummode = 0;

		}
		//
		if ( SIGNAL_MODE_FMTMPRESKIPSIDE_GET( ch ) != -1 ) _i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( s , ch );
		//
//		SIGNAL_MODE_FMTMSIDE_SET( ch , -1 , NULL , -1 ); // 2018.06.22
		SIGNAL_MODE_FMTMSIDE_SET( ch , -1 , NULL , -1 , 0 ); // 2018.06.22
		//
		if ( res >= 0 ) {
			//
			if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
				//
				if ( res == 1 ) {
					sprintf( buffer , "Append" );
				}
				else if ( res == 2 ) {
					sprintf( buffer , "Append2" );
				}
				else {
					sprintf( buffer , "Start" );
				}
				//
				FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
				//
				if ( dummode < 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Set Process %s at %s(%d)[%s]%cPROCESS_SET_START PM%d::%d:%s:::201%c%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar );
				}
				else if ( dummode == 0 ) {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Set Process %s at %s(D%d)[%s]%cPROCESS_SET_START PM%d::D%d:%s:::202%cD%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Set Process %s at %s(D%d-%d)[%s]%cPROCESS_SET_START PM%d::D%d-%d:%s:::203%cD%d-%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , processname , 9 , ch - PM1 + 1 , dummode , wid , processname , 9 , dummode , wid , MsgSltchar );
				}
				//
			}
			if ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) != 1 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) < 2 ) ) {
				//
				lotpre = 1;
				//
				if ( res == 0 ) {
					//
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
					//
					if ( dummode >= 0 ) {
						PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE , dummode + 1 );
					}
					else {
						PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE , FALSE );
					}
				}
				//
				PROCESS_MONITOR_Set_Last_ProcessJob_Data( ch , side , pt );
				//
			}
			else {
				//
				lotpre = 2;
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 2 , ch );
				if ( EQUIP_WAIT_PROCESS( side , ch ) != SYS_SUCCESS ) return -1;
			}
		}
		else { // 2016.12.07
			//
			FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
			//
			if ( dummode < 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Set Process Fail at %s(%d)[%s]%cPROCESS_SET_FAIL PM%d::%d:%s:::201%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar );
			}
			else if ( dummode == 0 ) {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Set Process Fail at %s(D%d)[%s]%cPROCESS_SET_FAIL PM%d::D%d:%s:::202%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar );
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Set Process Fail at %s(D%d-%d)[%s]%cPROCESS_SET_FAIL PM%d::D%d-%d:%s:::203%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , processname , 9 , ch - PM1 + 1 , dummode , wid , processname , 9 , dummode , wid , MsgSltchar );
			}
			//
		}
	}
	//
	//
	//
	//
	if ( !EQUIP_POST_PROCESS_SKIP_RUN( ch ) && _i_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , ch ) ) {
		//
		_i_SCH_PREPRCS_FirstRunPre_Done_PathProcessData( side , ch );
		//
		if ( _i_SCH_PRM_GET_inside_READY_RECIPE_USE( side , ch ) != 0 ) {
			//
			if ( _i_SCH_PRM_GET_EIL_INTERFACE() > 0 ) {
				if ( !EIL_Get_READY_RECIPE_NAME( side , pt , ch , processname ) ) {
					strcpy( processname , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( side , ch ) );
					processtype = _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( side , ch ); // 2017.10.18
				}
				else {
					processtype = 0; // 2017.10.18
				}
			}
			else {
				strcpy( processname , _i_SCH_PRM_GET_inside_READY_RECIPE_NAME( side , ch ) );
				processtype = _i_SCH_PRM_GET_inside_READY_RECIPE_TYPE( side , ch ); // 2017.10.18
			}
			//
		}
		else {
			strcpy( processname , "?" );
			processtype = 0; // 2017.10.18
		}
		/*
		// 2012.12.10
		if ( _i_SCH_EQ_SPAWN_PROCESS( side , pt , ch , pathin , wid , arm , processname , 99 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 ) ) {
			//
			if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
				//
				if ( dummode < 0 ) { // 2011.09.29
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process Start at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:A1000%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
				}
				else if ( dummode == 0 ) { // 2011.09.29
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process Start at %s(D%d)[%s]%cPROCESS_PRE_START PM%d:D%d:%s:A1000%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process Start at %s(D%d-%d)[%s]%cPROCESS_PRE_START PM%d:D%d-%d:%s:A1000%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , processname , 9 , ch - PM1 + 1 , dummode , wid , processname , 9 , dummode , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
				}
				//
			}
			if ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) != 1 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) < 2 ) ) {
				//
				lotpre = 1; // 2007.10.04
				//
				order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , rpd , ch ); // 2008.01.14
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
				//
				if ( dummode >= 0 ) { // 2011.09.29
					PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE , dummode + 1 );
				}
				else {
					PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE );
				}
//				return 0; // 2007.10.04
				//
				PROCESS_MONITOR_Set_Last_ProcessJob_Data( ch , side , pt ); // 2011.07.18
				//
			}
			else {
				//
				lotpre = 2; // 2007.10.04
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 2 , ch );
				if ( EQUIP_WAIT_PROCESS( side , ch ) != SYS_SUCCESS ) return -1;
			}
		}
*/
		//
		// 2012.12.10
		//
		order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , rpd , ch ); // 2008.01.14
		//
		res = _SCH_MACRO_PROCESS_APPEND_OPERATION( side , pt , ch , pathin , wid , arm , processname , 99 , 0 , "" , PROCESS_DEFAULT_MINTIME , -1 , FALSE , PROCESS_INDICATOR_PRE + wid , rpd , order , 0 , FALSE ); // 2012.12.10
		//
		if ( res >= 0 ) { // 2012.12.10
			//
			Scheduler_LotPre_Data_Set( TRUE , side , ch , processname , processtype ); // 2017.10.18
			//
			if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
				//
				if ( res == 1 ) { // 2012.12.10
					sprintf( buffer , "Append" );
				}
				else if ( res == 2 ) {
					sprintf( buffer , "Append2" );
				}
				else {
					sprintf( buffer , "Start" );
				}
				//
				FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
				//
				if ( dummode < 0 ) { // 2011.09.29
//					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process %s at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:A1000%c%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process %s at %s(%d)[%s]%cPROCESS_PRE_START PM%d::%d:%s:::201%c%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar ); // 2015.04.23
				}
				else if ( dummode == 0 ) { // 2011.09.29
//					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process %s at %s(D%d)[%s]%cPROCESS_PRE_START PM%d:D%d:%s:A1000%cD%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process %s at %s(D%d)[%s]%cPROCESS_PRE_START PM%d::D%d:%s:::202%cD%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar ); // 2015.04.23
				}
				else {
//					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process %s at %s(D%d-%d)[%s]%cPROCESS_PRE_START PM%d:D%d-%d:%s:A1000%cD%d-%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , processname , 9 , ch - PM1 + 1 , dummode , wid , processname , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process %s at %s(D%d-%d)[%s]%cPROCESS_PRE_START PM%d::D%d-%d:%s:::203%cD%d-%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , processname , 9 , ch - PM1 + 1 , dummode , wid , processname , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
				}
				//
			}
			if ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) != 1 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) < 2 ) ) {
				//
				lotpre = 1; // 2007.10.04
				//
				if ( res == 0 ) { // 2012.12.10
					//
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
					//
					if ( dummode >= 0 ) { // 2011.09.29
						PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE , dummode + 1 );
					}
					else {
//						PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE ); // 2014.02.14
						PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , processname , 0 , FALSE , FALSE ); // 2014.02.14
					}
	//				return 0; // 2007.10.04
					//
				}
				//
				PROCESS_MONITOR_Set_Last_ProcessJob_Data( ch , side , pt ); // 2011.07.18
				//
			}
			else {
				//
				lotpre = 2; // 2007.10.04
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 2 , ch );
				if ( EQUIP_WAIT_PROCESS( side , ch ) != SYS_SUCCESS ) return -1;
			}
		}
		else { // 2016.12.07
			//
			FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
			//
			if ( dummode < 0 ) { // 2011.09.29
				_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d::%d:%s:::201%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar ); // 2015.04.23
			}
			else if ( dummode == 0 ) { // 2011.09.29
				_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process Fail at %s(D%d)[%s]%cPROCESS_PRE_FAIL PM%d::D%d:%s:::202%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , processname , 9 , ch - PM1 + 1 , wid , processname , 9 , wid , MsgSltchar ); // 2015.04.23
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side , "PM PRe Process Fail at %s(D%d-%d)[%s]%cPROCESS_PRE_FAIL PM%d::D%d-%d:%s:::203%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , processname , 9 , ch - PM1 + 1 , dummode , wid , processname , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
			}
			//
		}
	}
//	else { // 2007.10.04
	//
	order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , rpd , ch ); // 2008.01.14
	//
//	if ( !EQUIP_POST_PROCESS_SKIP_RUN( ch ) && _i_SCH_CLUSTER_Check_Possible_UsedPre( side , pt , rpd , order , ch , TRUE ) ) { // 2017.09.07
	//
	if ( !LotPreOnly && !EQUIP_POST_PROCESS_SKIP_RUN( ch ) && _i_SCH_CLUSTER_Check_Possible_UsedPre( side , pt , rpd , order , ch , TRUE ) ) { // 2017.09.07

		_i_SCH_CLUSTER_Get_Next_PM_String( side , pt , rpd + 1 , processname , 1024 );
		//-------------------------------------------------------------------------------------------
		// 2007.10.04
		//-------------------------------------------------------------------------------------------
/*
		// 2012.12.10
		if ( lotpre == 1 ) {
			if ( PROCESS_MONITOR_SPAWN_APPENDING( side , pt , ch , pathin , wid , arm , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 2 , 0 , processname , PROCESS_DEFAULT_MINTIME , 1 , PROCESS_INDICATOR_PRE + wid , rpd , order , 0 , FALSE ) ) { // 2011.03.02
				return 2;
			}
			else {
				if ( _i_SCH_EQ_SPAWN_EVENT_PROCESS( side , pt , ch , pathin , wid , arm , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 2 , 0 , processname , PROCESS_DEFAULT_MINTIME , 1 ) ) {
					if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
						_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Append at %s(%d)[%s]%cPROCESS_PRE_APPEND PM%d:%d:%s:A1111%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
					}
					_i_SCH_CLUSTER_Set_PathProcessData_UsedPre( side , pt , rpd , order , ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) == 0 );
					//
//					PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 0 , FALSE ); // 2011.03.02(Checking-원래 없었음)
					//
					return 0;
				}
			}
		}
		//-------------------------------------------------------------------------------------------
		if ( _i_SCH_EQ_SPAWN_PROCESS( side , pt , ch , pathin , wid , arm , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 2 , 0 , processname , PROCESS_DEFAULT_MINTIME , -1 ) ) {
			if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
				//
				if ( dummode < 0 ) { // 2011.09.29
					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Start at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:A1001%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
				}
				else if ( dummode == 0 ) { // 2011.09.29
					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Start at %s(D%d)[%s]%cPROCESS_PRE_START PM%d:D%d:%s:A1001%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
				}
				else {
					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Start at %s(D%d-%d)[%s]%cPROCESS_PRE_START PM%d:D%d-%d:%s:A1001%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , dummode , wid , FA_SIDE_TO_SLOT_GETxS( side,pt ) );
				}
				//
			}
			if ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) != 1 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) < 2 ) ) {
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
				//
				if ( dummode >= 0 ) { // 2011.09.29
					PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 0 , FALSE , dummode + 1 );
				}
				else {
					PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 0 , FALSE );
				}
				//
				_i_SCH_CLUSTER_Set_PathProcessData_UsedPre( side , pt , rpd , order , ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) == 0 );
				//
				if ( lotpre != 0 ) return 0; // 2007.10.04
				return 1;
			}
			else {
				_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 2 , ch );
				if ( EQUIP_WAIT_PROCESS( side , ch ) != SYS_SUCCESS ) return -1;
//				if ( lotpre != 0 ) return 0; // 2007.10.04
				if ( lotpre == 1 ) return 0; // 2007.10.04 // 2009.01.23
			}
		}
*/
		//
		// 2012.12.10
		//
		res = _SCH_MACRO_PROCESS_APPEND_OPERATION( side , pt , ch , pathin , wid , arm , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 2 , 0 , processname , PROCESS_DEFAULT_MINTIME , -1 , ( lotpre == 1 ) , PROCESS_INDICATOR_PRE + wid , rpd , order , 0 , FALSE ); // 2012.12.10
		//
		if ( res >= 0 ) { // 2012.12.10
			//
			EST_PM_PRE_PROCESS_START( ch ); // 2016.10.22
			//
			if ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() % 2 ) {
				//
				if ( res == 1 ) { // 2012.12.10
					sprintf( buffer , "Append" );
				}
				else if ( res == 2 ) {
					sprintf( buffer , "Append2" );
				}
				else {
					sprintf( buffer , "Start" );
				}
				//
				FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
				//
				if ( dummode < 0 ) { // 2011.09.29
//					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process %s at %s(%d)[%s]%cPROCESS_PRE_START PM%d:%d:%s:A1001%c%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process %s at %s(%d)[%s]%cPROCESS_PRE_START PM%d::%d:%s:::301%c%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , MsgSltchar ); // 2015.04.23
				}
				else if ( dummode == 0 ) { // 2011.09.29
//					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process %s at %s(D%d)[%s]%cPROCESS_PRE_START PM%d:D%d:%s:A1001%cD%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process %s at %s(D%d)[%s]%cPROCESS_PRE_START PM%d::D%d:%s:::302%cD%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , MsgSltchar ); // 2015.04.23
				}
				else {
//					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process %s at %s(D%d-%d)[%s]%cPROCESS_PRE_START PM%d:D%d-%d:%s:A1001%cD%d-%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
					_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process %s at %s(D%d-%d)[%s]%cPROCESS_PRE_START PM%d::D%d-%d:%s:::303%cD%d-%d%s\n" , buffer , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
				}
				//
			}
			//
			if      ( res == 1 ) { // 2012.12.10
				_i_SCH_CLUSTER_Set_PathProcessData_UsedPre( side , pt , rpd , order , OnlyuseSelectPM );
				return 0;
			}
			else if ( res == 2 ) { // 2012.12.10
				_i_SCH_CLUSTER_Set_PathProcessData_UsedPre( side , pt , rpd , order , OnlyuseSelectPM );
				return 2;
			}
			//
			if ( ( ( _i_SCH_PRM_GET_UTIL_PREPOST_PROCESS_DEPAND() % 3 ) != 1 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) < 2 ) ) {
				//
				if ( res == 0 ) { // 2012.12.10
					//
					_i_SCH_TIMER_SET_PROCESS_TIME_START( 2 , ch );
					//
					if ( dummode >= 0 ) { // 2011.09.29
						PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 0 , FALSE , dummode + 1 );
					}
					else {
//						PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 0 , FALSE ); // 2014.02.13
						PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_PRE + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 0 , FALSE , FALSE ); // 2014.02.13
					}
				}
				//
				_i_SCH_CLUSTER_Set_PathProcessData_UsedPre( side , pt , rpd , order , OnlyuseSelectPM );
				//
				if ( lotpre != 0 ) return 0; // 2007.10.04
				//
				return 1;
			}
			else {
				//
				_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 2 , ch );
				if ( EQUIP_WAIT_PROCESS( side , ch ) != SYS_SUCCESS ) return -1;
//				if ( lotpre != 0 ) return 0; // 2007.10.04
				if ( lotpre == 1 ) return 0; // 2007.10.04 // 2009.01.23
			}
			//
		}
		else { // 2016.12.07
			//
			FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
			//
			if ( dummode < 0 ) { // 2011.09.29
				_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Fail at %s(%d)[%s]%cPROCESS_PRE_FAIL PM%d::%d:%s:::301%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , MsgSltchar ); // 2015.04.23
			}
			else if ( dummode == 0 ) { // 2011.09.29
				_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Fail at %s(D%d)[%s]%cPROCESS_PRE_FAIL PM%d::D%d:%s:::302%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , wid , MsgSltchar ); // 2015.04.23
			}
			else {
				_i_SCH_LOG_LOT_PRINTF( side , "PM Pre Process Fail at %s(D%d-%d)[%s]%cPROCESS_PRE_FAIL PM%d::D%d-%d:%s:::303%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , ch - PM1 + 1 , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 2 ) , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
			}
			//
		}
	}
	else {
		if ( lotpre != 0 ) return 0; // 2007.10.04
		_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 2 , ch );
	}
//	} // 2007.10.04
	return 2;
}

/*
//
// 2017.09.20
//
BOOL _SCH_MACRO_HAS_PRE_PROCESS_OPERATION( int side , int pt , int ch ) {
	int k , m , order;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	if ( ( ch < PM1 ) || ( ch >= AL ) ) return FALSE;
	//
	if ( _i_SCH_CLUSTER_Get_Extra_Flag( side , pt , 3 ) == 1 ) { // 2013.05.13
		return FALSE;
	}
	//
	if ( PROCESS_MONITOR_HAS_APPENDING( ch ) ) return TRUE; // 2017.09.19
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , k );
		//
		if ( m > 0 ) {
			if ( ( ch == -1 ) || ( ch == m ) ) {
				//
				if ( -1 != SIGNAL_MODE_FMTMSIDE_GET( FALSE , m ) ) return TRUE; // 2016.12.06
				//
				if ( _i_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , m ) ) return TRUE;
				//
				if ( _i_SCH_SUB_Check_Last_And_Current_CPName_is_Different( side , pt , ch ) ) return TRUE; // 2011.07.18
				//
				order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , 0 , m );
				if ( order != -1 ) {
					if ( _i_SCH_CLUSTER_Check_Possible_UsedPre( side , pt , 0 , order , m , FALSE ) ) return TRUE;
				}
			}
		}
	}
	return FALSE;
}
*/

//
// 2017.09.20
//
BOOL _SCH_MACRO_HAS_PRE_PROCESS_OPERATION( int side , int pt , int ch0 ) {
	int k , m , order , ch , mode;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return FALSE;
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	//
	if      ( ch0 == 0 ) {
		ch = -1;
		mode = 0;
	}
	else if ( ch0 == -1 ) {
		ch = -1;
		mode = 1;
	}
	else if ( ch0 == -2 ) {
		ch = -1;
		mode = 2;
	}
	else if ( ch0 == -3 ) {
		ch = -1;
		mode = 3;
	}
	else if ( ch0 == -4 ) {
		ch = -1;
		mode = 4;
	}
	else {
		//
		ch = ch0 % 100;
		mode = ch0 / 100;
		//
		if ( ( ch < PM1 ) || ( ch >= AL ) ) return FALSE;
		//
	}
	//
	if ( _i_SCH_CLUSTER_Get_Extra_Flag( side , pt , 3 ) == 1 ) { // 2013.05.13
		return FALSE;
	}
	//
	if ( ( mode == 0 ) || ( mode == 3 ) || ( mode == 4 ) ) {
		if ( PROCESS_MONITOR_HAS_APPENDING( ch ) ) return TRUE; // 2017.09.19
	}
	//
	for ( k = 0 ; k < MAX_PM_CHAMBER_DEPTH ; k++ ) {
		//
		m = _i_SCH_CLUSTER_Get_PathProcessChamber( side , pt , 0 , k );
		//
		if ( m > 0 ) {
			//
			if ( ( ch == -1 ) || ( ch == m ) ) {
				//
				if ( ( mode == 0 ) || ( mode == 3 ) || ( mode == 4 ) ) {
					if ( -1 != SIGNAL_MODE_FMTMSIDE_GET( FALSE , m ) ) return TRUE; // 2016.12.06
				}
				//
				if ( ( mode == 0 ) || ( mode == 2 ) || ( mode == 4 ) ) {
					//
					if ( _i_SCH_PREPRCS_FirstRunPre_Check_PathProcessData( side , m ) ) return TRUE;
					//
					if ( _i_SCH_SUB_Check_Last_And_Current_CPName_is_Different( side , pt , ch ) ) return TRUE; // 2011.07.18
					//
				}
				//
				if ( ( mode == 0 ) || ( mode == 1 ) ) {
					order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , 0 , m );
					if ( order != -1 ) {
						if ( _i_SCH_CLUSTER_Check_Possible_UsedPre( side , pt , 0 , order , m , FALSE ) ) return TRUE;
					}
					//
				}
				//
			}
		}
	}
	return FALSE;
}


int _SCH_MACRO_POST_PROCESS_OPERATION( int side , int pt , int arm , int ch0 ) {
	char NextPM[255+1];
	int order , wid , rpd; // 2009.01.14
	int ch; // 2011.09.15
	int pathin , dummode; // 2011.09.29
	char MsgSltchar[16]; /* 2013.05.23 */
	//
	ch = ch0 % 100;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_POST_PROCESS_OPERATION Failed because invalid side(%d)\n" , side );
		return -3;
	}
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_POST_PROCESS_OPERATION Failed because invalid pt(%d)\n" , pt );
		return -3;
	}
	if ( ( ch < PM1 ) || ( ch >= AL ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_POST_PROCESS_OPERATION Failed because invalid ch(%d)\n" , ch );
		return -3;
	}
	//
	//===========================================================
	//
	// 2016.11.04
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , pt ) ) return -3;
	//
	//===========================================================
	//
	if ( _i_SCH_CLUSTER_Get_Extra_Flag( side , pt , 3 ) == 1 ) { // 2013.05.13
//		return 0; // 2014.04.24
		return 4; // 2014.04.24
	}
	//
	// 2011.09.15
	//
	switch( ch0 / 100 ) {
	case 2 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt );		break;
	case 1 :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 1;	break;
	default :	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 2;	break;
	}
	//
//	rpd = _i_SCH_CLUSTER_Get_PathDo( side , pt ) - 2; // 2011.09.17
	//
	if ( ( rpd < 0 ) || ( rpd >= _i_SCH_CLUSTER_Get_PathRange( side , pt ) ) ) {
		_IO_CIM_PRINTF( "_SCH_MACRO_POST_PROCESS_OPERATION Failed because invalid rpd(%d)\n" , rpd );
		return -2;
	}
	//
	wid = _i_SCH_CLUSTER_Get_SlotIn( side , pt );
	pathin = _i_SCH_CLUSTER_Get_PathIn( side , pt ); // 2011.09.29
//	dummode = SCH_Inside_ThisIs_BM_OtherChamberD( pathin , 1 ); // 2011.09.29 // 2017.09.11
	dummode = SCH_Inside_ThisIs_DummyMethod( pathin , 1 , 10 , side , pt ); // 2011.09.29 // 2017.09.11
	order = _i_SCH_CLUSTER_Get_PathOrder( side , pt , rpd , ch );
	//
	if ( _i_SCH_CLUSTER_Check_Possible_UsedPost( side , pt , rpd , order ) ) {
		_i_SCH_CLUSTER_Get_Next_PM_String( side , pt , rpd + 1 , NextPM , 255 );
		if ( !EQUIP_POST_PROCESS_SKIP_RUN( ch ) ) {
			if ( _i_SCH_EQ_SPAWN_PROCESS( side , pt , ch , pathin , wid , arm , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 1 , 0 , NextPM , PROCESS_DEFAULT_MINTIME , -1 ) ) {
				//
				EST_PM_POST_PROCESS_START( ch ); // 2016.10.22
				//
				if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) {
					//
					FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
					//
					if ( dummode < 0 ) { // 2011.09.29
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(%d)[%s]%cPROCESS_POST_START PM%d:%d:%s:A1000%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , wid , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(%d)[%s]%cPROCESS_POST_START PM%d::%d:%s:::301%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , wid , MsgSltchar ); // 2015.04.23
					}
					else if ( dummode == 0 ) { // 2011.09.29
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d)[%s]%cPROCESS_POST_START PM%d:D%d:%s:A1000%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , wid , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d)[%s]%cPROCESS_POST_START PM%d::D%d:%s:::302%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , ch - PM1 + 1 , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , wid , MsgSltchar ); // 2015.04.23
					}
					else {
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d-%d)[%s]%cPROCESS_POST_START PM%d:D%d-%d:%s:A1000%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , ch - PM1 + 1 , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d-%d)[%s]%cPROCESS_POST_START PM%d::D%d-%d:%s:::303%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , ch - PM1 + 1 , dummode , wid , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
					}
					//
				}
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch );
				//
				if ( dummode >= 0 ) { // 2011.09.29
					PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_POST + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 0 , FALSE , dummode + 1 );
				}
				else {
//					PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_POST + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 0 , FALSE ); // 2014.02.13
					PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_POST + wid , side , pt , pathin , rpd , order , _i_SCH_CLUSTER_Get_PathProcessRecipe( side , pt , rpd , order , 1 ) , 0 , FALSE , FALSE ); // 2014.02.13
				}
				return 2;
			}
			else {
				_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 1 , ch );
				return -1;
			}
		}
		else {
			_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 1 , ch );
			return 1;
		}
	}
	else {
		if ( EQUIP_POST_PROCESS_FORCE_RUN( ch ) ) {
			_i_SCH_CLUSTER_Get_Next_PM_String( side , pt , rpd + 1 , NextPM , 255 );
			if ( _i_SCH_EQ_SPAWN_PROCESS( side , pt , ch , pathin , wid , arm , "?" , 1 , 0 , NextPM , PROCESS_DEFAULT_MINTIME , -1 ) ) {
				//
				EST_PM_POST_PROCESS_START( ch ); // 2016.10.22
				//
				if ( ( _i_SCH_PRM_GET_UTIL_LOT_LOG_PRE_POST_PROCESS() / 2 ) % 2 ) {
					//
					FA_SIDE_TO_SLOT_GET_L( side,pt , MsgSltchar );
					//
					if ( dummode < 0 ) { // 2011.09.29
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(%d)[%s]%cPROCESS_POST_START PM%d:%d:%s:A1001%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , "?" , 9 , ch - PM1 + 1 , wid , "?" , 9 , wid , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(%d)[%s]%cPROCESS_POST_START PM%d::%d:%s:::304%c%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , "?" , 9 , ch - PM1 + 1 , wid , "?" , 9 , wid , MsgSltchar ); // 2015.04.23
					}
					else if ( dummode == 0 ) { // 2011.09.29
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d)[%s]%cPROCESS_POST_START PM%d:D%d:%s:A1001%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , "?" , 9 , ch - PM1 + 1 , wid , "?" , 9 , wid , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d)[%s]%cPROCESS_POST_START PM%d::D%d:%s:::305%cD%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , wid , "?" , 9 , ch - PM1 + 1 , wid , "?" , 9 , wid , MsgSltchar ); // 2015.04.23
					}
					else {
//						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d-%d)[%s]%cPROCESS_POST_START PM%d:D%d-%d:%s:A1001%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , "?" , 9 , ch - PM1 + 1 , dummode , wid , "?" , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
						_i_SCH_LOG_LOT_PRINTF( side , "PM Post Process Start at %s(D%d-%d)[%s]%cPROCESS_POST_START PM%d::D%d-%d:%s:::306%cD%d-%d%s\n" , _i_SCH_SYSTEM_GET_MODULE_NAME( ch ) , dummode , wid , "?" , 9 , ch - PM1 + 1 , dummode , wid , "?" , 9 , dummode , wid , MsgSltchar ); // 2015.04.23
					}
					//
				}
				_i_SCH_TIMER_SET_PROCESS_TIME_START( 1 , ch );
				if ( dummode >= 0 ) { // 2011.09.29
					PROCESS_MONITOR_Setting_with_Dummy( ch , PROCESS_INDICATOR_POST + wid , side , pt , pathin , rpd , order , "?" , 0 , FALSE , dummode + 1 );
				}
				else {
//					PROCESS_MONITOR_Setting1( ch , PROCESS_INDICATOR_POST + wid , side , pt , pathin , rpd , order , "?" , 0 , FALSE ); // 2014.02.13
					PROCESS_MONITOR_Setting1( ch , 0 , 0 , PROCESS_INDICATOR_POST + wid , side , pt , pathin , rpd , order , "?" , 0 , FALSE , FALSE ); // 2014.02.13
				}
				return 3;
			}
			else {
				return -1;
			}
		}
		else {
			_i_SCH_TIMER_SET_PROCESS_TIME_SKIP( 1 , ch );
			return 0;
		}
	}
	return 0;
}

//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
//================================================================================================================================================================
/*
// 2012.09.21
int _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( int ch ) {
	int t , m , s , p , w , cs , cp , cw , csip , cpdo , top;
	int ts1 , tp1 , tw1 , ts2 , tp2 , tw2 , tsip;
	int ts , tp , tdo; // 2012.08.21
	int pm , z;
	//
//	*Result = 0;
	//
	if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_USE() == 0 ) return 0;
	//
	if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP(ch) != 0 ) return 0; // 2012.08.23
	//
	if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) return 0;
	//
	if ( ( ch < PM1 ) || ( ch >= AL ) ) return 0;
	//
	if ( !_i_SCH_MODULE_GET_USE_ENABLE( ch , FALSE , -1 ) ) return 0;
	//
	for ( z = 0 ; z < MAX_PM_SLOT_DEPTH ; z++ ) {
		cs = _i_SCH_MODULE_Get_PM_SIDE( ch , z );
		cp = _i_SCH_MODULE_Get_PM_POINTER( ch , z );
		cw = _i_SCH_MODULE_Get_PM_WAFER( ch , z );
		if ( cw > 0 ) break;
	}
	//
	if ( cw <= 0 ) return 0;
	//
	if ( ( cs < 0 ) || ( cs >= MAX_CASSETTE_SIDE ) ) return 0;
	if ( ( cp < 0 ) || ( cp >= MAX_CASSETTE_SLOT_SIZE ) ) return 0;
	//
	if ( !_i_SCH_SYSTEM_USING_STARTING( cs ) ) return 0;
	//
	csip = _i_SCH_CLUSTER_Get_SupplyID( cs , cp );
	cpdo = _i_SCH_CLUSTER_Get_PathDo( cs , cp );
	//
	for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
		if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( t , ch ) > 0 ) break;
	}
	//
	if ( t == MAX_TM_CHAMBER_DEPTH ) t = _i_SCH_PRM_GET_MODULE_GROUP( ch );
	//
	top = _i_SCH_PRM_GET_MODULE_PICK_ORDERING( t );	if ( top <= 0 ) return 0;
	//
	ts1 = _i_SCH_MODULE_Get_TM_SIDE( t , 0 );
	tp1 = _i_SCH_MODULE_Get_TM_POINTER( t , 0 );
	tw1 = _i_SCH_MODULE_Get_TM_WAFER( t , 0 );
	//
	ts2 = _i_SCH_MODULE_Get_TM_SIDE( t , 1 );
	tp2 = _i_SCH_MODULE_Get_TM_POINTER( t , 1 );
	tw2 = _i_SCH_MODULE_Get_TM_WAFER( t , 1 );
	//
	tsip = -1;
	tdo = -1; // 2012.08.21
	//
	if      ( ( tw1 > 0 ) && ( tw2 > 0 ) ) {
		return 0;
	}
	else if ( tw1 > 0 ) {
		if ( ( ts1 >= 0 ) && ( ts1 < MAX_CASSETTE_SIDE ) ) {
			if ( ( tp1 >= 0 ) && ( tp1 < MAX_CASSETTE_SLOT_SIZE ) ) {
				//
				tsip = _i_SCH_CLUSTER_Get_SupplyID( ts1 , tp1 );
				//
				tdo  = _i_SCH_CLUSTER_Get_PathDo( ts1 , tp1 ); // 2012.08.21
				if ( tdo > _i_SCH_CLUSTER_Get_PathRange( ts1 , tp1 ) ) tdo = -1; // 2012.08.21
				ts = ts1; // 2012.08.21
				tp = tp1; // 2012.08.21
				//
			}
		}
	}
	else if ( tw2 > 0 ) {
		if ( ( ts2 >= 0 ) && ( ts2 < MAX_CASSETTE_SIDE ) ) {
			if ( ( tp2 >= 0 ) && ( tp2 < MAX_CASSETTE_SLOT_SIZE ) ) {
				//
				tsip = _i_SCH_CLUSTER_Get_SupplyID( ts2 , tp2 );
				//
				tdo  = _i_SCH_CLUSTER_Get_PathDo( ts2 , tp2 ); // 2012.08.21
				if ( tdo > _i_SCH_CLUSTER_Get_PathRange( ts2 , tp2 ) ) tdo = -1; // 2012.08.21
				ts = ts2; // 2012.08.21
				tp = tp2; // 2012.08.21
				//
			}
		}
	}
	else {
		tsip = -1;
	}
	//
	if ( ( top == 1 ) && ( tdo > 0 ) ) { // 2012.08.21
		//
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			//
			pm = _i_SCH_CLUSTER_Get_PathProcessChamber( ts , tp , tdo - 1 , m );
			//
			if ( pm == ch ) return 0;
			//
		}
	}
	//
	if ( ( top != 1 ) && ( tsip == -1 ) ) {
		//
		if ( cpdo < _i_SCH_CLUSTER_Get_PathRange( cs , cp ) ) {
			for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
				pm = _i_SCH_CLUSTER_Get_PathProcessChamber( cs , cp , cpdo , m );
				//
				if ( pm > 0 ) {
					for ( z = 0 ; z < MAX_PM_SLOT_DEPTH ; z++ ) {
						if ( _i_SCH_MODULE_Get_PM_WAFER( ch , z ) > 0 ) break;
					}
					//
					if ( z == MAX_PM_SLOT_DEPTH ) {
						if ( PROCESS_MONITOR_Run_and_Get_Status( pm ) <= 0 ) {
							return 0;
						}
					}
				}
				//
			}
		}
		//
	}
	//
	if ( tsip >= 0 ) {
		if ( tsip < csip ) {
//			*Result = 1;
			return cw;
		}
	}
	//
	for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
		//
		if ( ch == ( m + PM1 ) ) continue;
		//
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( ( m + PM1 ) , FALSE , -1 ) ) continue;
		//
		if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ( m + PM1 ) ) >= 2 ) continue;
		//
		for ( z = 0 ; z < MAX_TM_CHAMBER_DEPTH ; z++ ) {
			if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( z , ( m + PM1 ) ) > 0 ) break;
		}
		//
		if ( z == MAX_TM_CHAMBER_DEPTH ) {
			if ( _i_SCH_PRM_GET_MODULE_GROUP( m + PM1 ) != t ) continue;
		}
		else {
			if ( z != t ) continue;
		}
		//
		for ( z = 0 ; z < MAX_PM_SLOT_DEPTH ; z++ ) {
			s = _i_SCH_MODULE_Get_PM_SIDE( m + PM1 , z );
			p = _i_SCH_MODULE_Get_PM_POINTER( m + PM1 , z );
			w = _i_SCH_MODULE_Get_PM_WAFER( m + PM1 , z );
			if ( w > 0 ) break;
		}
		//
		if ( w <= 0 ) continue;
		//
		if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) continue;
		if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) continue;
		//
		if ( top == 1 ) {
			if ( tsip < 0 ) {
				if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
					if ( csip < _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
//						*Result = ( m * 10 ) + 2;
						return cw;
					}
				}
			}
			else {
				if ( tsip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
					if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
						if ( csip < _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
//							*Result = ( m * 10 ) + 3;
							return cw;
						}
					}
				}
			}
		}
		else {
			if ( tsip < 0 ) {
				if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
					if ( csip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
//						*Result = ( m * 10 ) + 4;
						return cw;
					}
				}
			}
			else {
				if ( tsip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
					if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
						if ( csip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
//							*Result = ( m * 10 ) + 5;
							return cw;
						}
					}
				}
			}
		}
		//
	}
	//
	return 0;
	//
}
*/

// 2012.09.26
int SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[MAX_CHAMBER];
//int SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM;

void _iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_INIT() {
	int i;
//	SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM = FALSE;
	for ( i = 0 ; i < MAX_CHAMBER ; i++ ) SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] = FALSE;
}

void _iSCH_MACRO_CHECK_PICK_ORDERING_FLAG_SET( int ch , int data ) {
	int i;
	//
	if ( ch == -1 ) {
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[i] = data; // 2012.11.09
//		SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM = data;
		return;
	}
	//
	if ( ch < 0 ) return;
	if ( ch >= MAX_CHAMBER ) return;
	//
	if ( data ) {
		SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[ch] = TRUE;
	}
	else {
		SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[ch] = FALSE;
//		if ( ( ch >= BM1 ) && ( ch < TM ) ) SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM = FALSE;
	}
	//
}


int _i_SCH_MACRO_GET_T_POS( int ch , int s , int p ) { // 2012.11.21
	//
	int t , m , npm , cpdo;
	//
	if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) {
		//
		cpdo = _i_SCH_CLUSTER_Get_PathDo( s , p );
		//
		if ( cpdo >= _i_SCH_CLUSTER_Get_PathRange( s , p ) ) return -2;
		//
		t = -1;
		//
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			//
			npm = _i_SCH_CLUSTER_Get_PathProcessChamber( s , p , cpdo , m );
			//
			if ( npm > 0 ) {
				//
				if ( _i_SCH_PRM_GET_MODULE_GROUP( npm ) < 0 ) continue; // 2014.09.24
				//
				if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , npm ) >= 2 ) continue;
				//
				for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
					if ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( t , npm ) > 0 ) break;
				}
				//
				if ( t == MAX_TM_CHAMBER_DEPTH ) t = _i_SCH_PRM_GET_MODULE_GROUP( npm );
				//
				break;
				//
			}
			//
		}
		//
	}
	else {
		//
		for ( t = 0 ; t < MAX_TM_CHAMBER_DEPTH ; t++ ) {
			//
			if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( t , ch ) > 0 ) break;
			//
		}
		//
		if ( t == MAX_TM_CHAMBER_DEPTH ) {
			//
			if ( _i_SCH_PRM_GET_MODULE_GROUP( ch ) >= 0 ) { // 2014.09.24
				//
				t = _i_SCH_PRM_GET_MODULE_GROUP( ch );
				//
			}
			//
		}
		//
	}
	//
	return t;
	//
}

BOOL _i_SCH_MACRO_CHECK_PROCESSING_CHECK_Diff_Path( int s , int p , int src_s , int src_p ) { // 2012.11.29
	int i , j , k , m , pm[MAX_PM_CHAMBER_DEPTH];
	//
	for ( i = 0 ; i < MAX_PM_CHAMBER_DEPTH ; i++ ) {
		pm[i] = FALSE;
	}
	//
	k = _i_SCH_CLUSTER_Get_PathRange( s , p );
	//
	for ( i = 0 ; i < k ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( s , p , i , j );
			//
			if      ( m > 0 ) {
				pm[m-PM1] = TRUE;
			}
			else if ( m < 0 ) {
				pm[-m-PM1] = TRUE;
			}
			//
		}
	}
	//
	k = _i_SCH_CLUSTER_Get_PathRange( src_s , src_p );
	//
	for ( i = 0 ; i < k ; i++ ) {
		//
		for ( j = 0 ; j < MAX_PM_CHAMBER_DEPTH ; j++ ) {
			//
			m = _i_SCH_CLUSTER_Get_PathProcessChamber( src_s , src_p , i , j );
			//
			if      ( m > 0 ) {
				//
				if ( ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , m ) >= 2 ) ) continue;
				//
				if ( pm[m-PM1] ) return FALSE;
			}
			else if ( m < 0 ) {
				//
				if ( ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) >= 2 ) && ( _i_SCH_PRM_GET_MODULE_PLACE_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , -m ) >= 2 ) ) continue;
				//
				if ( pm[-m-PM1] ) return FALSE;
			}
			//
		}
	}
	//
	return TRUE;
}

// 2012.09.21
int _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( int ch , int *res ) {
	int t , m , s , p , w , cs , cp , cw , csip , cpdo , top;
	int ts1 , tp1 , tw1 , ts2 , tp2 , tw2 , tsip;
	int ts , tp , tdo; // 2012.08.21
	int pm , z , xw;
	int cclidx; // 2012.10.05
	int skipbmcnt , skipbm , bmsp , bmcw , bmres; // 2012.11.09
	//
	//------------------------------------------------------------------------------
	*res = 0;
	//
	if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_USE() == 0 ) return -1;
	//
	if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP(ch) != 0 ) return -2; // 2012.08.23
	//
	if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[ch] ) return -3; // 2012.09.26
	//
//	if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( MAX_TM_CHAMBER_DEPTH + 1 , ch ) >= 2 ) return -4; // 2012.11.21
	//
	//------------------------------------------------------------------------------
	if ( ( ch < PM1 ) || ( ch >= AL ) ) return -4;
	//------------------------------------------------------------------------------
	cw = 0;
	//
	for ( z = 0 ; z < _i_SCH_PRM_GET_MODULE_SIZE(ch) ; z++ ) {
		//
		cs = _i_SCH_MODULE_Get_PM_SIDE( ch , z );
		cp = _i_SCH_MODULE_Get_PM_POINTER( ch , z );
		xw = _i_SCH_MODULE_Get_PM_WAFER( ch , z );
		//
		if ( xw > 0 ) {
			//
			if ( ( cs < 0 ) || ( cs >= MAX_CASSETTE_SIDE ) ) continue;
			if ( ( cp < 0 ) || ( cp >= MAX_CASSETTE_SLOT_SIZE ) ) continue;
			//
			if ( !_i_SCH_SYSTEM_USING_STARTING( cs ) ) continue;
			//
			if ( _i_SCH_SYSTEM_MODE_END_GET( cs ) != 0 ) continue; // 2013.10.08
			//
			cw = xw;
			//
			break;
		}
		//
	}
	//
	//------------------------------------------------------------------------------
	if ( cw <= 0 ) return -11;
	//------------------------------------------------------------------------------
	//
	csip = _i_SCH_CLUSTER_Get_SupplyID( cs , cp );
	cpdo = _i_SCH_CLUSTER_Get_PathDo( cs , cp );
	//
	if ( cpdo >= _i_SCH_CLUSTER_Get_PathRange( cs , cp ) ) { // 2012.10.08
		cclidx = _i_SCH_CLUSTER_Get_ClusterIndex( cs , cp );
	}
	else {
		cclidx = -1;
	}
	//
	t = _i_SCH_MACRO_GET_T_POS( ch , cs , cp ); // 2012.11.21
	//
	if ( t < 0 ) return -12;
	//
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//
	top = _i_SCH_PRM_GET_MODULE_PICK_ORDERING( t );	if ( top <= 0 ) return -21;
	//
	ts1 = _i_SCH_MODULE_Get_TM_SIDE( t , 0 );
	tp1 = _i_SCH_MODULE_Get_TM_POINTER( t , 0 );
	tw1 = _i_SCH_MODULE_Get_TM_WAFER( t , 0 );
	//
	ts2 = _i_SCH_MODULE_Get_TM_SIDE( t , 1 );
	tp2 = _i_SCH_MODULE_Get_TM_POINTER( t , 1 );
	tw2 = _i_SCH_MODULE_Get_TM_WAFER( t , 1 );
	//
	tsip = -1;
	tdo = -1; // 2012.08.21
	//
	if      ( ( tw1 > 0 ) && ( tw2 > 0 ) ) {
		return -22;
	}
	else if ( tw1 > 0 ) {
		if ( ( ts1 >= 0 ) && ( ts1 < MAX_CASSETTE_SIDE ) ) {
			if ( ( tp1 >= 0 ) && ( tp1 < MAX_CASSETTE_SLOT_SIZE ) ) {
				//
				tsip = _i_SCH_CLUSTER_Get_SupplyID( ts1 , tp1 );
				//
				tdo  = _i_SCH_CLUSTER_Get_PathDo( ts1 , tp1 ); // 2012.08.21
				if ( tdo > _i_SCH_CLUSTER_Get_PathRange( ts1 , tp1 ) ) tdo = -1; // 2012.08.21
				ts = ts1; // 2012.08.21
				tp = tp1; // 2012.08.21
				//
			}
		}
	}
	else if ( tw2 > 0 ) {
		if ( ( ts2 >= 0 ) && ( ts2 < MAX_CASSETTE_SIDE ) ) {
			if ( ( tp2 >= 0 ) && ( tp2 < MAX_CASSETTE_SLOT_SIZE ) ) {
				//
				tsip = _i_SCH_CLUSTER_Get_SupplyID( ts2 , tp2 );
				//
				tdo  = _i_SCH_CLUSTER_Get_PathDo( ts2 , tp2 ); // 2012.08.21
				if ( tdo > _i_SCH_CLUSTER_Get_PathRange( ts2 , tp2 ) ) tdo = -1; // 2012.08.21
				ts = ts2; // 2012.08.21
				tp = tp2; // 2012.08.21
				//
			}
		}
	}
	else {
		tsip = -1;
	}
	//
	if ( ( top == 1 ) && ( tdo > 0 ) ) { // 2012.08.21
		//
		for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
			//
			pm = _i_SCH_CLUSTER_Get_PathProcessChamber( ts , tp , tdo - 1 , m );
			//
			if ( pm == ch ) return -23;
			//
		}
	}
	//
	if ( ( top != 1 ) && ( tsip == -1 ) ) {
		//
		if ( cpdo < _i_SCH_CLUSTER_Get_PathRange( cs , cp ) ) {
			//
			for ( m = 0 ; m < MAX_PM_CHAMBER_DEPTH ; m++ ) {
				//
				pm = _i_SCH_CLUSTER_Get_PathProcessChamber( cs , cp , cpdo , m );
				//
				if ( pm > 0 ) {
					//
					for ( z = 0 ; z < MAX_PM_SLOT_DEPTH ; z++ ) {
						if ( _i_SCH_MODULE_Get_PM_WAFER( ch , z ) > 0 ) break;
					}
					//
					if ( z == MAX_PM_SLOT_DEPTH ) {
						if ( PROCESS_MONITOR_Run_and_Get_Status( pm ) <= 0 ) {
							return -24;
						}
					}
				}
				//
			}
		}
		//
	}
	//
	if ( tsip >= 0 ) {
		if ( tsip < csip ) {
			*res = 1;
			return cw;
		}
	}
	//
	for ( m = PM1 ; m < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ; m++ ) {
		//
		if ( ch == m ) continue;
		//
		if ( !_i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) continue;
		//
		if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( m ) != 0 ) continue; // 2012.09.21
		//
		if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[m] ) continue; // 2012.09.26
		//
		w = 0;
		//
		for ( z = 0 ; z < _i_SCH_PRM_GET_MODULE_SIZE( m ) ; z++ ) {
			s = _i_SCH_MODULE_Get_PM_SIDE( m , z );
			p = _i_SCH_MODULE_Get_PM_POINTER( m , z );
			xw = _i_SCH_MODULE_Get_PM_WAFER( m , z );
			//
			if ( xw > 0 ) {
				//
				if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) continue;
				if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) continue;
				//
				if ( !_i_SCH_SYSTEM_USING_STARTING( s ) ) continue;
				//
				if ( _i_SCH_SYSTEM_MODE_END_GET( s ) != 0 ) continue; // 2013.10.08
				//
				w = xw;
				//
				break;
			}
		}
		//
		if ( w <= 0 ) continue;
		//
		if ( _i_SCH_MACRO_CHECK_PROCESSING_CHECK_Diff_Path( cs , cp , s , p ) ) continue; // 2012.11.29
		//
		if ( _i_SCH_MACRO_GET_T_POS( m , s , p ) != t ) continue; // 2012.11.21
		//
		if ( top == 1 ) {
			if ( tsip < 0 ) {
				if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
					if ( csip < _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
						*res = 2;
						return cw;
					}
				}
			}
			else {
				if ( tsip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
					if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
						if ( csip < _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
							*res = 3;
							return cw;
						}
					}
				}
			}
		}
		else {
			if ( tsip < 0 ) {
				if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
					if ( csip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
						*res = 4;
						return cw;
					}
				}
			}
			else {
				if ( tsip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
					if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
						if ( csip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
							*res = 5;
							return cw;
						}
					}
				}
			}
		}
		//
	}
	//
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//
	skipbmcnt = 0; // 2012.11.09
	skipbm = 0; // 2012.11.09
	//
	if ( top == 1 ) {
		//
		if ( t > 0 ) return -91; // 2012.11.21
		//
//		if ( !SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM ) { // 2012.09.26
			//
			bmsp = -1; // 2012.11.09
			bmcw = 0; // 2012.11.09
			//
			for ( m = BM1 ; m < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ; m++ ) {
				//
				if ( !_i_SCH_MODULE_GET_USE_ENABLE( m , FALSE , -1 ) ) continue;
				//
				if ( _i_SCH_MODULE_Get_BM_FULL_MODE( m ) != BUFFER_TM_STATION ) continue;
				//
				if ( SCH_Inside_ThisIs_BM_OtherChamber( m , 0 ) ) continue;
				//
				if ( _i_SCH_PRM_GET_MODULE_PICK_ORDERING_SKIP( m ) != 0 ) continue;
				//
//				if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[m] ) continue; // 2012.09.26 // 2012.11.09
				//
				tw1 = 0; // 0:do , 1:Sts
				//
				if ( _i_SCH_PRM_GET_MODULE_PICK_GROUP( 0 , m ) <= 0 ) {
					if ( _i_SCH_PRM_GET_MODULE_GROUP( m ) > 0 ) {
						continue;
					}
					else {
						if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , m ) != BUFFER_IN_MODE ) {
							if ( _i_SCH_PRM_GET_MODULE_BUFFER_MODE( 0 , m ) != BUFFER_SWITCH_MODE ) {
								continue;
							}
							else {
								tw1 = 1;
							}
						}
					}
				}
				//
				w = 0;
				//
				for ( z = 1 ; z <= _i_SCH_PRM_GET_MODULE_SIZE(m) ; z++ ) {
					//
					s = _i_SCH_MODULE_Get_BM_SIDE( m , z );
					p = _i_SCH_MODULE_Get_BM_POINTER( m , z );
					tw2 =  _i_SCH_MODULE_Get_BM_STATUS( m , z );
					xw = _i_SCH_MODULE_Get_BM_WAFER( m , z );
					//
					if ( xw > 0 ) {
						//
						if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) ) continue;
						if ( ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) continue;
						//
						if ( !_i_SCH_SYSTEM_USING_STARTING( s ) ) continue;
						//
						if ( _i_SCH_SYSTEM_MODE_END_GET( s ) != 0 ) continue; // 2013.10.08
						//
						if ( _i_SCH_CLUSTER_Get_PathDo( s , p ) > 0 ) continue;
						//
						if ( cclidx != -1 ) { // 2012.10.08
							if ( _i_SCH_CLUSTER_Get_ClusterIndex( s , p ) != cclidx ) continue;
						}
						//
						if ( tw1 == 1 ) {
							if ( tw2 != BUFFER_INWAIT_STATUS ) continue;
						}
						//
						w = xw;
						//
						break;
					}
					//
					//
				}
				//
				if ( w <= 0 ) continue;
				//
				if ( _i_SCH_MACRO_CHECK_PROCESSING_CHECK_Diff_Path( cs , cp , s , p ) ) continue; // 2012.11.29
				//
				if ( top == 1 ) {
					if ( tsip < 0 ) {
						if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
							if ( csip < _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
								//
								// 2012.11.09
								//
								if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[m] ) { // 2012.11.09
									//
									skipbmcnt++;
									skipbm = m;
									//
									if ( bmsp < 0 ) {
										bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
									}
									else {
										if ( bmsp > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
											bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
											bmcw = 0;
										}
									}
									//
									continue;
								}
								//
								if ( bmsp < 0 ) {
									bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
									bmcw = cw;
									bmres = 11;
								}
								else {
									if ( bmsp > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
										bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
										bmcw = cw;
										bmres = 12;
									}
									else {
										//
										skipbmcnt++;
										skipbm = m;
										//
									}
								}
								//
//								*res = 11; // 2012.11.09
//								return cw; // 2012.11.09
							}
						}
					}
					else {
						if ( tsip > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
							if ( cpdo != _i_SCH_CLUSTER_Get_PathDo( s , p ) ) {
								if ( csip < _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
									//
									// 2012.11.09
									//
									if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_STS[m] ) { // 2012.11.09
										//
										skipbmcnt++;
										skipbm = m;
										//
										if ( bmsp < 0 ) {
											bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
										}
										else {
											if ( bmsp > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
												bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
												bmcw = 0;
											}
										}
										//
										continue;
									}
									//
									if ( bmsp < 0 ) {
										bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
										bmcw = cw;
										bmres = 21;
									}
									else {
										if ( bmsp > _i_SCH_CLUSTER_Get_SupplyID( s , p ) ) {
											bmsp = _i_SCH_CLUSTER_Get_SupplyID( s , p );
											bmcw = cw;
											bmres = 22;
										}
										else {
											//
											skipbmcnt++;
											skipbm = m;
											//
										}
									}
									//
//									*res = 12; // 2012.11.09
//									return cw; // 2012.11.09
								}
							}
						}
					}
				}
			}
			//
			if ( bmcw > 0 ) { // 2012.11.09
				*res = bmres;
				return bmcw;
			}
			//
//		}
		//
		if ( skipbmcnt > 0 ) {
			return -( ( skipbmcnt * 100 ) + skipbm );
		}
		//
	}
	else {
		return -92;
	}
	//------------------------------------------------------------------------------------------------------
	//
//	if ( SCH_MACRO_PICK_ORDER_CHECK_SKIP_BM ) return -98;
	return -99;
	//
}

int _i_SCH_MACRO_CHECK_PROCESSING_SUB_CHECK( int ch0 , BOOL info ) { // 2012.08.21
	int d , res , ch , chp , chs;
	//
	if ( ch0 >= 1000000 ) { // 2016.04.26
		chs = ch0 / 1000000;			// Slot
		chp = ( ch0 % 1000000 ) / 1000;	// ch_Param
		ch =    ch0 % 1000;				// Function
		//
		if ( ch <= 0 ) ch = GetRealMessageFunction( chp , chs ); // Real Function
		//
	}
	else {
		if ( ch0 >= 1000 ) {
			ch = ch0 % 1000;
			chp = ch0 / 1000;
		}
		else {
			ch = ch0;
			chp = 0;
		}
	}
	//
	if ( ( ch < CM1 ) || ( ch > FM ) ) return -1;
	//
	if ( _i_SCH_PRM_GET_EIL_INTERFACE() < 0 ) { // 2012.09.10
		if ( ( ch >= BM1 ) && ( ch < TM ) ) return _EIL_FMMODE_MACRO_CHECK_PROCESSING( ch );
	}
	//
	res = PROCESS_MONITOR_Run_and_Get_Status( ch );
	//
	if      ( res > 0 ) {
		if ( info ) return res;
		return 1;
	}
	else if ( res == 0 ) {
		//
		res = _i_SCH_MACRO_CHECK_PROCESSING_FOR_MOVE( ch , &d );
		//
		SCH_MACRO_PROCESS_MOVE[ch] = ( d * 1000 ) + res;
		//
		if ( res > 0 ) {
			if ( info ) return res;
			return 1;
		}
		//
		return 0;
	}
	else {
		return res;
	}
}


int _SCH_MACRO_CHECK_PROCESSING( int ch ) {
//	int res;
//	//
//	if ( ( ch < CM1 ) || ( ch > FM ) ) return -1;
//	//
//	res = PROCESS_MONITOR_Run_and_Get_Status( ch );
//	if ( res > 0 ) return 1;
//	return res;
	return _i_SCH_MACRO_CHECK_PROCESSING_SUB_CHECK( ch , FALSE ); // 2010.10.15
}

int _SCH_MACRO_CHECK_PROCESSING_INFO( int ch ) {
//	//
//	if ( ( ch < CM1 ) || ( ch > FM ) ) return -1;
//	//
//	return PROCESS_MONITOR_Run_and_Get_Status( ch );
	//
	if ( ch >= 1000 ) return PROCESS_MONITOR_Reset_Status( ch - 1000 ); // 2018.10.31
	//
	return _i_SCH_MACRO_CHECK_PROCESSING_SUB_CHECK( ch , TRUE ); // 2010.10.15
}


int _SCH_MACRO_CHECK_PROCESSING_GET_REMAIN_TIME( int ch , int wafer , int *tdata , int willch ) {
	//
	if ( ( ch < CM1 ) || ( ch > FM ) ) {
		*tdata = 0;
		return 0;
	}
	//
	return PROCESS_MONITOR_Get_Remain_Predict_Time( ch , wafer , tdata , willch );
}



