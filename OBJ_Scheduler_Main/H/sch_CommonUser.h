#ifndef SCH_COMMON_USER_H
#define SCH_COMMON_USER_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void _SCH_COMMON_GUI_INTERFACE_ITEM_DISPLAY( int alg , int mode , int offset , HWND hdlg , int guiindex , char *message );

void _SCH_COMMON_GUI_INTERFACE_DATA_DISPLAY( int alg , int mode , int offset , HWND hdlg , int guiindex , int currdata );

void _SCH_COMMON_GUI_INTERFACE_DATA_CONTROL( int alg , int mode , int offset , HWND hdlg , int *currdata );

void _SCH_COMMON_GUI_INTERFACE_DATA_GET( int alg , int mode , int offset , int currdata , char *data );

BOOL _SCH_COMMON_LOCKEKEY_CHECK( int alg , int algsub , int *slinfo , BOOL );

#endif

