#include "INF_default.h"
#include "sch_A0_Main.h"

int _SCH_INF_DLL_API_ENTER_CONTROL_FEM_PART( int CHECKING_SIDE, int mode )
{
	return USER_DLL_SCH_INF_ENTER_CONTROL_FEM_STYLE_0( CHECKING_SIDE, mode );
}


BOOL _SCH_INF_DLL_API_RUN_CONTROL_FEM_PART( int CHECKING_SIDE )
{
	return USER_DLL_SCH_INF_RUN_CONTROL_FEM_STYLE_0( CHECKING_SIDE );
}
