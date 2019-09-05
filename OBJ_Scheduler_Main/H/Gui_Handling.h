#ifndef GUI_HANDLING_H
#define GUI_HANDLING_H

//----------------------------------------------------------------------------
void GUI_SAVE_PARAMETER_INIT(); // 2015.01.23
//----------------------------------------------------------------------------
void GUI_SAVE_PARAMETER_DATA( int mode );
//----------------------------------------------------------------------------
void GUI_PROCESS_LOG_MONITOR_RUN();
int  GUI_PROCESS_LOG_MONITOR_GET();
void GUI_PROCESS_LOG_MONITOR_RESET();
//----------------------------------------------------------------------------
void GUI_PROCESS_LOG_MONITOR2_SET( int , int , int , int , char * );
void GUI_PROCESS_LOG_MONITOR2_RUN( int );
int  GUI_PROCESS_LOG_MONITOR2_GET( int );
int  GUI_PROCESS_LOG_MONITOR2_GSD( int );
int  GUI_PROCESS_LOG_MONITOR2_WID( int );
char *GUI_PROCESS_LOG_MONITOR2_GRCP( int );
//----------------------------------------------------------------------------
void GUI_GROUP_SELECT_SET( int data );
int  GUI_GROUP_SELECT_GET();
//----------------------------------------------------------------------------
void GUI_Parameter_Drawing( HWND hWnd , HDC hDC );
BOOL GUI_Parameter_Control( HWND hWnd , long msg );
void GUI_Parameter_Show( HWND hWnd );
void GUI_LotData_Show( HWND hWnd );
//----------------------------------------------------------------------------
void GUI_Receive_Event_Inserting( char *message );
void GUI_Reject_Event_Inserting( char *message );
void GUI_Send_Event_Inserting( char *message );
//----------------------------------------------------------------------------
void Event_Message_Reject( char *data );
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GUI_RB_DISPLAY_SET( int data );
int  GUI_RB_DISPLAY_GET();

void GUI_TM_DISPLAY_SET( int data );
int  GUI_TM_DISPLAY_GET();

void GUI_FM_DISPLAY_SET( int data );
int  GUI_FM_DISPLAY_GET();
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GUI_INT_STRING_DISPLAY( HWND hWnd , int id ,
							int data , char *dstr , char *de ); // 2007.07.24
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL APIENTRY Gui_IDD_SYSTEM_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_MODULE_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_FIXEDCLST_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_ROBOTDATA_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_RCPFILE_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_RERCP_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_CLSSNR_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_FAILSNR_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_CHINTERLOCK_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_HANDLINGSIDE_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
BOOL APIENTRY Gui_IDD_FA_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );

BOOL APIENTRY Gui_IDD_STATUS_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );

BOOL APIENTRY Gui_IDD_ROBOTCAL_PAD_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );
//----------------------------------------------------------------------------

#endif