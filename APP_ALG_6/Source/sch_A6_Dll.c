//================================================================================
#include "INF_default.h"
//================================================================================
#include "INF_EQ_Enum.h"
//================================================================================

typedef int  (*DLL_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE) ( int , int ); // 2007.05.03

//================================================================================
DLL_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE		DRV_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE; // 2007.05.03

//================================================================================
void INIT_SCHEDULER_AL6_PART_SET_DLL_INSTANCE( HINSTANCE dllpt , int unload ) {
	if ( !dllpt || unload ) {
		DRV_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE	= FALSE; // 2007.05.03
	}
	else {
		DRV_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE	= (DLL_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE)	GetProcAddress( dllpt , "USER_RECIPE_INVALID_DUMMY_OPERATE" ); // 2007.05.03
	}
}
//===========================================================================================================================
//=========================================================================================
// 2007.05.03
//=========================================================================================
int  USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE( int bmindex , int bmslot ) {
	if ( !DRV_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE ) {
		if ( bmslot <= 0 ) return 1;
		return 0;
	}
	return( (* DRV_USER_RECIPE_SP_N6_INVALID_DUMMY_OPERATE ) ( bmindex , bmslot ) );
}
