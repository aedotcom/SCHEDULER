#include "default.h"

int Address_SCH_BUTTON = -2;
int Address_SCH_ERROR  = -2;
//===================================================================================================
void ERROR_HANDLER( int i , char *appendstr ) {
	if ( Address_SCH_ERROR == -2 ) Address_SCH_ERROR = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_ERROR" );
	if ( Address_SCH_ERROR != -1 ) {
		if ( strlen( appendstr ) <= 0 ) {
			_dWRITE_FUNCTIONF_EVENT( Address_SCH_ERROR , "%d" , i ); // 2007.01.04
		}
		else {
			_dWRITE_FUNCTIONF_EVENT( Address_SCH_ERROR , "%d|%s" , i , appendstr ); // 2007.01.04
		}
	}
}
//===================================================================================================
void BUTTON_HANDLER_REMOTE( LPSTR list , ... ) {
	va_list va;
	char TextBuffer[64];
	//
	if ( Address_SCH_BUTTON == -2 ) Address_SCH_BUTTON = _FIND_FROM_STRING( _K_F_IO , "SCHEDULER_BUTTON" );
	if ( Address_SCH_BUTTON != -1 ) {
		va_start( va , list );
		vsprintf( TextBuffer , list , (LPSTR) va );
		va_end( va );
		_dRUN_FUNCTION_FREE( Address_SCH_BUTTON , TextBuffer );
	}
}
//===================================================================================================
void ERROR_HANDLER2( int i , char *str ) { // 2017.02.09
	char buffer[32];
	sprintf( buffer , "ERROR=%d" , i );
	MessageBox( NULL , str , buffer , MB_ICONHAND );
}
//===================================================================================================
