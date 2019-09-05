#include "default.h"

#include "EQ_Enum.h"

#include "System_Tag.h"
#include "User_Parameter.h"
#include "FA_Handling.h"
#include "IO_Part_data.h"

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
BOOL   ROBOT_ANIMATION_SUB_GO_TARGET_POSITION( double Current , double Target , double Range , double *Result ) {
	if ( Current > Target ) {
		if ( ( Current - Target ) <= Range ) {
			*Result = Target;	return TRUE;
		}
		else *Result = Current - Range;
	}
	else {
		if ( ( Target - Current ) <= Range ) {
			*Result = Target;	return TRUE;
		}
		else *Result = Current + Range;
	}
	return FALSE;
}
//
BOOL   ROBOT_ANIMATION_SUB_GO_TARGET_COMPLETED( double Current , double Target , double Range ) {
	if ( Current > Target ) {
		if ( ( Current - Target ) <= Range ) return TRUE;
	}
	else {
		if ( ( Target - Current ) <= Range ) return TRUE;
	}
	return FALSE;
}
