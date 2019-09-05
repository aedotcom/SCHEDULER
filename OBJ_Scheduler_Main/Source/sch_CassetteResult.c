#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "system_tag.h"
#include "User_Parameter.h"
#include "IO_Part_data.h"
#include "sch_prm_FBTPM.h"
#include "sch_prm_cluster.h"

#include "INF_sch_CommonUser.h"

//------------------------------------------------------------------------------------------
int				CASSETTE_RESULT[ MAX_CHAMBER ][ MAX_CASSETTE_SLOT_SIZE ];
int				CASSETTE_RESULT2[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2011.04.18
//
int				CASSETTE_SKIP[ MAX_CASSETTE_SIDE ][ MAX_CASSETTE_SLOT_SIZE ]; // 2005.07.19
int				CASSETTE_SKIP_FLAG[ MAX_CASSETTE_SIDE ]; // 2005.07.19
//------------------------------------------------------------------------------------------
void SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( int mode , int station , int slot , int srccass , int srcslot , int TMATM );
void SCHEDULER_CASSETTE_WAFER_RESULT2_SET_FOR_IO( int mode , int station , int slot , int srccass , int srcslot , int s , int p , int TMATM );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Cassette Result Operation
//
//
//
//	-1	:	notrun(Init)
//	-2	:	notrun(pick)
//	0	:	success
//	1	:	fail
//  2   :   User -> IO:from 4
//
//
//	Wafer Result
//	0	:	PRESENT
//	1	:	SUCCESS
//	2	:	FAIL
//	3	:	PROCESSING
//	4	:	USER
//
// SCHEDULER_CASSETTE_LAST_RESULT2_SET = data
//  1   : FAIL
//  0   : SUCCESS
//  -2  : NOTRUN
//  -99 : SKIP

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL SCHEDULER_CONTROL_Set_CASSETTE_SKIP( int side , int slot , BOOL data ) { // 2005.07.19
	int i;
	if ( side < 0 ) return FALSE;
	if ( side >= MAX_CASSETTE_SIDE ) return FALSE;
	if ( slot == -1 ) {
		CASSETTE_SKIP_FLAG[ side ] = data;
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			CASSETTE_SKIP[ side ][ i ] = data;
		}
	}
	else {
		if ( slot < 1 ) return FALSE;
		if ( slot > MAX_CASSETTE_SLOT_SIZE ) return FALSE;
		CASSETTE_SKIP_FLAG[ side ] = data;
		CASSETTE_SKIP[ side ][ slot - 1 ] = data;
	}
	return TRUE;
}
//=======================================================================================
//=======================================================================================
int _i_SCH_CASSETTE_Chk_SKIP( int side ) { // 2005.07.19
	int i , c , p;
	if ( !CASSETTE_SKIP_FLAG[ side ] ) return -1;
	CASSETTE_SKIP_FLAG[ side ] = FALSE;
	for ( p = 0 , c = 0 ; c < MAX_CASSETTE_SLOT_SIZE ; c++ ) {
		if ( CASSETTE_SKIP[ side ][ c ] ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if (
					( _i_SCH_CLUSTER_Get_PathRange( side , i ) >= 0 ) &&
					( _i_SCH_CLUSTER_Get_PathStatus( side , i ) == SCHEDULER_READY ) &&
					( _i_SCH_CLUSTER_Get_PathDo( side , i ) <= 0 ) &&
					( _i_SCH_CLUSTER_Get_PathIn( side , i ) == ( CM1 + side ) ) &&
					( _i_SCH_CLUSTER_Get_SlotIn( side , i ) == ( c + 1 ) )
				) {
//					_i_SCH_CLUSTER_Set_PathRange( side , i , -1 ); // 2008.04.23
					_i_SCH_CLUSTER_Set_PathRange( side , i , -3 ); // 2008.04.23
					p++;
				}
			}
			CASSETTE_SKIP[ side ][ c ] = FALSE;
		}
	}
	return p;
}
//
BOOL _i_SCH_CASSETTE_Chk_SKIP_WITH_POINTER( int side , int pt ) { // 2009.04.29
	int c;
	//
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	//
	if ( !CASSETTE_SKIP_FLAG[ side ] ) return FALSE;
	//
	if ( _i_SCH_CLUSTER_Get_PathRange( side , pt ) < 0 ) return FALSE;
	if ( _i_SCH_CLUSTER_Get_PathStatus( side , pt ) != SCHEDULER_READY ) return FALSE;
	if ( _i_SCH_CLUSTER_Get_PathDo( side , pt ) > 0 ) return FALSE;
	if ( _i_SCH_CLUSTER_Get_PathIn( side , pt ) != ( CM1 + side ) ) return FALSE;
	//
	c = _i_SCH_CLUSTER_Get_SlotIn( side , pt );
	//
	if ( !CASSETTE_SKIP[ side ][ c - 1 ] ) return FALSE;
	//
	CASSETTE_SKIP[ side ][ c - 1 ] = FALSE;
	_i_SCH_CLUSTER_Set_PathRange( side , pt , -3 );
	return TRUE;
}
//
void _SCH_CASSETTE_ReSet_For_Move( int tside , int p2 , int side , int p ) { // 2011.04.18
	CASSETTE_RESULT2[ tside ][ p2 ] = CASSETTE_RESULT2[ side ][ p ];
}

void SCHEDULER_CASSETTE_LAST_RESULT_INCOMPLETE_FAIL( int side ) {
	int cs , s , i , Res;
	for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue;
		if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) == _i_SCH_CLUSTER_Get_PathRange( side , i ) ) {
			if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) > 1 ) {
				switch( _i_SCH_CLUSTER_Get_PathStatus( side , i ) ) {
				case SCHEDULER_RUNNING2	:
					break;
				default :
					cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
					Res = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , 1 , 0 );
					//
					if ( cs >= CM1 ) { // 2011.04.14
						s = _i_SCH_CLUSTER_Get_SlotIn( side , i ) - 1;
						if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SLOT_SIZE ) ) {
							CASSETTE_RESULT[ cs ][ s ] = Res;
						}
						//
						CASSETTE_RESULT2[ side ][ i ] = Res; // 2011.04.18
						//
					}
					break;
				}
			}
		}
		else if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) < _i_SCH_CLUSTER_Get_PathRange( side , i ) ) {
			if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) > 1 ) {
				cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
				Res = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , 1 , 1 );
				//
				if ( cs >= CM1 ) { // 2011.04.14
					s = _i_SCH_CLUSTER_Get_SlotIn( side , i ) - 1;
					if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SLOT_SIZE ) ) {
						CASSETTE_RESULT[ cs ][ s ] = Res;
					}
					//
					CASSETTE_RESULT2[ side ][ i ] = Res; // 2011.04.18
				}
			}
			else if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) == 1 ) {
				switch( _i_SCH_CLUSTER_Get_PathStatus( side , i ) ) {
				case SCHEDULER_RUNNING2	:
				case SCHEDULER_WAITING	:
				case SCHEDULER_BM_END	:
				case SCHEDULER_CM_END	:
				case SCHEDULER_RETURN_REQUEST	:
					cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
					Res = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , 1 , 2 );
					//
					if ( cs >= CM1 ) { // 2011.04.14
						s = _i_SCH_CLUSTER_Get_SlotIn( side , i ) - 1;
						if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SLOT_SIZE ) ) {
							CASSETTE_RESULT[ cs ][ s ] = Res;
						}
						//
						CASSETTE_RESULT2[ side ][ i ] = Res; // 2011.04.18
					}
					break;
				}
			}
		}
	}
}
//
/*
// 2011.12.14
void SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( int side , int Slot , int data ) {
	int cs , s , i;
	if ( side < 100 ) {
		s = Slot % 100;
		if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue; // 2004.06.15
				if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) <= 0 ) continue; // 2005.07.19
				//
				if ( _i_SCH_CLUSTER_Get_SlotIn( side , i ) == s ) {
					//
					cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
					if ( cs >= CM1 ) { // 2011.04.14
						if ( data != -99 ) CASSETTE_RESULT[ cs ][ s - 1 ] = data;
						//
						if ( data != -99 ) CASSETTE_RESULT2[ side ][ i ] = data; // 2011.04.18
					}
					//
					break;
				}
			}
		}
		s = Slot / 100;
		if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				//
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue; // 2004.06.15
				if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) <= 0 ) continue; // 2005.07.19
				//
				if ( _i_SCH_CLUSTER_Get_SlotIn( side , i ) == s ) {
					cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
					if ( cs >= CM1 ) { // 2011.04.14
						if ( data != -99 ) CASSETTE_RESULT[ cs ][ s - 1 ] = data;
						//
						if ( data != -99 ) CASSETTE_RESULT2[ side ][ i ] = data; // 2011.04.18
					}
					//
					break;
				}
			}
		}
	}
}
*/
// 2011.12.14
void SCHEDULER_CASSETTE_LAST_RESULT_ONLY_SET( int side , int pt , BOOL always , int data ) {
	int cs , s , wres;
	//
	if ( ( side < 0 ) || ( side >= MAX_CASSETTE_SIDE ) ) return;
	if ( ( pt < 0 ) || ( pt >= MAX_CASSETTE_SLOT_SIZE ) ) return;
	//
	if ( !always ) {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , pt ) <= 0 ) return;
		if ( _i_SCH_CLUSTER_Get_PathDo( side , pt ) <= 0 ) return;
	}
	else {
		if ( _i_SCH_CLUSTER_Get_PathRange( side , pt ) < 0 ) return;
	}
	//
	cs = _i_SCH_CLUSTER_Get_PathIn( side , pt );
	//
	if ( cs < CM1 ) return;
	//
	s = _i_SCH_CLUSTER_Get_SlotIn( side , pt );
	wres = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pt , data , 3 );
	//
	if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) CASSETTE_RESULT[ cs ][ s - 1 ] = wres;
	//
	CASSETTE_RESULT2[ side ][ pt ] = wres;
	//
}
//
void SCHEDULER_CASSETTE_LAST_RESULT_SET( int side , int PathIn , int Slot , int ch , int depth , int data , BOOL Gj ) {
	int cs , s , i;
	if ( side < 100 ) {
		s = Slot % 100;
		if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue; // 2004.06.15
				if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) <= 0 ) continue; // 2005.07.19
				if ( _i_SCH_CLUSTER_Get_SlotIn( side , i ) == s ) {
					//
					cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
					//
					if ( PathIn >= CM1 ) { // 2010.05.12
//						if ( ( ( PathIn >= CM1 ) && ( PathIn < PM1 ) ) && ( ( cs >= BM1 ) && ( cs < TM ) ) ) continue; // 2010.05.09 // 2015.04.29
//						if ( ( ( PathIn >= BM1 ) && ( PathIn < TM ) ) && ( ( cs >= CM1 ) && ( cs < PM1 ) ) ) continue; // 2010.05.09 // 2015.04.29
						if ( cs != PathIn ) continue; // 2010.05.09 // 2015.04.29
					}
					//
	//				if ( ( cs >= CM1 ) && ( cs < PM1 ) ) {
					if ( cs >= CM1 ) { // 2011.04.14
						if ( Gj ) {
							if ( data != -99 ) {
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 4 );
							}
						}
						else { // MRES_SUCCESS_LIKE_SUCCESS
							switch( CASSETTE_RESULT[ cs ][ s - 1 ] ) {
							case -1 :
							case -2 :
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 5 );
								break;
							case  0 :
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 6 );
								break;
							case  1 :
								break;
							default : // 2012.03.15
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 7 );
								break;
							}
						}
//						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ch , depth , cs , s , -1 ); // 2007.05.02 // 2011.04.18
	//				}
					}
					//
					break;
				}
			}
		}
		s = Slot / 100;
		if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
				if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) <= 0 ) continue; // 2004.06.15
				if ( _i_SCH_CLUSTER_Get_PathDo( side , i ) <= 0 ) continue; // 2005.07.19
				if ( _i_SCH_CLUSTER_Get_SlotIn( side , i ) == s ) {
					cs = _i_SCH_CLUSTER_Get_PathIn( side , i );
					//
					if ( PathIn >= CM1 ) { // 2010.05.12
//						if ( ( ( PathIn >= CM1 ) && ( PathIn < PM1 ) ) && ( ( cs >= BM1 ) && ( cs < TM ) ) ) continue; // 2010.05.09 // 2015.04.29
//						if ( ( ( PathIn >= BM1 ) && ( PathIn < TM ) ) && ( ( cs >= CM1 ) && ( cs < PM1 ) ) ) continue; // 2010.05.09 // 2015.04.29
						if ( cs != PathIn ) continue; // 2010.05.09 // 2015.04.29
					}
					//
	//				if ( ( cs >= CM1 ) && ( cs < PM1 ) ) {
					if ( cs >= CM1 ) { // 2011.04.14
						if ( Gj ) { // 2002.07.21 (missed before)
							if ( data != -99 ) CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 8 );
						}
						else {
							switch( CASSETTE_RESULT[ cs ][ s - 1 ] ) {
							case -1 :
							case -2 :
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 9 );
								break;
							case  0 :
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 10 );
								break;
							case  1 :
								break;
							default : // 2012.03.15
								CASSETTE_RESULT[ cs ][ s - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , i , data , 11 );
								break;
							}
						}
//						SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ch , depth + 1 , cs , s , -1 ); // 2007.05.02 // 2011.04.18
	//				}
					}
					break;
				}
			}
		}
	}
	else {
		cs = _i_SCH_PRM_GET_MODULE_DUMMY_BUFFER_CHAMBER();
		if ( cs != 0 ) {
			s = (side % 100) + 1 ;
			if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
				if ( Gj ) {
					if ( data != -99 ) CASSETTE_RESULT[ cs ][ s - 1 ] = data;
				}
				else {
					switch( CASSETTE_RESULT[ cs ][ s - 1 ] ) {
					case -1 :
					case -2 :
						CASSETTE_RESULT[ cs ][ s - 1 ] = data;
						break;
					case  0 :
						CASSETTE_RESULT[ cs ][ s - 1 ] = data;
						break;
					case  1 :
						break;
					default : // 2012.03.15
						CASSETTE_RESULT[ cs ][ s - 1 ] = data;
						break;
					}
				}
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( 0 , ch , depth , cs , s , -1 ); // 2007.05.02
			}
		}
	}
}
//
void SCHEDULER_CASSETTE_LAST_RESULT2_SET( int side , int pointer , int ch , int depth , int data , BOOL Gj ) { // 2011.04.27
	int cs;
	int data2 , Gj2;
	//

//_IO_CIM_PRINTF( "LAST_RESULT2 1 [side=%d][pointer=%d][ch=%d][depth=%d][data=%d][Gj=%d]\n" , side , pointer , ch , depth , data , Gj );

	if ( side < 100 ) {
		//
		cs = _i_SCH_CLUSTER_Get_PathIn( side , pointer );
		//
//_IO_CIM_PRINTF( "LAST_RESULT2 2 [side=%d][pointer=%d][ch=%d][depth=%d][data=%d][Gj=%d] [cs=%d]\n" , side , pointer , ch , depth , data , Gj , cs  );
		//
		if ( cs >= CM1 ) {
			//
			// 2011.05.07
			data2	= data;
			Gj2		= Gj;
			//
			if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE( 2 , side , pointer , ch , depth , &data2 , &Gj2 ) ) {
				data = data2;
				Gj   = Gj2;
			}
			//
//_IO_CIM_PRINTF( "LAST_RESULT2 3 [side=%d][pointer=%d][ch=%d][depth=%d][data=%d][Gj=%d] [cs=%d] [%d]\n" , side , pointer , ch , depth , data , Gj , cs , CASSETTE_RESULT2[ side ][ pointer ] );
			//
			if ( Gj ) {
				if ( data != -99 ) CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 16 );
			}
			else {
				switch( CASSETTE_RESULT2[ side ][ pointer ] ) {
				case -1 :
				case -2 :
					CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 17 );
					break;
				case  0 : // success
					CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 18 );
					break;
				case  1 : // fail // 2011.07.12
					if ( data == 0 ) CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 19 ); // 2011.07.12
					break;
				default : // 2012.03.15
					CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 20 );
					break;
				}
				//
			}
//_IO_CIM_PRINTF( "LAST_RESULT2 4 [side=%d][pointer=%d][ch=%d][depth=%d][data=%d][Gj=%d] [cs=%d] [%d]\n" , side , pointer , ch , depth , data , Gj , cs , CASSETTE_RESULT2[ side ][ pointer ] );
			//
			SCHEDULER_CASSETTE_WAFER_RESULT2_SET_FOR_IO( 0 , ch , depth , cs , 0 , side , pointer , -1 );
			//
			// 2011.05.07
			data2	= data;
			Gj2		= Gj;
			//
			if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE( 3 , side , pointer , ch , depth , &data2 , &Gj2 ) ) {
				data = data2;
				Gj   = Gj2;
				//
				//
				if ( Gj ) {
					if ( data != -99 ) CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 21 );
				}
				else {
					switch( CASSETTE_RESULT2[ side ][ pointer ] ) {
					case -1 :
					case -2 :
						CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 22 );
						break;
					case  0 :
						CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 23 );
						break;
					case  1 : // fail // 2012.03.15
						if ( data == 0 ) CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 24 ); // 2011.07.12
						break;
					default : // 2012.03.15
						CASSETTE_RESULT2[ side ][ pointer ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( side , pointer , data , 25 );
						break;
					}
					//
				}
				//
			}
			//
		}
	}
}
//
int _i_SCH_CASSETTE_LAST_RESULT_GET( int cs , int s ) {
	//
	int i , j;
	//
	// 2011.08.25
	for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( i ) ) continue;
		//
		for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( i , j ) < 0 ) continue;
			//
			if ( _i_SCH_CLUSTER_Get_PathIn( i , j ) != cs ) continue;
			if ( _i_SCH_CLUSTER_Get_SlotIn( i , j ) != s ) continue;
			//
			return( CASSETTE_RESULT2[ i ][ j ] );
		}
	}
	//
	if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
//		if ( ( cs >= CM1 ) && ( cs < PM1 ) ) {
			return( CASSETTE_RESULT[ cs ][ s - 1 ] );
//		}
	}
	return -1;
}
//
int _i_SCH_CASSETTE_LAST_RESULT2_GET( int s , int p ) {
	if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) ) {
		if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
			return( CASSETTE_RESULT2[ s ][ p ] );
		}
	}
	return -1;
}
//

int _i_SCH_CASSETTE_LAST_RESULT_WITH_CASS_GET( int cs , int s ) { // 2007.07.13
	switch( _i_SCH_CASSETTE_LAST_RESULT_GET( cs , s ) ) {
	case 0  : return CWM_PROCESS; break;
	case 1  : return CWM_FAILURE; break;
	default : return CWM_PRESENT; break;
	}
}

int _i_SCH_CASSETTE_LAST_RESULT2_WITH_CASS_GET( int s , int p ) { // 2011.04.18
	switch( _i_SCH_CASSETTE_LAST_RESULT2_GET( s , p ) ) {
	case 0  : return CWM_PROCESS; break;
	case 1  : return CWM_FAILURE; break;
	default : return CWM_PRESENT; break;
	}
}

void SCHEDULER_CASSETTE_LAST_RESULT_INIT_AFTER_CM_PICK( int cs , int s , int side , int pt ) {
	if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
//		if ( ( cs >= CM1 ) && ( cs < PM1 ) ) {
			if ( CASSETTE_RESULT[ cs ][ s - 1 ] < 0 )
				CASSETTE_RESULT[ cs ][ s - 1 ] = -2;
//		}
	}
	if ( ( side >= 0 ) && ( side < MAX_CASSETTE_SIDE ) ) { // 2011.04.21
		if ( ( pt >= 0 ) && ( pt < MAX_CASSETTE_SLOT_SIZE ) ) {
			if ( CASSETTE_RESULT2[ side ][ pt ] < 0 )
				CASSETTE_RESULT2[ side ][ pt ] = -2;
		}
	}
}
//
void _i_SCH_CASSETTE_LAST_RESET( int cs , int s ) {
	int i , j;
	if ( cs == -1 ) {
		for ( j = 0 ; j < MAX_CHAMBER ; j++ ) {
//		for ( j = CM1 ; j < PM1 ; j++ ) {
			for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) CASSETTE_RESULT[ j ][ i ] = -1;
		}
	}
	else {
//		if ( ( cs >= CM1 ) && ( cs < PM1 ) ) { // 2003.06.21
			if ( s <= 0 ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) CASSETTE_RESULT[ cs ][ i ] = -1;
			}
			else {
				if ( ( s > 0 ) && ( s <= MAX_CASSETTE_SLOT_SIZE ) ) {
					CASSETTE_RESULT[ cs ][ s - 1 ] = -1;
				}
			}
//		} // 2003.06.21
	}
}
//
void _i_SCH_CASSETTE_LAST_RESET2( int s , int p ) {
	int i , j;
	if ( s == -1 ) {
		for ( j = 0 ; j < MAX_CASSETTE_SIDE ; j++ ) {
			//
			if ( p == -1 ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) CASSETTE_RESULT2[ j ][ i ] = -1;
			}
			else {
				if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
					CASSETTE_RESULT2[ j ][ p ] = -1;
				}
			}
		}
	}
	else {
		if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) ) { // 2011.04.21
			if ( p == -1 ) {
				for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) CASSETTE_RESULT2[ s ][ i ] = -1;
			}
			else {
				if ( ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) {
					CASSETTE_RESULT2[ s ][ p ] = -1;
				}
			}
		}
	}
}
//
//-----------------------------
//	WAFERRESULT_PRESENT ,
//	WAFERRESULT_SUCCESS ,
//	WAFERRESULT_FAILURE ,
//	WAFERRESULT_PROCESSING ,
//-----------------------------

void SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( int srccass , int srcslot , int wres ) {
	//
	switch( wres ) {
	case WAFERRESULT_SUCCESS :
		CASSETTE_RESULT[ srccass ][ srcslot ] = 0;
		break;
	case WAFERRESULT_FAILURE :
		CASSETTE_RESULT[ srccass ][ srcslot ] = 1;
		break;
	default :
		if ( wres >= WAFERRESULT_USER ) {
			CASSETTE_RESULT[ srccass ][ srcslot ] = 2 + ( wres - WAFERRESULT_USER );
		}
		else {
			CASSETTE_RESULT[ srccass ][ srcslot ] = -1;
		}
		break;
	}
}

void SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( int srccass , int srcslot , int station , int slot ) {
	if ( srccass == 1001 ) {
		switch( srcslot ) {
		case 0 :
			_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_SUCCESS );
			break;
		case 1 :
			_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_FAILURE );
			break;
		default :
			if ( srcslot >= 2 ) {
				_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_USER + srcslot - 2 );
			}
			else {
				_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PRESENT );
			}
			break;
		}
	}
	else {
		if ( srcslot >= 0 ) {
			switch( CASSETTE_RESULT[ srccass ][ srcslot ] ) {
			case 0 :
				_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_SUCCESS );
				break;
			case 1 :
				_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_FAILURE );
				break;
			default :
				if ( CASSETTE_RESULT[ srccass ][ srcslot ] >= 2 ) {
					_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot ] - 2 );
				}
				else {
					_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PRESENT );
				}
				break;
			}
		}
	}
}


void SCHEDULER_CASSETTE_WAFER_RESULT_DATA_SET_FROM_IO( int mode , int station , int slot , int srccass , int srcslot , int TMATM , int Arm ) {
	int srccass2 , srcslot2;
	//
	srccass2 = srccass; // 2010.11.23
	srcslot2 = srcslot; // 2010.11.23
	if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE( 0 , mode , TMATM , station , slot , &srccass2 , &srcslot2 ) ) { // 2010.11.23
		srccass = srccass2; // 2010.11.23
		srcslot = srcslot2; // 2010.11.23
	} // 2010.11.23
	//
	switch( mode ) {
	case 0 :
		if      ( ( station >= CM1 ) && ( station < PM1 ) ) {
			if ( srccass >= 0 ) {
				if ( srcslot > 0 ) {
					switch( _i_SCH_IO_GET_MODULE_STATUS( station , slot ) ) {
					case CWM_UNKNOWN :
					case CWM_ABSENT :
					case CWM_PRESENT :
//						CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = -1; // 2012.08.29
						CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( -1 , ( station * 1000 ) + slot , -1 , Arm ); // 2012.08.29
						break;
					case CWM_PROCESS :
//						CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = 0; // 2012.08.29
						CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( -1 , ( station * 1000 ) + slot , 0 , Arm ); // 2012.08.29
						break;
					default :
//						CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = 1; // 2012.08.29
						CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( -1 , ( station * 1000 ) + slot , 1 , Arm ); // 2012.08.29
						break;
					}
				}
			}
		}
		else if ( ( station >= BM1 ) && ( station < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
			if ( srccass >= 0 ) {
				if ( srcslot > 0 ) {
					//
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , _i_SCH_IO_GET_MODULE_RESULT( station , slot ) );
					//
				}
			}
		}
		else if ( station == F_AL ) { // 2013.03.20
			if ( srccass >= 0 ) {
				if ( srcslot > 0 ) {
					//
					if ( slot <= 1 ) {
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , _i_SCH_IO_GET_MODULE_RESULT( F_AL , slot ) );
					}
					else {
						SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , _i_SCH_IO_GET_MODULE_RESULT( 1001 , slot ) );
					}
					//
				}
			}
		}
		else if ( station == F_IC ) {
			if ( srccass >= 0 ) {
				if ( srcslot > 0 ) {
					//
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , _i_SCH_IO_GET_MODULE_RESULT( F_IC , slot ) );
					//
				}
			}
		}
		else {
			if ( srccass >= 0 ) {
				if ( slot == 0 ) slot = 1;
				if ( srcslot > 0 ) {
					//
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , _i_SCH_IO_GET_MODULE_RESULT( station , slot ) );
					//
				}
			}
		}
		break;
	case 1 :
		if ( srccass >= 0 ) {
			if ( srcslot > 0 ) {
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , WAFER_RESULT_IN_FM( 0 , station ) );
				//
			}
		}
		break;
	case 11 :
		if ( srccass >= 0 ) {
			if ( srcslot > 0 ) {
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , WAFER_RESULT_IN_FM( 1 , station ) );
				//
			}
		}
		break;
	case 2 :
		if ( srccass >= 0 ) {
			if ( srcslot > 0 ) {
				//
//				SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , WAFER_RESULT_IN_TM( TMATM , station ) ); // 2015.05.27
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , WAFER_RESULT_IN_TM( TMATM , station , slot ) ); // 2015.05.27
				//
			}
		}
		break;
	default :
		if ( srccass >= 0 ) {
			if ( srcslot > 0 ) {
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_SUB( srccass , srcslot - 1 , TMATM );
				//
			}
		}
		break;
	}
}
//-----------------------------
BOOL SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( int s , int p , int w , int *res ) { // 2012.03.18
	int R;
	//
	if ( w <= 0 ) return FALSE;
	//
	if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) || ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) return FALSE;
	//
	R = _SCH_COMMON_ANIMATION_RESULT_UPDATE( s , p , 99 , 99 );
	//
	if ( R <=  1 ) {
		*res = WAFERRESULT_PROCESSING;
		return TRUE;
	}
	if ( R == 99 ) {
		*res = WAFERRESULT_PROCESSING;
		return TRUE;
	}
	//
	*res = R + 2;
	return TRUE;
}
//
void SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_IO( int mode , int station , int slot , int srccass , int srcslot , int TMATM ) {
	int xp , res; // 2007.05.02

//_i_SCH_LOG_LOT_PRINTF( 0 , "mode=%d station=%d slot=%d srccass=%d srcslot=%d TMATM=%d\n" , mode , station , slot , srccass , srcslot , TMATM );
	int srccass2 , srcslot2;
	//
	srccass2 = srccass; // 2010.11.23
	srcslot2 = srcslot; // 2010.11.23
	if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE( 1 , mode , TMATM , station , slot , &srccass2 , &srcslot2 ) ) { // 2010.11.23
		srccass = srccass2; // 2010.11.23
		srcslot = srcslot2; // 2010.11.23
	} // 2010.11.23
	//

	switch( mode ) {
	case 0 :
		if      ( ( station >= CM1 ) && ( station < PM1 ) ) { // 2008.02.26
			if ( srccass == 1001 ) {
				switch( srcslot ) {
				case 0 :
					_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PROCESS );
					break;
				case 1 :
					_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_FAILURE );
					break;
				default :
					_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PRESENT );
					break;
				}
			}
			else if ( srccass >= 0 ) {
				if ( srcslot > 0 ) {
					switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
					case 0 :
						_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PROCESS );
						break;
					case 1 :
						_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_FAILURE );
						break;
					default :
						_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PRESENT );
						break;
					}
				}
			}
		}
		else if ( ( station >= BM1 ) && ( station < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
			if ( srccass < 0 ) {
//				_i_SCH_IO_SET_MODULE_RESULT( station , slot , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_BM_SIDE( station , slot ) , _i_SCH_MODULE_Get_BM_POINTER( station , slot ) , _i_SCH_MODULE_Get_BM_WAFER( station , slot ) ) ); // 2012.11.14
				if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_BM_SIDE( station , slot ) , _i_SCH_MODULE_Get_BM_POINTER( station , slot ) , _i_SCH_MODULE_Get_BM_WAFER( station , slot ) , &res ) ) { // 2012.11.14
					_i_SCH_IO_SET_MODULE_RESULT( station , slot , res ); // 2012.11.14
				}
				// 2013.06.19
//				else { // 2013.04.29
//					if ( _i_SCH_IO_GET_MODULE_STATUS( station , slot ) > 0 ) {
//						_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PROCESSING );
//					}
//				}
			}
			else {
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( srccass , srcslot - 1 , station , slot );
			}
		}
		else if ( station == F_AL ) { // 2013.03.20
			if ( slot <= 1 ) {
				if ( srccass < 0 ) {
					_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , 3 );
				}
				else {
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( srccass , srcslot - 1 , station , slot );
				}
			}
			else {
				if ( srccass < 0 ) {
					_i_SCH_IO_SET_MODULE_RESULT( 1001 , slot , 3 );
				}
				else {
					SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( srccass , srcslot - 1 , 1001 , slot );
				}
			}
		}
		else if ( station == F_IC ) {
			if ( srccass < 0 ) {
				_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , 3 );
			}
			else {
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( srccass , srcslot - 1 , station , slot );
			}
		}
		else {
			if ( srccass == -2 ) { // 2016.04.26
				if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , slot-1 ) ) > 0 ) {
					if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , slot-1 ) , _i_SCH_MODULE_Get_PM_POINTER( station , slot-1 ) , _i_SCH_MODULE_Get_PM_WAFER( station , slot-1 ) , &res ) ) {
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , res );
					}
				}
			}
			else if ( srccass < 0 ) {
				for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH; xp++ ) { // 2007.05.02
//					_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) ) ); // 2012.11.14
//
					// 2013.06.19
//					if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) , &res ) ) { // 2012.11.14
//						_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , res ); // 2012.11.14
//					}
//					else { // 2013.04.29
//						if ( _i_SCH_IO_GET_MODULE_STATUS( station , xp + 1 ) > 0 ) {
//							_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , WAFERRESULT_PROCESSING );
//						}
//					}
//
					// 2013.06.19
					if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , xp ) % 100 ) > 0 ) {
						if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) % 100 , &res ) ) {
							_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , res );
						}
					}
					if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , xp ) / 100 ) > 0 ) {
						if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp + 1 ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp + 1 ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) / 100 , &res ) ) {
							_i_SCH_IO_SET_MODULE_RESULT( station , xp + 2 , res );
						}
						xp++;
					}
					//
				}
			}
			else if ( srccass == 1001 ) { // 2011.05.07
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( srccass , srcslot - 1 , station , slot );
			}
			else {
				if ( slot == 0 ) slot = 1;
				//
				SCHEDULER_CASSETTE_WAFER_RESULT_SET_IO_SUB( srccass , srcslot - 1 , station , slot );
				//
			}
		}
		break;
	case 1 :
		if ( srccass < 0 ) {
			WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PROCESSING );
		}
		else if ( srccass == 1001 ) { // 2011.05.07
			switch( srcslot ) {
			case 0 :
				WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_SUCCESS );
				break;
			case 1 :
				WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_FAILURE );
				break;
			default :
				if ( srcslot >= 2 ) {
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_USER + srcslot - 2 );
				}
				else {
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PRESENT );
				}
				break;
			}
		}
		else {
			if ( srcslot > 0 ) { // 2003.10.21
				switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
				case 0 :
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_SUCCESS );
					break;
				case 1 :
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_FAILURE );
					break;
				default :
					if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
						WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
					}
					else {
						WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PRESENT );
					}
					break;
				}
			}
			else if ( srcslot == 0 ) { // 2008.02.26
				WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PRESENT );
			}
		}
		break;
	case 11 : // 2007.06.08
		if ( srccass < 0 ) {
			WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PROCESSING );
		}
		else if ( srccass == 1001 ) { // 2011.05.07
			switch( srcslot ) {
			case 0 :
				WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_SUCCESS );
				break;
			case 1 :
				WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_FAILURE );
				break;
			default :
				if ( srcslot >= 2 ) {
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_USER + srcslot- 2 );
				}
				else {
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PRESENT );
				}
				break;
			}
		}
		else {
			if ( srcslot > 0 ) { // 2003.10.21
				switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
				case 0 :
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_SUCCESS );
					break;
				case 1 :
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_FAILURE );
					break;
				default :
					if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
						WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
					}
					else {
						WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PRESENT );
					}
					break;
				}
			}
			else if ( srcslot == 0 ) { // 2008.02.26
				WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PRESENT );
			}
		}
		break;
	case 2 :
		if ( srccass < 0 ) {
//			WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PROCESSING ); // 2003.10.21 // 2015.05.27
			WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PROCESSING ); // 2003.10.21 // 2015.05.27
		}
		else if ( srccass == 1001 ) { // 2011.05.07
			switch( srcslot ) {
			case 0 :
//				WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_SUCCESS ); // 2015.05.27
				WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_SUCCESS ); // 2015.05.27
				break;
			case 1 :
//				WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_FAILURE ); // 2015.05.27
				WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_FAILURE ); // 2015.05.27
				break;
			default :
				if ( srcslot >= 2 ) {
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_USER + srcslot- 2 ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_USER + srcslot- 2 ); // 2015.05.27
				}
				else {
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PRESENT ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PRESENT ); // 2015.05.27
				}
				break;
			}
		}
		else {
			if ( srcslot > 0 ) { // 2003.10.21
				switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
				case 0 :
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_SUCCESS ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_SUCCESS ); // 2015.05.27
					break;
				case 1 :
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_FAILURE ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_FAILURE ); // 2015.05.27
					break;
				default :
					if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
//						WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ]- 2 ); // 2015.05.27
						WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ]- 2 ); // 2015.05.27
					}
					else {
//						WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PRESENT ); // 2015.05.27
						WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PRESENT ); // 2015.05.27
					}
					break;
				}
			}
			else if ( srcslot == 0 ) { // 2008.02.26
//				WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PRESENT ); // 2015.05.27
				WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PRESENT ); // 2015.05.27
			}
		}
		break;
	}
}


void SCHEDULER_CASSETTE_WAFER_RESULT2_SET_FOR_IO( int mode , int station , int slot , int srccass , int srcslot , int s , int p , int TMATM ) {
	int xp , res; // 2007.05.02

//_i_SCH_LOG_LOT_PRINTF( 0 , "mode=%d station=%d slot=%d srccass=%d srcslot=%d TMATM=%d\n" , mode , station , slot , srccass , srcslot , TMATM );
// 2011.05.07
//	int side2 , pointer2;
//	//
//	side2    = s;
//	pointer2 = p;
//	if ( _SCH_COMMON_ANIMATION_SOURCE_UPDATE( 1 , mode , TMATM , station , slot , &side2 , &pointer2 ) ) { // 2010.11.23
//		s = side2; // 2010.11.23
//		p = pointer2; // 2010.11.23
//	} // 2010.11.23
	//

	switch( mode ) {
	case 0 :
		if      ( ( station >= CM1 ) && ( station < PM1 ) ) { // 2008.02.26
			if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
				switch( CASSETTE_RESULT2[ s ][ p ] ) {
				case 0 :
					_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PROCESS );
					break;
				case 1 :
					_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_FAILURE );
					break;
				default :
					_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PRESENT );
					break;
				}
			}
			else {
				if ( srccass >= 0 ) {
					if ( srcslot > 0 ) {
						switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
						case 0 :
							_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PROCESS );
							break;
						case 1 :
							_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_FAILURE );
							break;
						default :
							_i_SCH_IO_SET_MODULE_STATUS( station , slot , CWM_PRESENT );
							break;
						}
					}
				}
			}
		}
		else if ( ( station >= BM1 ) && ( station < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
			if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
				if ( srccass < 0 ) {
//					_i_SCH_IO_SET_MODULE_RESULT( station , slot , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 ) ); // 2012.11.14
					if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 , &res ) ) { // 2012.11.14
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , res ); // 2012.11.14
					}
				}
				else {
					switch( CASSETTE_RESULT2[ s ][ p ] ) {
					case 0 :
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_SUCCESS );
						break;
					case 1 :
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_FAILURE );
						break;
					default :
						if ( CASSETTE_RESULT2[ s ][ p ] >= 2 ) {
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 );
						}
						else {
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PRESENT );
						}
						break;
					}
				}
			}
			else {
				if ( srccass < 0 ) {
//					_i_SCH_IO_SET_MODULE_RESULT( station , slot , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_BM_SIDE( station , slot ) , _i_SCH_MODULE_Get_BM_POINTER( station , slot ) , _i_SCH_MODULE_Get_BM_WAFER( station , slot ) ) ); // 2012.11.14
					if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_BM_SIDE( station , slot ) , _i_SCH_MODULE_Get_BM_POINTER( station , slot ) , _i_SCH_MODULE_Get_BM_WAFER( station , slot ) , &res ) ) { // 2012.11.14
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , res ); // 2012.11.14
					}
				}
				else {
					if ( srcslot > 0 ) { // 2003.10.21
						switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
						case 0 :
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_SUCCESS );
							break;
						case 1 :
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_FAILURE );
							break;
						default :
							if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
								_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
							}
							else {
								_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PRESENT );
							}
							break;
						}
					}
				}
			}
		}
		else if ( station == F_IC ) {
			if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
				if ( srccass < 0 ) {
					_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , 3 );
				}
				else {
					switch( CASSETTE_RESULT2[ s ][ p ] ) {
					case 0 :
						_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_SUCCESS );
						break;
					case 1 :
						_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_FAILURE );
						break;
					default :
						if ( CASSETTE_RESULT2[ s ][ p ] >= 2 ) {
							_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 );
						}
						else {
							_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_PRESENT );
						}
						break;
					}
				}
			}
			else {
				if ( srccass < 0 ) {
					_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , 3 );
				}
				else {
					if ( srcslot > 0 ) { // 2003.10.21
						switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
						case 0 :
							_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_SUCCESS );
							break;
						case 1 :
							_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_FAILURE );
							break;
						default :
							if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
								_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
							}
							else {
								_i_SCH_IO_SET_MODULE_RESULT( F_IC , slot , WAFERRESULT_PRESENT );
							}
							break;
						}
					}
				}
			}
		}
		else {
			if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
				if ( srccass < 0 ) {
					for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH; xp++ ) { // 2007.05.02
//						_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) ) ); // 2012.11.14
//
						// 2013.06.19
//						if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) , &res ) ) { // 2012.11.14
//							_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , res ); // 2012.11.14
//						}
//						else { // 2013.04.29
//							if ( _i_SCH_IO_GET_MODULE_STATUS( station , xp + 1 ) > 0 ) {
//								_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , WAFERRESULT_PROCESSING );
//							}
//						}
						// 2013.06.19
						if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , xp ) % 100 ) > 0 ) {
							if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) % 100 , &res ) ) {
								_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , res );
							}
						}
						if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , xp ) / 100 ) > 0 ) {
							if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp + 1 ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp + 1 ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) / 100 , &res ) ) {
								_i_SCH_IO_SET_MODULE_RESULT( station , xp + 2 , res );
							}
							xp++;
						}
						//
					}
				}
				else {
					switch( CASSETTE_RESULT2[ s ][ p ] ) {
					case 0 :
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_SUCCESS );
						break;
					case 1 :
						_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_FAILURE );
						break;
					default :
						if ( CASSETTE_RESULT2[ s ][ p ] >= 2 ) {
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 );
						}
						else {
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PRESENT );
						}
						break;
					}
				}
			}
			else {
				if ( srccass < 0 ) {
					for ( xp = 0 ; xp < MAX_PM_SLOT_DEPTH; xp++ ) { // 2007.05.02
//						_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS(  _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) ) ); // 2012.11.14
						// 2013.06.19
//						if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS(  _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) , &res ) ) { // 2012.11.14
//							_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , res ); // 2012.11.14
//						}
//						else { // 2013.04.29
//							if ( _i_SCH_IO_GET_MODULE_STATUS( station , xp + 1 ) > 0 ) {
//								_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , WAFERRESULT_PROCESSING );
//							}
//						}
						// 2013.06.19
						if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , xp ) % 100 ) > 0 ) {
							if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) % 100 , &res ) ) {
								_i_SCH_IO_SET_MODULE_RESULT( station , xp + 1 , res );
							}
						}
						if ( ( _i_SCH_MODULE_Get_PM_WAFER( station , xp ) / 100 ) > 0 ) {
							if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( _i_SCH_MODULE_Get_PM_SIDE( station , xp + 1 ) , _i_SCH_MODULE_Get_PM_POINTER( station , xp + 1 ) , _i_SCH_MODULE_Get_PM_WAFER( station , xp ) / 100 , &res ) ) {
								_i_SCH_IO_SET_MODULE_RESULT( station , xp + 2 , res );
							}
							xp++;
						}
						//
					}
				}
				else {
					if ( slot == 0 ) slot = 1;
					if ( srcslot > 0 ) {
						switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
						case 0 :
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_SUCCESS );
							break;
						case 1 :
							_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_FAILURE );
							break;
						default :
							if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
								_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
							}
							else {
								_i_SCH_IO_SET_MODULE_RESULT( station , slot , WAFERRESULT_PRESENT );
							}
							break;
						}
					}
				}
			}
		}
		break;
	case 1 :
		if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
			if ( srccass < 0 ) {
//				WAFER_RESULT_SET_FM( 0 , station , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 ) ); // 2012.11.14
				if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 , &res ) ) { // 2012.11.14
					WAFER_RESULT_SET_FM( 0 , station , res ); // 2012.11.14
				}
			}
			else {
				switch( CASSETTE_RESULT2[ s ][ p ] ) {
				case 0 :
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_SUCCESS );
					break;
				case 1 :
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_FAILURE );
					break;
				default :
					if ( CASSETTE_RESULT2[ s ][ p ] >= 2 ) {
						WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 );
					}
					else {
						WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PRESENT );
					}
					break;
				}
			}
		}
		else {
			if ( srccass < 0 ) {
				WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PROCESSING );
			}
			else {
				if ( srcslot > 0 ) { // 2003.10.21
					switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
					case 0 :
						WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_SUCCESS );
						break;
					case 1 :
						WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_FAILURE );
						break;
					default :
						if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
							WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
						}
						else {
							WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PRESENT );
						}
						break;
					}
				}
				else if ( srcslot == 0 ) { // 2008.02.26
					WAFER_RESULT_SET_FM( 0 , station , WAFERRESULT_PRESENT );
				}
			}
		}
		break;
	case 11 : // 2007.06.08
		if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
			if ( srccass < 0 ) {
//				WAFER_RESULT_SET_FM( 1 , station , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 ) ); // 2012.11.14
				if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 , &res ) ) { // 2012.11.14
					WAFER_RESULT_SET_FM( 1 , station , res ); // 2012.11.14
				}
			}
			else {
				switch( CASSETTE_RESULT2[ s ][ p ] ) {
				case 0 :
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_SUCCESS );
					break;
				case 1 :
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_FAILURE );
					break;
				default :
					if ( CASSETTE_RESULT2[ s ][ p ] >= 2 ) {
						WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 );
					}
					else {
						WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PRESENT );
					}
					break;
				}
			}
		}
		else {
			if ( srccass < 0 ) {
				WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PROCESSING );
			}
			else {
				if ( srcslot > 0 ) { // 2003.10.21
					switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
					case 0 :
						WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_SUCCESS );
						break;
					case 1 :
						WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_FAILURE );
						break;
					default :
						if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
							WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
						}
						else {
							WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PRESENT );
						}
						break;
					}
				}
				else if ( srcslot == 0 ) { // 2008.02.26
					WAFER_RESULT_SET_FM( 1 , station , WAFERRESULT_PRESENT );
				}
			}
		}
		break;
	case 2 :
		if ( ( s >= 0 ) && ( p >= 0 ) ) { // 2011.04.18
			if ( srccass < 0 ) {
//				WAFER_RESULT_SET_TM( TMATM , station , SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 ) ); // 2003.10.21 // 2012.11.14
				if ( SCHEDULER_CASSETTE_GET_PROCESSING_IO_STATUS( s , p , 1 , &res ) ) { // 2003.10.21 // 2012.11.14
//					WAFER_RESULT_SET_TM( TMATM , station , res ); // 2003.10.21 // 2012.11.14 // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , res ); // 2003.10.21 // 2012.11.14 // 2015.05.27
				}
			}
			else {
				switch( CASSETTE_RESULT2[ s ][ p ] ) {
				case 0 :
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_SUCCESS ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_SUCCESS ); // 2015.05.27
					break;
				case 1 :
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_FAILURE ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_FAILURE ); // 2015.05.27
					break;
				default :
					if ( CASSETTE_RESULT2[ s ][ p ] >= 2 ) {
//						WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 ); // 2015.05.27
						WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_USER + CASSETTE_RESULT2[ s ][ p ] - 2 ); // 2015.05.27
					}
					else {
//						WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PRESENT ); // 2015.05.27
						WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PRESENT ); // 2015.05.27
					}
					break;
				}
			}
		}
		else {
			if ( srccass < 0 ) {
//				WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PROCESSING ); // 2003.10.21 // 2015.05.27
				WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PROCESSING ); // 2003.10.21 // 2015.05.27
			}
			else {
				if ( srcslot > 0 ) { // 2003.10.21
					switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
					case 0 :
//						WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_SUCCESS ); // 2015.05.27
						WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_SUCCESS ); // 2015.05.27
						break;
					case 1 :
//						WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_FAILURE ); // 2015.05.27
						WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_FAILURE ); // 2015.05.27
						break;
					default :
						if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
//							WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 ); // 2015.05.27
							WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 ); // 2015.05.27
						}
						else {
//							WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PRESENT ); // 2015.05.27
							WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PRESENT ); // 2015.05.27
						}
						break;
					}
				}
				else if ( srcslot == 0 ) { // 2008.02.26
//					WAFER_RESULT_SET_TM( TMATM , station , WAFERRESULT_PRESENT ); // 2015.05.27
					WAFER_RESULT_SET_TM( TMATM , station , slot , WAFERRESULT_PRESENT ); // 2015.05.27
				}
			}
		}
		break;
	}
}
//-----------------------------
int SCHEDULER_CASSETTE_WAFER_RESULT_SET_FOR_DIRECT( int srccass , int srcslot , int station , int subslot , int data , int s , int p ) {
	if ( srccass < CM1 ) return 1;
	if ( srccass == AL ) return 2;
	if ( srccass == IC ) return 3;
	if ( srccass >= TM ) return 4;
	//
	if ( srcslot < 1                      ) return 11;
	if ( srcslot > MAX_CASSETTE_SLOT_SIZE ) return 12;
	//
	if ( station != 0 ) {
//		if ( station < PM1 ) return 7; // 2007.11.30
		if ( station == AL ) return 21;
		if ( station == IC ) return 22;
		if ( station >  FM ) return 23;
		//
		if      ( station < PM1 ) { // 2007.11.30
			if ( subslot < 1                      ) return 31;
			if ( subslot > MAX_CASSETTE_SLOT_SIZE ) return 32;
		}
		else if ( ( station >= PM1 ) && ( station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
			if ( subslot == 0 ) subslot = 1; // 2012.03.18
			if ( subslot < 1                 ) return 41; // 2012.03.18
			if ( subslot > MAX_PM_SLOT_DEPTH ) return 42; // 2012.03.18
			//
			if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) || ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
				//
				if ( ( subslot == 2 ) && ( ( _i_SCH_MODULE_Get_PM_WAFER( station , 0 ) / 100 ) > 0 ) ) { // 2015.06.10
					s = _i_SCH_MODULE_Get_PM_SIDE( station , subslot - 1 );
					p = _i_SCH_MODULE_Get_PM_POINTER( station , subslot - 1 );
				}
				else {
					if ( _i_SCH_MODULE_Get_PM_WAFER( station , subslot - 1 ) > 0 ) {
						s = _i_SCH_MODULE_Get_PM_SIDE( station , subslot - 1 );
						p = _i_SCH_MODULE_Get_PM_POINTER( station , subslot - 1 );
					}
				}
			}
			//
		}
		else if ( ( station >= BM1 ) && ( station < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
			if ( subslot < 1                      ) return 51;
			if ( subslot > MAX_CASSETTE_SLOT_SIZE ) return 52;
			//
			if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) || ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
				if ( _i_SCH_MODULE_Get_BM_WAFER( station , subslot ) > 0 ) {
					s = _i_SCH_MODULE_Get_BM_SIDE( station , subslot );
					p = _i_SCH_MODULE_Get_BM_POINTER( station , subslot );
				}
			}
			//
		}
		else if ( ( station >= TM  ) && ( station < ( TM + MAX_TM_CHAMBER_DEPTH ) ) ) {
			if ( subslot < 0 ) return 61;
			if ( subslot > 1 ) return 62;
			//
			if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) || ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
				if ( _i_SCH_MODULE_Get_TM_WAFER( station - TM , subslot ) > 0 ) {
					s = _i_SCH_MODULE_Get_TM_SIDE( station - TM , subslot );
					p = _i_SCH_MODULE_Get_TM_POINTER( station - TM , subslot );
				}
			}
			//
		}
		else if (   station == FM  ) {
			if ( subslot < 0 ) return 71;
			if ( subslot > 9 ) return 72;
			//
			if ( ( s < 0 ) || ( s >= MAX_CASSETTE_SIDE ) || ( p < 0 ) || ( p >= MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
				if ( _i_SCH_MODULE_Get_FM_WAFER( subslot ) > 0 ) {
					s = _i_SCH_MODULE_Get_FM_SIDE( subslot );
					p = _i_SCH_MODULE_Get_FM_POINTER( subslot );
				}
			}
			//
		}
	}
	//
//	if ( ( data >= -2 ) && ( data <= 1 ) ) { // 2012.03.15
	if ( data >= -2 ) { // 2012.03.15
		if ( ( s >= 0 ) && ( s < MAX_CASSETTE_SIDE ) && ( p >= 0 ) && ( p < MAX_CASSETTE_SLOT_SIZE ) ) { // 2012.03.16
			CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = _SCH_COMMON_ANIMATION_RESULT_UPDATE( s , p , data , -1 );
			CASSETTE_RESULT2[ s ][ p ] = CASSETTE_RESULT[ srccass ][ srcslot - 1 ];
		}
		else {
			CASSETTE_RESULT[ srccass ][ srcslot - 1 ] = data;
		}
	}
	//
	if ( station != 0 ) {
		if      ( station < PM1 ) { // 2007.11.30
			switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
			case 0 :
				_i_SCH_IO_SET_MODULE_STATUS( station , subslot , CWM_PROCESS );
				break;
			case 1 :
				_i_SCH_IO_SET_MODULE_STATUS( station , subslot , CWM_FAILURE );
				break;
			default : // 2012.03.21
				_i_SCH_IO_SET_MODULE_STATUS( station , subslot , CWM_PRESENT );
				break;
			}
		}
		else if ( ( station >= PM1 ) && ( station < ( PM1 + MAX_PM_CHAMBER_DEPTH ) ) ) {
			switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
			case 0 :
				_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_SUCCESS ); // 2012.03.18
				break;
			case 1 :
				_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_FAILURE ); // 2012.03.18
				break;
			default :
				if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
					_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
				}
				else {
					_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_PRESENT ); // 2012.03.18
				}
				break;
			}
		}
		else if ( ( station >= BM1 ) && ( station < ( BM1 + MAX_BM_CHAMBER_DEPTH ) ) ) {
			switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
			case 0 :
				_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_SUCCESS );
				break;
			case 1 :
				_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_FAILURE );
				break;
			default :
				if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
					_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
				}
				else {
					_i_SCH_IO_SET_MODULE_RESULT( station , subslot , WAFERRESULT_PRESENT );
				}
				break;
			}
		}
		else if ( ( station >= TM  ) && ( station < ( TM + MAX_TM_CHAMBER_DEPTH ) ) ) {
			/*
			// 2015.05.27
			switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
			case 0 :
				WAFER_RESULT_SET_TM( station - TM , subslot , WAFERRESULT_SUCCESS );
				break;
			case 1 :
				WAFER_RESULT_SET_TM( station - TM , subslot , WAFERRESULT_FAILURE );
				break;
			default :
				if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
					WAFER_RESULT_SET_TM( station - TM , subslot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
				}
				else {
					WAFER_RESULT_SET_TM( station - TM , subslot , WAFERRESULT_PRESENT );
				}
				break;
			}
			*/
			//
			// 2015.05.27
			//
			if ( ( subslot / 100 ) > 0 ) {
				switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
				case 0 :
					WAFER_RESULT_SET_TM( station - TM , subslot % 100 , 1 , WAFERRESULT_SUCCESS );
					break;
				case 1 :
					WAFER_RESULT_SET_TM( station - TM , subslot % 100 , 1 , WAFERRESULT_FAILURE );
					break;
				default :
					if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
						WAFER_RESULT_SET_TM( station - TM , subslot % 100 , 1 , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
					}
					else {
						WAFER_RESULT_SET_TM( station - TM , subslot % 100 , 1 , WAFERRESULT_PRESENT );
					}
					break;
				}
			}
			else {
				switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
				case 0 :
					WAFER_RESULT_SET_TM( station - TM , subslot , 0 , WAFERRESULT_SUCCESS );
					break;
				case 1 :
					WAFER_RESULT_SET_TM( station - TM , subslot , 0 , WAFERRESULT_FAILURE );
					break;
				default :
					if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
						WAFER_RESULT_SET_TM( station - TM , subslot , 0 , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
					}
					else {
						WAFER_RESULT_SET_TM( station - TM , subslot , 0 , WAFERRESULT_PRESENT );
					}
					break;
				}
			}
			//
		}
		else if ( station == FM  ) {
			switch( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] ) {
			case 0 :
				WAFER_RESULT_SET_FM( 0 , subslot , WAFERRESULT_SUCCESS );
				break;
			case 1 :
				WAFER_RESULT_SET_FM( 0 , subslot , WAFERRESULT_FAILURE );
				break;
			default :
				if ( CASSETTE_RESULT[ srccass ][ srcslot - 1 ] >= 2 ) {
					WAFER_RESULT_SET_FM( 0 , subslot , WAFERRESULT_USER + CASSETTE_RESULT[ srccass ][ srcslot - 1 ] - 2 );
				}
				else {
					WAFER_RESULT_SET_FM( 0 , subslot , WAFERRESULT_PRESENT );
				}
				break;
			}
		}
	}
	return 0;
}


void INIT_SCHEDULER_CASSETTE_RESULT_DATA( int apmode , int side ) {
	if ( apmode == 0 ) {
		_i_SCH_CASSETTE_LAST_RESET( -1 , -1 );
		_i_SCH_CASSETTE_LAST_RESET2( -1 , -1 ); // 2011.04.21
	}
}
