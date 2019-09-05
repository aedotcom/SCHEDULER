#include "default.h"

#include "EQ_Enum.h"

#include "Multijob.h"
#include "MR_Data.h"
#include "DLL_Interface.h"
#include "IO_Part_data.h"
#include "User_Parameter.h"
#include "system_tag.h"
#include "sch_main.h"
#include "sch_cassmap.h"
#include "sch_sub.h"
#include "sch_prm.h"
#include "sch_prm_cluster.h"
#include "sch_prepost.h"
#include "sch_processmonitor.h"
#include "Multireg.h"
#include "Multijob.h"
#include "FA_Handling.h"
#include "Gui_Handling.h"
#include "Errorcode.h"
#include "Resource.h"
#include "sch_HandOff.h"
#include "iolog.h"

#include "INF_sch_CommonUser.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

int SCHMRDATA_TAG_DATA = 0;
int SCHMRDATA_LOG_COUNT = 0;
int SCHMRDATA_LOG_TARGET = 32;

CRITICAL_SECTION CR_SCHMRDATA;

CRITICAL_SECTION CR_UNLOADLOCK[MAX_CASSETTE_SIDE]; // 2012.04.01
int              PL_UNLOADLOCK[MAX_CASSETTE_SIDE]; // 2012.04.01
int              PI_UNLOADLOCK[MAX_CASSETTE_SIDE]; // 2012.04.01

void SCHEDULER_Set_CARRIER_INDEX( int , int ); // 2011.09.14
int  SCHEDULER_Get_CARRIER_INDEX( int ); // 2011.09.14

void SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( int ch );

void SCHMRDATA_INIT() {
	int i;
	//
	SCHMRDATA_TAG_DATA = 0;
	SCHMRDATA_LOG_COUNT = 0;
	//
	if      ( MAX_MULTI_MTRINFO_SIZE <= 256 ) {
		SCHMRDATA_LOG_TARGET = 256;
	}
	else if ( MAX_MULTI_MTRINFO_SIZE <= 512 ) {
		SCHMRDATA_LOG_TARGET = 128;
	}
	else if ( MAX_MULTI_MTRINFO_SIZE <= 1024 ) {
		SCHMRDATA_LOG_TARGET = 64;
	}
	else {
		SCHMRDATA_LOG_TARGET = 32;
	}
	//
	InitializeCriticalSection( &CR_SCHMRDATA );
	//
	for ( i = 0 ; i < MAX_CASSETTE_SIDE  ; i++ ) { // 2012.04.01
		InitializeCriticalSection( &CR_UNLOADLOCK[i] );
		PL_UNLOADLOCK[i] = 0;
		PI_UNLOADLOCK[i] = 0;
	}
	//
	if ( _SCH_MULTIJOB_CHECK_EXTEND_MRDATA_USE() ) {
		for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
			Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 0;
			Sch_Multi_Material_Info_Data[i]->MtlOut_L_Mdl[0] = 0;
			Sch_Multi_Material_Info_Data[i]->MtlOut_L2_Mdl[0] = 0;
		}
	}
	//
	if ( !_SCH_COMMON_USE_EXTEND_OPTION( 3 , 0 ) ) DeleteFile( "history\\MR_Data.log" );
	//
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

BOOL SCH_MR_UNLOAD_LOCK_START( int side ) { // 2012.04.01
	int i;
	if ( side < 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE  ; i++ ) {
			EnterCriticalSection( &CR_UNLOADLOCK[i] );
		}
	}
	else {
		if ( !TryEnterCriticalSection( &CR_UNLOADLOCK[side] ) ) return FALSE;
	}
	return TRUE;
}
//
void SCH_MR_UNLOAD_LOCK_END( int side ) { // 2012.04.01
	int i;
	if ( side < 0 ) {
		for ( i = 0 ; i < MAX_CASSETTE_SIDE  ; i++ ) {
			LeaveCriticalSection( &CR_UNLOADLOCK[i] );
		}
	}
	else {
		LeaveCriticalSection( &CR_UNLOADLOCK[side] );
	}
}

void SCH_MR_UNLOAD_LOCK_SET_PLACE( int side , BOOL data ) { // 2012.04.01
	PL_UNLOADLOCK[side] = data;
}

void SCH_MR_UNLOAD_LOCK_SET_PICK( int side , BOOL data ) { // 2012.04.01
	PI_UNLOADLOCK[side] = data;
}

BOOL SCH_MR_UNLOAD_LOCK_GET_PICKPLACE( int side ) { // 2012.04.01
	if ( PI_UNLOADLOCK[side] ) return TRUE;
	if ( PL_UNLOADLOCK[side] ) return TRUE;
	return FALSE;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

int SCH_MR_Get_Tag() {
	SCHMRDATA_TAG_DATA++;
	return SCHMRDATA_TAG_DATA;
}

void SCH_MR_Data_Logging( int mode , char *message , int data , int data2 , int data3 , int data4 , int data5 ) {
	int i;
	FILE *fpt;
	SYSTEMTIME		SysTime;
	//
//	printf( "=SCH_MR_Data_Logging = [%d][%s] [%d][%d]\n" , mode , message , data , data2 );
	//
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return; // 2012.04.04 // 2012.08.14

	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() <= 0 ) return; // 2011.11.24
	//
	if ( !_SCH_COMMON_USE_EXTEND_OPTION( 3 , 0 ) ) return;
	//
//	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	SCHMRDATA_LOG_COUNT++;
	//
	if ( SCHMRDATA_LOG_COUNT > SCHMRDATA_LOG_TARGET ) {
		//
		SCHMRDATA_LOG_COUNT = 0;
		//
		DeleteFile( "history\\MR_Data_O3.log" );
		//
		rename( "history\\MR_Data_O2.log" , "history\\MR_Data_O3.log" );
		rename( "history\\MR_Data_O1.log" , "history\\MR_Data_O2.log" );
		rename( "history\\MR_Data.log"    , "history\\MR_Data_O1.log" );
		//
	}
	//
	fpt = fopen( "history\\MR_Data.log" , "a" );
	//
	if ( fpt != NULL ) {
		//
		GetLocalTime( &SysTime );
		//
//		fprintf(fpt, "%04d%/%02d/%02d %02d:%02d:%02d.%03d\tACT=%d,Msg=%s\t%d,%d,%d,%d,%d\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds, mode, message, data, data2, data3, data4, data5); // 2019.03.07
		fprintf( fpt , "%04d/%02d/%02d %02d:%02d:%02d.%03d\tACT=%d,Msg=%s\t%d,%d,%d,%d,%d\n" , SysTime.wYear , SysTime.wMonth , SysTime.wDay , SysTime.wHour , SysTime.wMinute , SysTime.wSecond , SysTime.wMilliseconds , mode , message , data , data2 , data3 , data4 , data5 ); // 2019.03.07
		//
		for ( i = 0 ; i < MAX_CASSETTE_SIDE ; i++ ) {
			//
			fprintf( fpt , "\tC%d\tINDEX=%04d\tVERIFY=%d\tMID=%c%s%c\tMIDSTART=%c%s%c\n" , i+1 ,
				SCHEDULER_Get_CARRIER_INDEX( i ) ,
				SCHMULTI_CASSETTE_VERIFY_GET( i ) ,
				'"' , SCHMULTI_CASSETTE_MID_GET( i ) , '"' ,
				'"' , SCHMULTI_CASSETTE_MID_GET_FOR_START( i ) , '"' ); // 2013.06.05
		}
		for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
			//
			if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode == 0 ) continue;
			//
			/*
			// 2012.08.24
			fprintf( fpt , "\t%d\tMode=%d,Tag=%d\tIN:%d:%d,OUT:%d:%d\tL:%s:%d,L2:%s:%d\tS=%d,P=%d\tETC=%d\n"
				, i
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Mode
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Tag
				, Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM
				, Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L_Mdl
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L_SlotArm
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L2_Mdl
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L2_SlotArm
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Side
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Etc
				);
			*/
			// 2012.08.24
			fprintf( fpt , "\t%d\t" , i );
			//
			switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
			case 1 :	fprintf( fpt , "Mode=1(Loading)" );	break;
			case 2 :	fprintf( fpt , "Mode=2(Loaded)" );	break;
			case 3 :	fprintf( fpt , "Mode=3(LoadFail)" );	break;
			case 4 :	fprintf( fpt , "Mode=4(Mapping)" );	break;
			case 5 :	fprintf( fpt , "Mode=5(Mapped)" );	break;
			case 6 :	fprintf( fpt , "Mode=6(MappingFail)" );	break;
			case 7 :	fprintf( fpt , "Mode=7(Starting)" );	break;
			case 8 :	fprintf( fpt , "Mode=8(Running)" );	break;
			case 9 :	fprintf( fpt , "Mode=9(End)" );	break;
			case 10 :	fprintf( fpt , "Mode=10(Unloading)" );	break;
			case 11 :	fprintf( fpt , "Mode=11(Unloaded)" );	break;
			case 12 :	fprintf( fpt , "Mode=12(UnloadFail)" );	break;
			default :	fprintf( fpt , "Mode=%d" , Sch_Multi_Material_Info_Data[i]->MtlOut_Mode );	break;
			}
			fprintf( fpt , "\tCI=[%d]\t" , Sch_Multi_Material_Info_Data[i]->MtlOut_Tag );
			//
			if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM >= CM1 ) {
				fprintf( fpt , "CM%d:%d->" , Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM - CM1 + 1 , Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot );
			}
			else {
				fprintf( fpt , "(%d):%d->" , Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM , Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot );
			}
			if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM >= CM1 ) {
				fprintf( fpt , "CM%d:%d\t" , Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM - CM1 + 1 , Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot );
			}
			else {
				fprintf( fpt , "(%d):%d\t" , Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM , Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot );
			}
			//
			fprintf( fpt , "L:%s:%d,L2:%s:%d\tS=%d,P=%d\tETC=%d\n"
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L_Mdl
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L_SlotArm
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L2_Mdl
				, Sch_Multi_Material_Info_Data[i]->MtlOut_L2_SlotArm
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Side
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer
				, Sch_Multi_Material_Info_Data[i]->MtlOut_Etc
				);
		}
		fclose( fpt );
		//
	}
	//
//	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24

}

void iSCH_MR_Data_Set_Init_Data( int index , int t , int cm , int slot , int mode ) {
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_Tag = t;
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_In_CM = cm;
	Sch_Multi_Material_Info_Data[index]->MtlOut_In_Slot = slot;
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_Out_CM = cm;
	Sch_Multi_Material_Info_Data[index]->MtlOut_Out_Slot = slot;
	//
//	Sch_Multi_Material_Info_Data[index]->MtlOut_L_Mdl[0] = 0; // 2012.09.21
//	Sch_Multi_Material_Info_Data[index]->MtlOut_L_SlotArm = 0; // 2012.09.21
	//
	strcpy( Sch_Multi_Material_Info_Data[index]->MtlOut_L_Mdl , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM(cm) ); // 2012.09.21
	Sch_Multi_Material_Info_Data[index]->MtlOut_L_SlotArm = slot; // 2012.09.21
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_L2_Mdl[0] = 0;
	Sch_Multi_Material_Info_Data[index]->MtlOut_L2_SlotArm = 0;
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_Side = -1;
	Sch_Multi_Material_Info_Data[index]->MtlOut_Pointer = -1;
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_Etc = -1;
	//
	Sch_Multi_Material_Info_Data[index]->MtlOut_Mode = mode;
}

/*
// 2012.10.05
void iSCH_MR_Data_Setting_for_Place_Other_Out_Data( int cm , int slot , int side , int pt , int t , int x ) {
	int i;
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( i == x ) continue;
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM  != cm ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot  != slot ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot != slot ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 2 : // Loaded
		case 3 : // LoadFail
			//
		case 5 : // Mapped
		case 6 : // Mapping Fail
			//
		case 7 : // Starting
		case 8 : // Running
			//
			Sch_Multi_Material_Info_Data[i]->MtlOut_Side = side;
			Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer = pt;
			Sch_Multi_Material_Info_Data[i]->MtlOut_Etc = t;
			//
			break;
		}
	}
}
*/

// 2012.10.05
int iSCH_MR_Data_Setting_for_Place_Other_Out_Data( int cm , int slot , int t , int x ) {
	int i , r = -1;
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( i == x ) continue;
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM  != cm ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot  != slot ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot != slot ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 2 : // Loaded
		case 3 : // LoadFail
			//
		case 5 : // Mapped
		case 6 : // Mapping Fail
			//
			Sch_Multi_Material_Info_Data[i]->MtlOut_Etc = t;
			//
			r = Sch_Multi_Material_Info_Data[i]->MtlOut_Tag;
			//
			break;
		}
	}
	//
	return r;
}


void iSCH_MR_Data_Remove_for_Before_New_Loading( int cm ) {
	int i;
	//
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 1 : // Loading
		case 2 : // Loaded
		case 3 : // LoadFail
			//
		case 4 : // Mapping
		case 5 : // Mapped
		case 6 : // Mapping Fail
			//
			Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 0;
			//
			break;
		}
	}
}

void iSCH_MR_Data_Remove_for_Before_New_Unloading( int cm ) {
	int i;
	//
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 10 : // Unloading
		case 11 : // Unloaded
		case 12 : // UnloadFail
			Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 0;
			break;
		}
	}
	//
	/*
	// 2012.10.05
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM == cm ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 9 : // Ending
			Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 0;
			break;
		}
	}
	*/
	//
}



BOOL SCH_MR_FINDING_SAME_CASSETTE_INDEX_AFTER_LOADMAP( int cm , BOOL mapped , int *Index ) { // 2013.06.05
	int s , p , ps;
	char mid[256];
	//
	*Index = -1;
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() < 1 ) return FALSE;
	//
	switch( _i_SCH_PRM_GET_FA_MID_READ_POINT( cm - CM1 ) ) {
	case 1 : // With IO
		if ( mapped ) return FALSE;
		IO_GET_MID_NAME_FROM_READ( cm - CM1 , mid );
		break;
	case 2 : // With AfterMap
		if ( !mapped ) return FALSE;
		break;
	case 3 : // With Current IO
		if ( mapped ) return FALSE;
		IO_GET_MID_NAME( cm - CM1 , mid );
		break;
	default : // With Message
		if ( mapped ) return FALSE;
		IO_GET_MID_NAME( cm - CM1 , mid );
		break;
	}
	//
	if ( mid[0] == 0 ) return FALSE; // 2019.02.27
	//
	for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
		//
		if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
		//
		for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
			//
			ps = _i_SCH_CLUSTER_Get_PathStatus( s , p );
			//
			if ( ps == SCHEDULER_CM_END ) continue;
			//
			if ( ps == SCHEDULER_READY ) {
				//
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( s , p ) % 100 ) != _MOTAG_12_NEED_IN_OUT /* IN+OUT */ ) { // 2013.07.02
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( s , p ) % 100 ) != _MOTAG_13_NEED_IN ) /* IN */continue; // 2013.07.02
				}
				//
//				if ( mid[0] == 0 ) { // 2019.02.27
//					if ( _i_SCH_CLUSTER_Get_PathIn( s , p ) != cm ) continue; // 2019.02.27
//				} // 2019.02.27
				//
				if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( s ) < 2 ) { // 2013.08.16
					if ( !STRCMP_L( STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_MaterialID( s , p ) ) , mid ) ) continue;
				}
				//
				*Index = _i_SCH_CLUSTER_Get_Ex_CarrierIndex( s , p );
				//
				return TRUE;
			}
			//
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( s , p ) % 100 ) != _MOTAG_12_NEED_IN_OUT /* IN+OUT */) { // 2013.07.02
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( s , p ) % 100 ) != _MOTAG_14_NEED_OUT /* OUT */) continue; // 2013.07.02
			}
			//
//			if ( mid[0] == 0 ) { // 2019.02.27
//				if ( _i_SCH_CLUSTER_Get_PathOut( s , p ) != cm ) continue; // 2019.02.27
//			} // 2019.02.27
			//
			if ( _i_SCH_PRM_GET_FA_MAPPING_AFTERLOAD( s ) < 2 ) { // 2013.08.16
				if ( !STRCMP_L( STR_MEM_GET_STR( _i_SCH_CLUSTER_Get_Ex_OutMaterialID( s , p ) ) , mid ) ) continue;
			}
			//
			*Index = _i_SCH_CLUSTER_Get_Ex_OutCarrierIndex( s , p );
			//
			return TRUE;
		}
	}
	//
	return FALSE;
	//
}

BOOL SCH_MR_Data_Setting_for_Loading( int cm , int mode ) {
	int i , c , t , m , ot; // , s , p , pi , si;
	//
/*
// 2012.08.14
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.04
		if      ( mode == 1 ) { // loading
			t = SCH_MR_Get_Tag();
			SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , t );
		}
		else if ( mode == 4 ) { // mapping
			t = SCHEDULER_Get_CARRIER_INDEX( cm - CM1 );
			//
			if ( t < 0 ) {
				t = SCH_MR_Get_Tag();
				SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , t );
			}
			//
		}
	}
	else {
*/

		//====================================================================================================================================
		// 2013.06.05
		//
		t = -1;
		//
		if      ( mode == 2 ) { // Loaded
			if ( !SCH_MR_FINDING_SAME_CASSETTE_INDEX_AFTER_LOADMAP( cm , FALSE , &t ) ) t = -1;
		}
		else if ( mode == 5 ) { // mapped
			if ( !SCH_MR_FINDING_SAME_CASSETTE_INDEX_AFTER_LOADMAP( cm , TRUE , &t ) ) t = -1;
		}
		//
		if ( t != -1 ) {
			//
			ot = SCHEDULER_Get_CARRIER_INDEX( cm - CM1 );
			//
			if ( ot != t ) {
				//
				SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , t );
				//
				for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode < 10 ) continue;
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Tag != t ) continue;
					//
					Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 7;
					//
				}
				//
				for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
					//
					if ( ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode < 1 ) || ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode > 5 ) ) continue;
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Tag != ot ) continue;
					//
					Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 0;
					//
				}
				//







					/*

					if ( ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode >= 1 ) && ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode <= 5 ) ) {
						//
						Sch_Multi_Material_Info_Data[i]->MtlOut_Tag = t;
						//
						for ( s = 0 ; s < MAX_CASSETTE_SIDE ; s++ ) {
							//
							if ( !_i_SCH_SYSTEM_USING_RUNNING( s ) ) continue;
							//
							for ( p = 0 ; p < MAX_CASSETTE_SLOT_SIZE ; p++ ) {
								//
								if ( _i_SCH_CLUSTER_Get_PathRange( s , p ) < 0 ) continue;
								//
								if ( _i_SCH_CLUSTER_Get_Ex_MtlOut( s , p ) != 12 ) continue;
								//
								pi = _i_SCH_CLUSTER_Get_PathIn( s , p );
								si = _i_SCH_CLUSTER_Get_SlotIn( s , p );
								//
								if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != pi ) continue;
								if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot != si ) continue;
								//
								if ( _i_SCH_CLUSTER_Get_PathOut( s , p ) >= CM1 ) {
									Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM   = _i_SCH_CLUSTER_Get_PathOut( s , p );
								}
								else {
									Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM   = 0;
								}
								//
								Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 7;
								//
								Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot = _i_SCH_CLUSTER_Get_SlotOut( s , p );
								//
								Sch_Multi_Material_Info_Data[i]->MtlOut_Side = s;
								Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer = p;
								//
								s = MAX_CASSETTE_SIDE;
								//
								break;
							}
						}
					}
				}
					*/
				//
				switch( mode ) {
				case 2 : // Loaded
					SCH_MR_Data_Logging( 1 , "Remapping(Load)" , cm , mode , 0 , t , 0 );
					break;
				case 5 : // Mapped
					SCH_MR_Data_Logging( 1 , "Remapping(Map)" , cm , mode , 0 , t , 0 );
					break;
				}
				//
				return TRUE;
				//
			}
		}
		//====================================================================================================================================
		t = -1;
		//
		if ( mode == 4 ) { // mapping
			//
			m = 1;
			//
			for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
				//
				if ( ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode >= 1 ) && ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode <= 3 ) ) {
					//
					m = 4;
					//
					t = Sch_Multi_Material_Info_Data[i]->MtlOut_Tag;
					//
					break;
				}
				if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode == 7 ) { // 2013.06.24
					//
					m = 4;
					//
					t = Sch_Multi_Material_Info_Data[i]->MtlOut_Tag;
					//
					break;
				}
				//
			}
			//
		}
		else {
			m = mode;
		}
		//
		if ( m == 1 ) {
			//
			iSCH_MR_Data_Remove_for_Before_New_Loading( cm );
			//
			c = 1;
			t = SCH_MR_Get_Tag();
			//
			for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode != 0 ) continue;
				//
				iSCH_MR_Data_Set_Init_Data( i , t , cm , c , mode );
				//
				c++;
				//
				if ( c >= MAX_CASSETTE_SLOT_SIZE ) break;
			}
		}
		else {
			//
			c = 0;
			//
			for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
				//
				switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
				case 1 : // Loading
				case 2 : // Loaded
				case 3 : // LoadFail
					//
				case 4 : // Mapping
				case 5 : // Mapped
				case 6 : // Mapping Fail
					//
					Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = mode;
					break;
				}
				//
			}
			//
		}
		//
		if ( ( m == 1 ) || ( m == 4 ) ) {
			SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , t ); // 2011.09.14
		}
		//
/*
// 2012.08.14
	}
*/
	//
//	SCH_MR_Data_Logging( 1 , "Loading" , cm , mode , c , t , m ); // 2013.06.05
	//
	switch( mode ) { // 2013.06.05
	case 1 : // Loading
		SCH_MR_Data_Logging( 1 , "Loading" , cm , mode , c , t , m );
		break;
	case 2 : // Loaded
		SCH_MR_Data_Logging( 1 , "Loaded" , cm , mode , c , t , m );
		break;
	case 3 : // LoadFail
		SCH_MR_Data_Logging( 1 , "LoadFail" , cm , mode , c , t , m );
		break;
	case 4 : // Mapping
		SCH_MR_Data_Logging( 1 , "Mapping" , cm , mode , c , t , m );
		break;
	case 5 : // Mapped
		SCH_MR_Data_Logging( 1 , "Mapped" , cm , mode , c , t , m );
		break;
	case 6 : // Mapping Fail
		SCH_MR_Data_Logging( 1 , "MappingFail" , cm , mode , c , t , m );
		break;
	default :
		SCH_MR_Data_Logging( 1 , "Loading(?)" , cm , mode , c , t , m );
		break;
	}
	//
	return FALSE;
	//
}


/*
// 2012.10.05
void iSCH_MR_Data_Setting_for_Unloading_Move( int index ) {
	int i;
	//
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( i == index ) continue;
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode != 0 ) continue;
		//
		Sch_Multi_Material_Info_Data[i]->MtlOut_Tag = Sch_Multi_Material_Info_Data[index]->MtlOut_Tag;
		//
		Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM    = Sch_Multi_Material_Info_Data[index]->MtlOut_In_CM;
		Sch_Multi_Material_Info_Data[i]->MtlOut_In_Slot  = Sch_Multi_Material_Info_Data[index]->MtlOut_In_Slot;
		//
		Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM   = Sch_Multi_Material_Info_Data[index]->MtlOut_In_CM;
		Sch_Multi_Material_Info_Data[i]->MtlOut_Out_Slot = Sch_Multi_Material_Info_Data[index]->MtlOut_In_Slot;
		//
		strcpy( Sch_Multi_Material_Info_Data[i]->MtlOut_L_Mdl , Sch_Multi_Material_Info_Data[index]->MtlOut_L_Mdl );
		Sch_Multi_Material_Info_Data[i]->MtlOut_L_SlotArm = Sch_Multi_Material_Info_Data[index]->MtlOut_L_SlotArm;
		//
		strcpy( Sch_Multi_Material_Info_Data[i]->MtlOut_L2_Mdl , Sch_Multi_Material_Info_Data[index]->MtlOut_L2_Mdl );
		Sch_Multi_Material_Info_Data[i]->MtlOut_L2_SlotArm = Sch_Multi_Material_Info_Data[index]->MtlOut_L2_SlotArm;
		//
		Sch_Multi_Material_Info_Data[i]->MtlOut_Side = -1;
		Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer = -1;
		//
		Sch_Multi_Material_Info_Data[i]->MtlOut_Etc = Sch_Multi_Material_Info_Data[index]->MtlOut_Etc;
		//
		Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 10;
		//
		break;
		//
	}
}
*/

void SCHMRDATA_Data_Setting_for_Finishing( int cm ) {
	int i;
	//
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return; // 2012.04.03 // 2012.08.14
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 7 : // Starting
		case 8 : // Running
		case 9 : // End
			//
			SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , Sch_Multi_Material_Info_Data[i]->MtlOut_Tag ); // 2011.09.14
			//
			LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
			//
			return;
			//
			break;
			//
		}
		//
	}
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
}


BOOL SCH_MR_Data_Setting_for_Unloading( int cm , int mode ) {
//	int i , c = -1 , t = -1 , f , s; // 2012.10.05
	int i , c = -1 , t = -1 , f;
	//
/*
// 2012.08.14
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.03
		if ( mode == 11 ) { // Unloaded
			SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , -1 );
		}
	}
	else {
*/
		if ( mode == 10 ) { // Unloading
			//
			iSCH_MR_Data_Remove_for_Before_New_Unloading( cm );
			//
			/*
			// 2012.10.04
			f = 0;
			//
			if ( f == 0 ) { // IN$/OUT
				//
				for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM == cm ) continue;
					//
					switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
					case 7 : // Starting
					case 8 : // Running
						//
						s = 0;
						//
						if ( _i_SCH_CLUSTER_Get_PathStatus( Sch_Multi_Material_Info_Data[i]->MtlOut_Side , Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer ) == SCHEDULER_CM_END ) {
							if ( _i_SCH_CLUSTER_Get_PathIn( Sch_Multi_Material_Info_Data[i]->MtlOut_Side , Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer ) == cm ) {
								if ( _i_SCH_CLUSTER_Get_PathOut( Sch_Multi_Material_Info_Data[i]->MtlOut_Side , Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer ) != cm ) {
									//
									s = 1;
									//
									Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = mode;
								}
							}
						}
						//
						if ( s == 0 ) {
							//
							iSCH_MR_Data_Setting_for_Unloading_Move( i );
							//
							Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = 9;
							//
						}
						//
						f = 1;
						//
						t = Sch_Multi_Material_Info_Data[i]->MtlOut_Tag;
						//
						break;
					}
				}
				//
			}
			//
			if ( f == 0 ) { // IN=OUT* // IN/OUT*
				//
				for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
					//
					switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
					case 7 : // Starting
					case 8 : // Running
					case 9 : // End
						//
						if ( _i_SCH_CLUSTER_Get_PathStatus( Sch_Multi_Material_Info_Data[i]->MtlOut_Side , Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer ) == SCHEDULER_CM_END ) {
							if ( _i_SCH_CLUSTER_Get_PathIn( Sch_Multi_Material_Info_Data[i]->MtlOut_Side , Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer ) == cm ) {
								if ( _i_SCH_CLUSTER_Get_PathOut( Sch_Multi_Material_Info_Data[i]->MtlOut_Side , Sch_Multi_Material_Info_Data[i]->MtlOut_Pointer ) == cm ) {
									Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = mode;
								}
							}
						}
						//
						f = 2;
						//
						t = Sch_Multi_Material_Info_Data[i]->MtlOut_Tag;
						//
						break;
					}
				}
				//
			}
			//
			if ( f == 0 ) { // IN=OUT*
				//
				for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
					//
					switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
					case 1 : // Loading
					case 2 : // Loaded
					case 3 : // Loaded Fail
						//
					case 4 : // Mapping
					case 5 : // Mapped
					case 6 : // Mapping Fail
						//
						Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = mode;
						//
						f = 3;
						//
						t = Sch_Multi_Material_Info_Data[i]->MtlOut_Tag;
						//
						break;
					}
				}
				//
			}
			//
			if ( f == 0 ) {
				//
				c = 1;
				t = SCH_MR_Get_Tag();
				//
				for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
					//
					if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode != 0 ) continue;
					//
					iSCH_MR_Data_Set_Init_Data( i , t , cm , c , mode );
					//
					c++;
					//
					if ( c >= MAX_CASSETTE_SLOT_SIZE ) break;
				}
			}
			//
			SCHEDULER_Set_CARRIER_INDEX( cm - CM1 , t ); // 2011.09.14
			//
			*/
			// 2012.10.05
			//
			f = 0;
			//
			for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
				//
				switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
				case 1 : // Loading
				case 2 : // Loaded
				case 3 : // Loaded Fail
					//
				case 4 : // Mapping
				case 5 : // Mapped
				case 6 : // Mapping Fail
					//
				case 7 : // Starting
				case 8 : // Running
				case 9 : // End
					//
					Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = mode;
					//
					f = 1;
					//
					break;
				}
			}
			//
		}
		else {
			//
			for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Out_CM != cm ) continue;
				//
				switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
				case 10 : // Unloading
					//
					Sch_Multi_Material_Info_Data[i]->MtlOut_Mode = mode;
					//
					break;
				}
				//
			}
		}
/*
// 2012.08.14
	}
*/
	//
//	SCH_MR_Data_Logging( 2 , "Unloading" , cm , mode , c , t , f ); // 2013.06.05
	//
	switch( mode ) { // 2013.06.05
	case 10 : // UnLoading
		SCH_MR_Data_Logging( 2 , "Unloading" , cm , mode , c , t , f );
		break;
	case 11 : // UnLoaded
		SCH_MR_Data_Logging( 2 , "Unloaded" , cm , mode , c , t , f );
		break;
	case 12 : // UnLoadFail
		SCH_MR_Data_Logging( 2 , "UnloadFail" , cm , mode , c , t , f );
		break;
	default :
		SCH_MR_Data_Logging( 2 , "Unloading(?)" , cm , mode , c , t , f );
		break;
	}
	//
	return FALSE;
	//
}

void SCHMRDATA_Data_Setting_for_PreStarting( int cm , int side ) {
	int i , c = -1 , t = -1;
	//
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return; // 2012.04.03 // 2012.08.14
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_In_CM != cm ) continue;
		//
		switch( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode ) {
		case 1 : // Loading
		case 2 : // Loaded
		case 3 : // Loaded Fail
			//
		case 4 : // Mapping
		case 5 : // Mapped
		case 6 : // Mapping Fail
			//
			SCHEDULER_Set_CARRIER_INDEX( side , Sch_Multi_Material_Info_Data[i]->MtlOut_Tag ); // 2011.09.14
			//
			LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
			//
			return;
		}
		//
	}
	//
	c = 1;
	t = SCH_MR_Get_Tag();
	//
	for ( i = 0 ; i < MAX_MULTI_MTRINFO_SIZE  ; i++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[i]->MtlOut_Mode != 0 ) continue;
		//
		iSCH_MR_Data_Set_Init_Data( i , t , cm , c , 5 );
		//
		c++;
		//
		if ( c >= MAX_CASSETTE_SLOT_SIZE ) break;
	}
	//
	SCHMULTI_CASSETTE_VERIFY_MID_READ_SET_LOW( cm - CM1 ); // 2013.06.24
	//
	SCHEDULER_Set_CARRIER_INDEX( side , t ); // 2011.09.14
	//
	SCH_MR_Data_Logging( 11 , "PreStarting" , cm , side , c , t , 0 );
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
}

void SCHMRDATA_Data_Setting_for_Starting( int side ) {
	int i , j , pi , si , tag = -1 , f , c_ok = 0 , c_f = 0 , to;
//	int t , po , cm; // 2012.10.11
//	int cm[MAX_CHAMBER]; // 2012.10.11
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
/*
// 2012.10.11
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) { // 2012.04.03
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) cm[i] = FALSE;
		//
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
			//
			pi = _i_SCH_CLUSTER_Get_PathIn( side , i );
			si = _i_SCH_CLUSTER_Get_SlotIn( side , i );
			po = _i_SCH_CLUSTER_Get_PathOut( side , i );
			//
			if ( ( pi >= CM1 ) && ( pi < PM1 ) ) {
				t = SCHEDULER_Get_CARRIER_INDEX( pi - CM1 );
				if ( t < 0 ) {
					t = SCH_MR_Get_Tag();
					SCHEDULER_Set_CARRIER_INDEX( pi - CM1 , t );
				}
				//
				_i_SCH_CLUSTER_Set_Ex_CarrierIndex( side , i , t );
				_i_SCH_CLUSTER_Set_Ex_MaterialID( side , i , SCHMULTI_CASSETTE_MID_GET_FOR_START( pi - CM1 ) );
				//
				if ( pi == po ) {
					_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( side , i , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , i ) );
					_i_SCH_CLUSTER_Set_Ex_OutMaterialID( side , i , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , i ) );
				}
				else {
					_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( side , i , -1 );
					_i_SCH_CLUSTER_Set_Ex_OutMaterialID( side , i , "" );
				}
			}
			else {
				_i_SCH_CLUSTER_Set_Ex_CarrierIndex( side , i , -1 );
				_i_SCH_CLUSTER_Set_Ex_MaterialID( side , i , "" );
				_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( side , i , -1 );
				_i_SCH_CLUSTER_Set_Ex_OutMaterialID( side , i , "" );
				//
				continue;
				//
			}
			//
			for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE  ; j++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_CM != pi ) continue;
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_Slot != si ) continue;
				//
				f = 0;
				//
				switch( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode ) {
				case 1 : // Loading
				case 2 : // Loaded
				case 3 : // Loaded Fail
					//
				case 4 : // Mapping
				case 5 : // Mapped
				case 6 : // Mapping Fail
					//
					if ( po >= CM1 ) {
						Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM   = _i_SCH_CLUSTER_Get_PathOut( side , i );
					}
					else {
						Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM   = 0;
					}
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Out_Slot = _i_SCH_CLUSTER_Get_SlotOut( side , i );
					//
					//strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl , "" ); // 2012.09.28
					//Sch_Multi_Material_Info_Data[j]->MtlOut_L_SlotArm = si; // 2012.09.28
					//
					//strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L2_Mdl , "" ); // 2012.09.28
					//Sch_Multi_Material_Info_Data[j]->MtlOut_L2_SlotArm = si; // 2012.09.28
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Side = side;
					Sch_Multi_Material_Info_Data[j]->MtlOut_Pointer = i;
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Mode = 7;
					//
					cm[pi] = TRUE;
					//
					f = 1;
					//
					break;
				}
				//
				if ( f == 1 ) break;
				//
			}
			//
		}
		//
		for ( i = 0 ; i < MAX_CHAMBER ; i++ ) {
			//
			if ( !cm[i] ) continue;
			//
			for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE  ; j++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_CM != i ) continue;
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode == 7 ) continue;
				//
//				Sch_Multi_Material_Info_Data[j]->MtlOut_Mode = 0; // 2012.09.28
				Sch_Multi_Material_Info_Data[j]->MtlOut_Mode = 7; // 2012.09.28
				//
			}
			//
		}
	}
	else {
*/
		for ( i = 0 ; i < MAX_CASSETTE_SLOT_SIZE ; i++ ) {
			//
			if ( _i_SCH_CLUSTER_Get_PathRange( side , i ) < 0 ) continue;
			//
			pi = _i_SCH_CLUSTER_Get_PathIn( side , i );
			si = _i_SCH_CLUSTER_Get_SlotIn( side , i );
			//
			for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE  ; j++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_CM != pi ) continue;
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_Slot != si ) continue;
				//
				f = 0;
				//
				switch( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode ) {
				case 1 : // Loading
				case 2 : // Loaded
				case 3 : // Loaded Fail
					//
				case 4 : // Mapping
				case 5 : // Mapped
				case 6 : // Mapping Fail
					//
					if ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) {
						Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM   = _i_SCH_CLUSTER_Get_PathOut( side , i );
					}
					else {
						Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM   = 0;
					}
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Out_Slot = _i_SCH_CLUSTER_Get_SlotOut( side , i );
					//
//					strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl , "" ); // 2012.09.28
//					Sch_Multi_Material_Info_Data[j]->MtlOut_L_SlotArm = si; // 2012.09.28
					//
//					strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L2_Mdl , "" ); // 2012.09.28
//					Sch_Multi_Material_Info_Data[j]->MtlOut_L2_SlotArm = si; // 2012.09.28
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Side = side;
					Sch_Multi_Material_Info_Data[j]->MtlOut_Pointer = i;
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Mode = 7;
					//
					_i_SCH_CLUSTER_Set_Ex_CarrierIndex( side , i , Sch_Multi_Material_Info_Data[j]->MtlOut_Tag ); // 2011.09.14
					//
//					if ( ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathOut( side , i ) < PM1 ) ) { // 2012.04.01 // 2013.01.15
					if ( ( _i_SCH_CLUSTER_Get_PathIn( side , i ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathIn( side , i ) < PM1 ) ) { // 2012.04.01 // 2013.01.15
						_i_SCH_CLUSTER_Set_Ex_MaterialID( side , i , SCHMULTI_CASSETTE_MID_GET_FOR_START( _i_SCH_CLUSTER_Get_PathIn( side , i ) - CM1 ) );
					}
					else {
						_i_SCH_CLUSTER_Set_Ex_MaterialID( side , i , "" );
					}
					//
					if ( _i_SCH_CLUSTER_Get_PathIn( side , i ) == _i_SCH_CLUSTER_Get_PathOut( side , i ) ) { // 2012.04.01
						_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( side , i , _i_SCH_CLUSTER_Get_Ex_CarrierIndex( side , i ) );
						_i_SCH_CLUSTER_Set_Ex_OutMaterialID( side , i , _i_SCH_CLUSTER_Get_Ex_MaterialID( side , i ) );
					}
					else {
						if ( ( _i_SCH_CLUSTER_Get_PathOut( side , i ) >= CM1 ) && ( _i_SCH_CLUSTER_Get_PathOut( side , i ) < PM1 ) ) { // 2013.07.02
							to = SCHEDULER_Get_CARRIER_INDEX( _i_SCH_CLUSTER_Get_PathOut( side , i ) - CM1 );
							if ( to < 0 ) {
								to = SCH_MR_Get_Tag();
								SCHEDULER_Set_CARRIER_INDEX( _i_SCH_CLUSTER_Get_PathOut( side , i ) - CM1 , to );
							}
							_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( side , i , to );
							_i_SCH_CLUSTER_Set_Ex_OutMaterialID( side , i , SCHMULTI_CASSETTE_MID_GET_FOR_START( _i_SCH_CLUSTER_Get_PathOut( side , i ) - CM1 ) );
						}
						else {
							_i_SCH_CLUSTER_Set_Ex_OutCarrierIndex( side , i , -1 );
							_i_SCH_CLUSTER_Set_Ex_OutMaterialID( side , i , "" );
						}
					}
					//
					if ( tag == -1 ) tag = Sch_Multi_Material_Info_Data[j]->MtlOut_Tag;
					//
					f = 1;
					//
					SCHMULTI_RUNJOB_DATA_SET( side , i ); // 2018.06.27
					//
					break;
				}
				//
				if ( f == 1 ) {
					c_ok++;
					break;
				}
				else {
					c_f++;
				}
				//
			}
			//
		}
		//
		if ( tag != -1 ) {
			for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE  ; j++ ) {
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Tag != tag ) continue;
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode == 0 ) continue; // 2019.02.27
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode == 7 ) continue;
				//
//				Sch_Multi_Material_Info_Data[j]->MtlOut_Mode = 0; // 2012.09.28
				Sch_Multi_Material_Info_Data[j]->MtlOut_Mode = 7; // 2012.09.28
				//
			}
		}
//	} // 2012.10.11
	//
	SCH_MR_Data_Logging( 12 , "Starting" , side , tag , c_ok , c_f , 0 );
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
}

BOOL SCHMRDATA_Data_Setting_for_outcm( int cm , int side , int pt , int log ) {
	int j , f = -1 , r = -1 , act;
	//
	if ( !_SCH_MULTIJOB_CHECK_EXTEND_MRDATA_USE() ) return FALSE;
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return; // 2012.04.03 // 2012.10.11
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	act = FALSE;
	//
	if ( log != -1 ) { // 2013.06.05
		//
		for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE ; j++ ) {
			//
			if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Side != side ) continue;
			if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Pointer != pt ) continue;
			//
			if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode <= 0 ) continue; // 2013.06.12
			if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode >= 10 ) continue; // 2013.06.12
			//
			act = TRUE;
			//
			Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM = ( cm < CM1 ) ? 0 : cm;
			//
			// 2012.10.05
			if ( ( cm >= CM1 ) && ( cm < PM1 ) ) {
				//
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_CM != Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM ) {
					//
					r = iSCH_MR_Data_Setting_for_Place_Other_Out_Data( cm , Sch_Multi_Material_Info_Data[j]->MtlOut_Out_Slot , Sch_Multi_Material_Info_Data[j]->MtlOut_Tag , j );
					//
					Sch_Multi_Material_Info_Data[j]->MtlOut_Etc = r;
					//
				}
				//
			}
			//
			f = j;
			//
			break;
		}
		//
		if ( log == 0 ) {
			LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
			return act; 
		}
		if ( ( log == 1 ) && !act ) {
			LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
			return act;
		}
		//
	}
	//
	SCH_MR_Data_Logging( 21 , "SetForOutCM" , cm , side , pt , f , r );
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	return act;
	//
}

void SCHMRDATA_ReSet_For_Move( int tside , int tpt , int side , int pt ) {
	int j , f = -1;
	//
	if ( !_SCH_MULTIJOB_CHECK_EXTEND_MRDATA_USE() ) return;
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return; // 2012.04.03 // 2012.10.11
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE  ; j++ ) {
		//
		if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Side != side ) continue;
		if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Pointer != pt ) continue;
		//
		if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode <= 0 ) continue; // 2013.06.12
		if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode >= 10 ) continue; // 2013.06.12
		//
		Sch_Multi_Material_Info_Data[j]->MtlOut_Side = tside;
		Sch_Multi_Material_Info_Data[j]->MtlOut_Pointer = tpt;
		//
		f = j;
		//
		break;
	}
	//
	SCH_MR_Data_Logging( 22 , "Move" , tside , tpt , side , pt , f );
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
}

void SCHMRDATA_Data_Setting_for_PickPlacing( int mode , int side , int pt , int rbm , int arm , int mdl , int slot ) {
	int i , j , f = -1;
	//
	if ( !_SCH_MULTIJOB_CHECK_EXTEND_MRDATA_USE() ) return;
//	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() > 1 ) return; // 2012.04.03 // 2012.08.14
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	for ( i = 0 ; i < 2 ; i++ ) {
		//
		for ( j = 0 ; j < MAX_MULTI_MTRINFO_SIZE ; j++ ) {
			//
			if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Side != side ) continue;
			if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Pointer != pt ) continue;
			//
			if ( i == 0 ) { // 2013.06.24
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode <= 0 ) continue; // 2013.06.12
				if ( Sch_Multi_Material_Info_Data[j]->MtlOut_Mode >= 10 ) continue; // 2013.06.12
			}
			//
			strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L2_Mdl , Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl );
			Sch_Multi_Material_Info_Data[j]->MtlOut_L2_SlotArm = Sch_Multi_Material_Info_Data[j]->MtlOut_L_SlotArm;
			//
//			if ( mode == MACRO_PICK ) { // 2014.03.18
			if ( ( mode == MACRO_PICK ) || ( mode == MACRO_PICKPLACE ) ) { // 2014.03.18
				Sch_Multi_Material_Info_Data[j]->MtlOut_L_SlotArm = arm;
				if ( rbm >= 100 ) {
					sprintf( Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl , "FM%d" , ( rbm / 100 ) + 1 );
				}
				else if ( rbm <= 0 ) {
					sprintf( Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl , "FM" );
				}
				else {
					strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( TM + rbm - 1 ) );
				}
	//			Sch_Multi_Material_Info_Data[j]->MtlOut_L_SlotArm = arm; // 2012.10.04
			}
			else {
				Sch_Multi_Material_Info_Data[j]->MtlOut_L_SlotArm = slot;
				strcpy( Sch_Multi_Material_Info_Data[j]->MtlOut_L_Mdl , _i_SCH_SYSTEM_GET_MODULE_NAME_for_SYSTEM( mdl ) );
				//
				/*
				// 2012.10.04
				if ( ( mdl >= CM1 ) && ( mdl < PM1 ) ) { // 2011.10.13
					if ( ( Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM == mdl ) && ( Sch_Multi_Material_Info_Data[j]->MtlOut_Out_Slot == slot ) ) {
						if ( Sch_Multi_Material_Info_Data[j]->MtlOut_In_CM != Sch_Multi_Material_Info_Data[j]->MtlOut_Out_CM ) {
							//
							iSCH_MR_Data_Setting_for_Place_Other_Out_Data( mdl , slot , side , pt , Sch_Multi_Material_Info_Data[j]->MtlOut_Tag , j );
							//
						}
					}
				}
				*/
				//
			}
			//
			f = j;
			//
			i = 2;
			//
			break;
		}
	}
	//
	if ( mode == MACRO_PICK ) {
		SCH_MR_Data_Logging( 31 , "Picking" , side , pt , rbm , arm , f );
	}
	else if ( mode == MACRO_PICKPLACE ) { // 2014.03.18
		SCH_MR_Data_Logging( 33 , "Swapping" , side , pt , rbm , arm , f );
	}
	else {
		SCH_MR_Data_Logging( 32 , "Placing" , side , pt , mdl , slot , f );
	}
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
BOOL SCHMRDATA_Data_Setting_for_LoadUnload( int cm , int mode ) {
	//
	BOOL Res;
	//
	_SCH_COMMON_HANDOFF_ACTION( cm , mode , 0 ); // 2014.06.27
	//
	if ( !_SCH_MULTIJOB_CHECK_EXTEND_MRDATA_USE() ) return FALSE;
	//
	EnterCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	Res = FALSE;
	switch( mode ) {
	case 1 : // Loading
		Res = SCH_MR_Data_Setting_for_Loading( cm , mode );
		break;
	case 2 : // Loaded
		Res = SCH_MR_Data_Setting_for_Loading( cm , mode );
		break;
	case 3 : // Load Fail
		Res = SCH_MR_Data_Setting_for_Loading( cm , mode );
		break;
		//
	case 4 : // Mapping
		Res = SCH_MR_Data_Setting_for_Loading( cm , mode );
		break;
	case 5 : // Mapped
		Res = SCH_MR_Data_Setting_for_Loading( cm , mode );
		break;
	case 6 : // Mapping Fail
		Res = SCH_MR_Data_Setting_for_Loading( cm , mode );
		break;

		//
	case 10 : // Unloading
		Res = SCH_MR_Data_Setting_for_Unloading( cm , mode );
		break;
	case 11 : // Unloaded
		Res = SCH_MR_Data_Setting_for_Unloading( cm , mode );
		break;
	case 12 : // Unload Fail
		Res = SCH_MR_Data_Setting_for_Unloading( cm , mode );
		break;
	}
	//
	LeaveCriticalSection( &CR_SCHMRDATA ); // 2013.06.24
	//
	return Res;
}







//=====================================================================================================================================================
//=====================================================================================================================================================
//=====================================================================================================================================================
//=====================================================================================================================================================








BOOL SCHMRDATA_Data_IN_Unload_Request_After_Place( int side , int cm ) { // 2016.10.31
	int j;
	BOOL has;
	//
	if ( _i_SCH_PRM_GET_MTLOUT_INTERFACE() >= 1 ) return FALSE;
	//
	if ( _i_SCH_SYSTEM_USING_GET( side ) < 5 ) return FALSE;
	//
	if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , cm ) != MUF_01_USE ) return FALSE;
	//
	has = FALSE;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) != SCHEDULER_READY ) return FALSE;
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) == cm ) {
			//
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) < _MOTAG_10_NEED_UPDATE ) return FALSE;
			//
			has = TRUE;
			//
		}
		//
	}
	//
	if ( !has ) return FALSE;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) != SCHEDULER_READY ) return FALSE;
		//
		if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) == cm ) {
			//
			if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) >= _MOTAG_10_NEED_UPDATE ) {
				if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , j ) == MR_PRE_1_REQUEST ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOutWait( side , j , MR_PRE_0_READY );
				}
				else if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , j ) == MR_PRE_2_REQUEST_and_WAIT ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOutWait( side , j , MR_PRE_3_WAIT );
				}
			}
			//
		}
		//
	}
	//
	return TRUE;
	//
}



BOOL SCHMRDATA_Data_IN_Unload_Request_After_Pick( int side , int pt , int cm ) { // 2016.10.31
	int j;
	//
	if ( _i_SCH_SYSTEM_USING_GET( side ) < 5 ) return FALSE;
	//
	if ( _i_SCH_MODULE_Get_Mdl_Use_Flag( side , cm ) != MUF_01_USE ) return FALSE;
	//
	if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , pt ) != MR_PRE_1_REQUEST ) {
		if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , pt ) != MR_PRE_2_REQUEST_and_WAIT ) {
			return FALSE;
		}
	}
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( j <= pt ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
		//
		if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) >= _MOTAG_10_NEED_UPDATE ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_READY ) { // 2013.05.27
			if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) != cm ) {
				if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != cm ) {
					continue;
				}
			}
			if      ( _i_SCH_CLUSTER_Get_PathIn( side , j ) == cm ) {
				if      ( _i_SCH_CLUSTER_Get_PathIn( side , j ) == _i_SCH_CLUSTER_Get_PathOut( side , j ) ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 100 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
				}
				else {
					if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_14_NEED_OUT /* OUT */ ) {
						_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 100 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
					}
					else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) < _MOTAG_10_NEED_UPDATE ) {
						_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 100 + _MOTAG_13_NEED_IN /* IN */ );
					}
				}
			}
			else if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) == cm ) {
				if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) == _MOTAG_13_NEED_IN /* IN */ ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 100 + _MOTAG_12_NEED_IN_OUT /* IN+OUT */ );
				}
				else if ( ( _i_SCH_CLUSTER_Get_Ex_MtlOut( side , j ) % 100 ) < _MOTAG_10_NEED_UPDATE ) {
					_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 100 + _MOTAG_14_NEED_OUT /* OUT */ );
				}
			}
		}
		else {
			if ( _i_SCH_CLUSTER_Get_PathOut( side , j ) != cm ) continue;
			_i_SCH_CLUSTER_Set_Ex_MtlOut( side , j , 100 + _MOTAG_14_NEED_OUT /* OUT */ );
		}
	}
	//
	return TRUE;
	//
}



BOOL SCHMRDATA_Data_IN_Wait_Reset( int side , int cm ) { // 2016.10.31
	int j;
	//
	if ( _i_SCH_SYSTEM_USING_GET( side ) < 5 ) return FALSE;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( _i_SCH_CLUSTER_Get_PathStatus( side , j ) == SCHEDULER_CM_END ) continue;
		//
		if ( cm != -1 ) {
			if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != cm ) continue;
		}
		//
		if      ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , j ) == MR_PRE_1_REQUEST ) {
			_i_SCH_CLUSTER_Set_Ex_MtlOutWait( side , j , MR_PRE_0_READY );
		}
		else if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , j ) == MR_PRE_2_REQUEST_and_WAIT ) {
			_i_SCH_CLUSTER_Set_Ex_MtlOutWait( side , j , MR_PRE_0_READY );
		}
		else if ( _i_SCH_CLUSTER_Get_Ex_MtlOutWait( side , j ) == MR_PRE_3_WAIT ) {
			_i_SCH_CLUSTER_Set_Ex_MtlOutWait( side , j , MR_PRE_0_READY );
		}
	}
	//
	return TRUE;
}

BOOL SCHMRDATA_Data_IN_Wait_Set( int side , int cm , int count , BOOL Manual ) { // 2016.10.31
	int j , c;
	//
	if ( _i_SCH_SYSTEM_USING_GET( side ) < 5 ) return FALSE;
	//
	if ( count <= 0 ) {
		return SCHMRDATA_Data_IN_Wait_Reset( side , cm );
	}
	//
	c = 0;
	//
	for ( j = 0 ; j < MAX_CASSETTE_SLOT_SIZE ; j++ ) {
		//
		if ( _i_SCH_CLUSTER_Get_PathRange( side , j ) < 0 ) continue;
		//
		if ( cm != -1 ) {
			if ( _i_SCH_CLUSTER_Get_PathIn( side , j ) != cm ) continue;
		}
		//
		c++;
		//
		if ( c >= count ) {
			_i_SCH_CLUSTER_Set_Ex_MtlOutWait( side , j , Manual ? MR_PRE_2_REQUEST_and_WAIT : MR_PRE_1_REQUEST );
		}
	}
	//
	return FALSE;
}

