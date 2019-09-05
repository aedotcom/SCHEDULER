#ifndef IOLOG__H
#define IOLOG__H

//------------------------------------------------------------------------------------------
//#define  SCHEDULER_DEBUG_DATA_FILE	"history\\schdata.log" // 2006.02.24
//#define  SCHEDULER_DEBUG_FLOW_FILE	"history\\schdebug.log" // 2006.02.24
//------------------------------------------------------------------------------------------
extern char SCHEDULER_DEBUG_DATA_FILE[64]; // 2006.02.24
extern char SCHEDULER_DEBUG_FLOW_FILE[64]; // 2006.02.24
//==============================================================================================================
void IO_LOG_INIT();
//==============================================================================================================
void IO_LOT_DIR_INITIAL( int mode , char *directory , char *folder );

void IO_LOT_DIR_USER_MESSAGE( int M , char *data ); // 2012.02.17
void IO_LOT_TIMER_USER_MESSAGE( char *data ); // 2012.02.17

//==============================================================================================================
//void IO_MSG_PRINTF( LPSTR , ... );
//==============================================================================================================
void IO_DEBUG_INIT();
BOOL IO_DEBUG_ENABLE( int group );
void IO_DEBUG_DISABLE( BOOL alw );
void IO_DEBUG_WAITING( BOOL );
BOOL IO_DEBUG_FILE_ENABLE( int group , char * , int , BOOL ); // 2005.03.15
BOOL IO_DEBUG_RUNNING(); // 2008.04.18
//==============================================================================================================
int  IO_LOT_LOG_STATUS( int mode );
//==============================================================================================================
void _IO_MSGEVENT_PRINTF( char *SeparateSting , char *data ); // 2007.07.04
//==============================================================================================================
void _IO_DLL_MESSAGE_PRINTF( char *SeparateSting , LPSTR list , ... ); // 2008.04.01
//==============================================================================================================

void _i_SCH_LOG_LOT_PRINTF( int mode , LPSTR , ... );
void _i_SCH_LOG_DEBUG_PRINTF( int mode , int tm , LPSTR , ... );
void _i_SCH_LOG_TIMER_PRINTF( int side , int mode , int i1 , int i2 , int i3 , int i4 , int i5 , char *s1 , char *s2 );

//
void f_enc_printf( FILE *fpt , LPSTR list , ... );

//
void xprintf( LPSTR list , ... ); // for Test log

#endif
