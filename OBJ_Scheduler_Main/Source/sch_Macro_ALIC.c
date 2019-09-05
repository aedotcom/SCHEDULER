#include "default.h"

//================================================================================
#include "EQ_Enum.h"

#include "Equip_Handling.h"
#include "system_tag.h"
#include "Sch_Multi_ALIC.h"

//================================================================================
int _SCH_MACRO_SPAWN_FM_MALIGNING( int CHECKING_SIDE , int Slot1 , int Slot2 , int SrcCas , int swch ) { // 2008.01.16
	return EQUIP_ALIGNING_SPAWN_FOR_FM_MAL( CHECKING_SIDE , Slot1 , Slot2 , SrcCas , swch , FALSE , -1 , -1 , -1 , -1 ); // 2006.02.08
}

int _SCH_MACRO_CHECK_FM_MALIGNING( int side , BOOL waitfinish ) { // 2008.01.16
	int Result;
	int l_c; // 2008.09.14
	l_c = 0; // 2008.09.14
	while ( TRUE ) {
		Result = EQUIP_ALIGNING_STATUS_FOR_FM_MAL( side );
		if ( Result != SYS_RUNNING ) break;
		if ( !waitfinish ) break;
		// Sleep(1); // 2008.09.14
		if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
		l_c++; // 2008.09.14
	}
	return Result;
}

int  _SCH_MACRO_SPAWN_TM_MALIGNING( int CHECKING_SIDE , int Finger , int Slot , int SrcCas ) {
	return EQUIP_ALIGNING_SPAWN_FOR_TM_MAL( CHECKING_SIDE , Finger , Slot , SrcCas );
}

int _SCH_MACRO_CHECK_TM_MALIGNING( int side , BOOL waitfinish ) { // 2008.01.16
	int Result;
	int l_c; // 2008.09.14
	l_c = 0; // 2008.09.14
	while ( TRUE ) {
		Result = EQUIP_ALIGNING_STATUS_FOR_TM_MAL( side );
		if ( Result != SYS_RUNNING ) break;
		if ( !waitfinish ) break;
		// Sleep(1); // 2008.09.14
		if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
		l_c++; // 2008.09.14
	}
	return Result;
}

int  _SCH_MACRO_SPAWN_FM_MCOOLING( int side , int icslot1 , int icslot2 , int Slot1 , int Slot2 , int SrcCas ) { // 2008.02.15
	return EQUIP_COOLING_SPAWN_FOR_FM_MIC( side , icslot1 , icslot2 , Slot1 , Slot2 , SrcCas , ( icslot1 > 0 ) ? _i_SCH_MODULE_Get_MFIC_SIDE(icslot1) : -1 , ( icslot1 > 0 ) ? _i_SCH_MODULE_Get_MFIC_POINTER(icslot1) : -1 , ( icslot2 > 0 ) ? _i_SCH_MODULE_Get_MFIC_SIDE(icslot2) : -1 , ( icslot2 > 0 ) ? _i_SCH_MODULE_Get_MFIC_SIDE(icslot2) : -1 );
}

int  _SCH_MACRO_CHECK_FM_MCOOLING( int side , int icslot , BOOL waitfinish ) { // 2008.02.15
	int Result;
	int l_c; // 2008.09.14
	l_c = 0; // 2008.09.14
	while ( TRUE ) {
		Result = EQUIP_COOLING_STATUS_FOR_FM_MIC( side , icslot );
		if ( Result != SYS_RUNNING ) break;
		if ( !waitfinish ) break;
		// Sleep(1); // 2008.09.14
		if ( ( l_c % 10 ) == 0 ) Sleep(1); // 2008.09.14
		l_c++; // 2008.09.14
	}
	return Result;
}

int  _SCH_MACRO_FM_ALIC_OPERATION( int fms , int PickPlaceMode , int side , int chamber , int alic_slot1 , int alic_slot2 , int SrcCas , int swch , BOOL insidelog , int sourcewaferidA , int sourcewaferidB , int side1 , int side2 , int pointer , int pointer2 ) {
//	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side ) ) return SYS_ABORTED; // 2012.07.12 // 2016.11.04
	if ( !_i_SCH_SYSTEM_PAUSE2_ABORT_CHECK( side , -1 ) ) return SYS_ABORTED; // 2012.07.12 // 2016.11.04
	return EQUIP_PLACE_AND_PICK_WITH_FALIC( fms , side , chamber , alic_slot1 , alic_slot2 , SrcCas , swch , PickPlaceMode , insidelog , sourcewaferidA , sourcewaferidB , side1 , side2 , pointer , pointer2 , FALSE );
}


