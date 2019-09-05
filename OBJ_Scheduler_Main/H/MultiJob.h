#ifndef MULTIJOB_H
#define MULTIJOB_H

//===================================================================================
#include "lottype.h"
#include "schjobintf.h"
//===================================================================================
#define COUNTER_CHAR		sizeof(char)	//1
#define COUNTER_INT			sizeof(int)		//4
#define COUNTER_LONG		sizeof(long)	//4
#define COUNTER_FLOAT		sizeof(float)	//4
#define COUNTER_DOUBLE		sizeof(double)	//8
#define COUNTER_CLOCK		sizeof(long)
#define COUNTER_BOOL		sizeof(char)
//===================================================================================
enum {
	ENUM_REJECT_PROCESSJOB_NONE ,
	ENUM_REJECT_PROCESSJOB_INSERT ,
	ENUM_REJECT_PROCESSJOB_CHANGE ,
	ENUM_REJECT_PROCESSJOB_DELETE ,
	ENUM_REJECT_PROCESSJOB_VERIFY ,
	ENUM_REJECT_PROCESSJOB_PAUSE ,
	ENUM_REJECT_PROCESSJOB_RESUME ,
	ENUM_REJECT_PROCESSJOB_STOP ,
	ENUM_REJECT_PROCESSJOB_ABORT ,
	ENUM_REJECT_PROCESSJOB_CANCEL ,
	ENUM_REJECT_PROCESSJOB_UPDATE ,
};
//
enum {
	ENUM_ACCEPT_PROCESSJOB_NONE ,
	ENUM_ACCEPT_PROCESSJOB_INSERT ,
	ENUM_ACCEPT_PROCESSJOB_CHANGE ,
	ENUM_ACCEPT_PROCESSJOB_DELETE ,
	ENUM_ACCEPT_PROCESSJOB_VERIFY ,
	ENUM_ACCEPT_PROCESSJOB_PAUSING ,
	ENUM_ACCEPT_PROCESSJOB_RESUME ,
	ENUM_ACCEPT_PROCESSJOB_STOP ,
	ENUM_ACCEPT_PROCESSJOB_ABORT ,
	ENUM_ACCEPT_PROCESSJOB_CANCEL ,
	ENUM_ACCEPT_PROCESSJOB_UPDATE ,
};
//
enum {
	ENUM_NOTIFY_PROCESSJOB_NONE ,
	ENUM_NOTIFY_PROCESSJOB_FINISH ,
	ENUM_NOTIFY_PROCESSJOB_START ,
	ENUM_NOTIFY_PROCESSJOB_WAITVERIFY ,
	ENUM_NOTIFY_PROCESSJOB_FIRSTSTART ,
	ENUM_NOTIFY_PROCESSJOB_LASTFINISH ,
	ENUM_NOTIFY_PROCESSJOB_PAUSE
};
//
enum {
	ENUM_REJECT_CONTROLJOB_NONE ,
	ENUM_REJECT_CONTROLJOB_CHGSELECT ,
	ENUM_REJECT_CONTROLJOB_INSERT ,
	ENUM_REJECT_CONTROLJOB_CHANGE ,
	ENUM_REJECT_CONTROLJOB_DELETE ,
	ENUM_REJECT_CONTROLJOB_HEADOFQ ,
	ENUM_REJECT_CONTROLJOB_DELSELECT ,
	ENUM_REJECT_CONTROLJOB_VERIFY ,
	ENUM_REJECT_CONTROLJOB_PAUSE ,
	ENUM_REJECT_CONTROLJOB_RESUME ,
	ENUM_REJECT_CONTROLJOB_STOP ,
	ENUM_REJECT_CONTROLJOB_ABORT
};
//
enum {
	ENUM_ACCEPT_CONTROLJOB_NONE ,
	ENUM_ACCEPT_CONTROLJOB_CHGSELECT ,
	ENUM_ACCEPT_CONTROLJOB_INSERT ,
	ENUM_ACCEPT_CONTROLJOB_CHANGE ,
	ENUM_ACCEPT_CONTROLJOB_DELETE ,
	ENUM_ACCEPT_CONTROLJOB_HEADOFQ ,
	ENUM_ACCEPT_CONTROLJOB_DELSELECT ,
	ENUM_ACCEPT_CONTROLJOB_VERIFY ,
	ENUM_ACCEPT_CONTROLJOB_PAUSE ,
	ENUM_ACCEPT_CONTROLJOB_RESUME ,
	ENUM_ACCEPT_CONTROLJOB_STOP ,
	ENUM_ACCEPT_CONTROLJOB_ABORT
};
//
enum {
	ENUM_NOTIFY_CONTROLJOB_NONE ,
	ENUM_NOTIFY_CONTROLJOB_SELECT ,
	ENUM_NOTIFY_CONTROLJOB_FINISH ,
	ENUM_NOTIFY_CONTROLJOB_START ,
	ENUM_NOTIFY_CONTROLJOB_WAITVERIFY ,
	ENUM_NOTIFY_CONTROLJOB_CANCEL
};
//===================================================================================
typedef struct {
	int  RunStatus; // CassRunJob_Control_Status
	int  Cassette;
	int  SelectCJIndex;
	int  SelectCJOrder; // 2013.09.27
	//---------------------------------------------------------------------
	char CarrierID[ 128 + 1 ];

//	int  OutCassette; // 2009.04.03 // 2011.02.15(Testing) // 2011.05.21
//	char OutCarrierID[ 128 + 1 ]; // 2009.04.03 // 2011.02.15(Testing) // 2011.05.21

	int  MtlCJ_StartMode; // Auto / Manual

	char MtlCJName[ 128 + 1 ];

	int  MtlCount;
	//
	/*
	//
	// 2018.11.15
	//
	int  MtlPJ_StartMode[MAX_CASSETTE_SLOT_SIZE];
	char MtlPJName[MAX_CASSETTE_SLOT_SIZE][ 128 + 1 ];
	int  MtlPJID[MAX_CASSETTE_SLOT_SIZE];
	int  MtlPJIDRes[MAX_CASSETTE_SLOT_SIZE];
	int  MtlPJIDEnd[MAX_CASSETTE_SLOT_SIZE];
//	int  MtlInSlot[MAX_CASSETTE_SLOT_SIZE]; // 2012.04.01(Testing)
	char MtlRecipeName[MAX_CASSETTE_SLOT_SIZE][ 128 + 1 ];
	//
	int  MtlInCassette[MAX_CASSETTE_SLOT_SIZE]; // 2012.04.01(Testing)
	char MtlInCarrierID[MAX_CASSETTE_SLOT_SIZE][ 128 + 1 ]; // 2012.04.01(Testing)
	//
	int  MtlInSlot[MAX_CASSETTE_SLOT_SIZE]; // 2012.04.01(Testing)
	//
	int  MtlOutCassette[MAX_CASSETTE_SLOT_SIZE]; // 2011.02.15(Testing) // 2011.05.21
	char MtlOutCarrierID[MAX_CASSETTE_SLOT_SIZE][ 128 + 1 ]; // 2011.02.15(Testing) // 2011.05.21

	int  MtlOutSlot[MAX_CASSETTE_SLOT_SIZE];
	//
	int  MtlRecipeMode[MAX_CASSETTE_SLOT_SIZE]; // * // Only Recipe (0): Tunning(1)
	//
	*/
	//
	//
	// 2018.11.15
	//
	int  MtlPJ_StartMode[MAX_RUN_CASSETTE_SLOT_SIZE];
	char MtlPJName[MAX_RUN_CASSETTE_SLOT_SIZE][ 128 + 1 ];
	int  MtlPJID[MAX_RUN_CASSETTE_SLOT_SIZE];
	int  MtlPJIDRes[MAX_RUN_CASSETTE_SLOT_SIZE];
	int  MtlPJIDEnd[MAX_RUN_CASSETTE_SLOT_SIZE];
	char MtlRecipeName[MAX_RUN_CASSETTE_SLOT_SIZE][ 128 + 1 ];
	//
	int  MtlInCassette[MAX_RUN_CASSETTE_SLOT_SIZE]; // 2012.04.01(Testing)
	char MtlInCarrierID[MAX_RUN_CASSETTE_SLOT_SIZE][ 128 + 1 ]; // 2012.04.01(Testing)
	//
	int  MtlInSlot[MAX_RUN_CASSETTE_SLOT_SIZE]; // 2012.04.01(Testing)
	//
	int  MtlOutCassette[MAX_RUN_CASSETTE_SLOT_SIZE]; // 2011.02.15(Testing) // 2011.05.21
	char MtlOutCarrierID[MAX_RUN_CASSETTE_SLOT_SIZE][ 128 + 1 ]; // 2011.02.15(Testing) // 2011.05.21

	int  MtlOutSlot[MAX_RUN_CASSETTE_SLOT_SIZE];
	//
	int  MtlRecipeMode[MAX_RUN_CASSETTE_SLOT_SIZE]; //*// Only Recipe (0): Tunning(1)

} Scheduling_Multi_RunJob;
//===================================================================================
//===================================================================================
typedef struct {
	//---------------------------------------------------------------------
	time_t InsertTime;
	//---------------------------------------------------------------------
	int	FixedResult; // 2018.11.14
	//---------------------------------------------------------------------
} Scheduling_Multi_Ins_ProcessJob;
//===================================================================================
//===================================================================================
typedef struct {
	//---------------------------------------------------------------------
	time_t InsertTime;
	//---------------------------------------------------------------------
	int	   Queued_to_Select_Wait; // 2012.09.26
	//---------------------------------------------------------------------
	int	   MoveMode; // 2013.09.03
	//---------------------------------------------------------------------
	int	FixedResult; // 2018.11.14
	//---------------------------------------------------------------------
} Scheduling_Multi_Ins_ControlJob;
//===================================================================================
//===================================================================================

BOOL SCHMULTI_INIT();

int SCHMULTI_CASSETTE_VERIFY_GET( int side );
char *SCHMULTI_CASSETTE_MID_GET( int side );
char *SCHMULTI_CASSETTE_MID_GET_FOR_START( int side );

void SCHMULTI_RCPTUNE_EXTEND_USE_SET( int mode );
 // 2007.11.28
char *SCHMULTI_TUNE_INFO_DATA_GET( int ch ); // 2007.06.08
void  SCHMULTI_TUNE_INFO_DATA_RESET( int ch ); // 2007.06.08

void SCHMULTI_CTJOB_USE_SET( BOOL );
BOOL SCHMULTI_CTJOB_USE_GET();

void SCHMULTI_CTJOB_SINGLE_MODE_SET( BOOL );
BOOL SCHMULTI_CTJOB_SINGLE_MODE_GET();

void SCHMULTI_DEFAULT_GROUP_SET( char *data );
char *SCHMULTI_DEFAULT_GROUP_GET();

void SCHMULTI_CHECK_OVERTIME_HOUR_SET( int data ); // 2012.04.20
int  SCHMULTI_CHECK_OVERTIME_HOUR_GET(); // 2012.04.20

//void SCHMULTI_DEFAULT_LOTRECIPE_READ_SET( int data ); // 2003.09.24 // 2014.06.23
//int  SCHMULTI_DEFAULT_LOTRECIPE_READ_GET(); // 2003.09.24 // 2014.06.23

void _i_SCH_MULTIJOB_SET_LOTRECIPE_READ( int data ); // 2003.09.24 // 2014.06.23
int  _i_SCH_MULTIJOB_GET_LOTRECIPE_READ(); // 2003.09.24 // 2014.06.23

void SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_SET( int data ); // 2004.05.11
int  SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_GET(); // 2004.05.11

void SCHMULTI_AFTER_SELECT_CHECK_SET( int data ); // 2004.06.26
int  SCHMULTI_AFTER_SELECT_CHECK_GET(); // 2004.06.26

void SCHMULTI_JOBNAME_DISPLAYMODE_SET( int data ); // 2005.05.11
int  SCHMULTI_JOBNAME_DISPLAYMODE_GET(); // 2005.05.11

void SCHMULTI_MESSAGE_SCENARIO_SET( int data ); // 2006.04.13
int  SCHMULTI_MESSAGE_SCENARIO_GET(); // 2006.04.13

int  SCHMULTI_GET_AFTERSEL_WAITMODE( int ); // 2007.10.02

void SCHMULTI_MAKE_MANUAL_CJPJJOBMODE_DATA( int index , char *cjname , char *pjname , BOOL *MultiCJ ); // 2004.05.11

//void SCHMULTI_ALL_FORCE_CHECK( int Cass ); // 2004.10.14 // 2018.08.30
void SCHMULTI_ALL_FORCE_CHECK( int Side ); // 2004.10.14 // 2018.08.30

void SCHMULTI_ALL_QUEUE_DELETE_CHECK(); // 2016.06.10

BOOL SCHMULTI_CONTROLLER_MONITORING( int slinfo );

void SCHMULTI_CTJOB_IO_SET();
void SCHMULTI_PRJOB_IO_SET();
//
//BOOL SCHMULTI_RUNJOB_STOPPED( int Cass_Side ); // 2016.09.13 // 2018.08.30
BOOL SCHMULTI_RUNJOB_STOPPED( int Side ); // 2016.09.13 // 2018.08.30

//
BOOL SCHMULTI_SELECTDATA_FULL_MTLCOUNT( int index ); // 2018.05.10
int  SCHMULTI_SELECTDATA_GET_MTLCOUNT( int index );
void SCHMULTI_SELECTDATA_SET_SELECTCJINDEX( int index , int data ); // 2003.07.23
void SCHMULTI_SELECTDATA_SET_SELECTCJORDER( int index , int data ); // 2013.09.27
void SCHMULTI_SELECTDATA_SET_MTLCOUNT( int index , int data );
void SCHMULTI_SELECTDATA_SET_RUNSTATUS( int index , int data );
void SCHMULTI_SELECTDATA_SET_RUNSIDE( int index , int data ); // 2018.08.30
void SCHMULTI_SELECTDATA_SET_CASSETTE( int index , int data );
void SCHMULTI_SELECTDATA_SET_CASSETTEINDEX( int index , int data ); // 2018.05.24
void SCHMULTI_SELECTDATA_SET_CARRIERID( int index , char *data );

//void SCHMULTI_SELECTDATA_SET_OUTCARRIERID( int index , char *data ); // 2011.02.15(Testing) // 2011.05.21

void SCHMULTI_SELECTDATA_SET_MTLCJNAME( int index , char *data );
void SCHMULTI_SELECTDATA_SET_MTLCJSTARTMODE( int index , int data );
int  SCHMULTI_SELECTDATA_GET_MTLINSLOT( int index , int subslot );
int  SCHMULTI_SELECTDATA_GET_MTLOUTSLOT( int index , int subslot );
void SCHMULTI_SELECTDATA_SET_MTLINSLOT( int index , int subslot , int data );

void SCHMULTI_SELECTDATA_SET_MTLINCASSETTE( int index , int subslot , int data ); // 2012.04.01(Testing)
void SCHMULTI_SELECTDATA_SET_MTLINCARRIERID( int index , int subslot , char *data ); // 2012.04.01(Testing)
int  SCHMULTI_SELECTDATA_GET_MTLINCASSETTE( int index , int subslot ); // 2012.04.01(Testing)
char *SCHMULTI_SELECTDATA_GET_MTLINCARRIERID( int index , int subslot ); // 2012.04.01(Testing)

void SCHMULTI_SELECTDATA_SET_MTLOUTCASSETTE( int index , int subslot , int data ); // 2011.02.15(Testing) // 2011.05.21
void SCHMULTI_SELECTDATA_SET_MTLOUTCARRIERID( int index , int subslot , char *data ); // 2011.02.15(Testing) // 2011.05.21
int  SCHMULTI_SELECTDATA_GET_MTLOUTCASSETTE( int index , int subslot ); // 2011.02.15(Testing) // 2011.05.21
char *SCHMULTI_SELECTDATA_GET_MTLOUTCARRIERID( int index , int subslot ); // 2011.02.15(Testing) // 2011.05.21


void SCHMULTI_SELECTDATA_SET_MTLSIDE( int index , int subslot , int data ); // 2018.05.10
int  SCHMULTI_SELECTDATA_GET_MTLSIDE( int index , int subslot ); // 2018.05.10
void SCHMULTI_SELECTDATA_SET_MTLPOINTER( int index , int subslot , int data ); // 2018.05.10
int  SCHMULTI_SELECTDATA_GET_MTLPOINTER( int index , int subslot ); // 2018.05.10
void SCHMULTI_SELECTDATA_SET_MTLINCARRIERINDEX( int index , int subslot , int data ); // 2018.05.10
int  SCHMULTI_SELECTDATA_GET_MTLINCARRIERINDEX( int index , int subslot ); // 2018.05.10
void SCHMULTI_SELECTDATA_SET_MTLOUTCARRIERINDEX( int index , int subslot , int data ); // 2018.05.10
int  SCHMULTI_SELECTDATA_GET_MTLOUTCARRIERINDEX( int index , int subslot ); // 2018.05.10

void SCHMULTI_RUNJOB_DATA_SET( int side , int pt ); // 2018.05.10

int  SCHMULTI_GET_VALID_CASSETTE( int cass , char *name ); // 2011.05.21

BOOL SCHMULTI_CHECK_ANOTHER_OUT_CASS_USING( BOOL reset , BOOL incheck , BOOL alwaychk , int cm , BOOL *used );
int  SCHMULTI_RUNJOB_GET_SUPPLY_POSSIBLE_FOR_OUTCASS( int side , int pointer , BOOL inmode , BOOL *return_impossible , BOOL chkmode ); // 2011.03.30

void SCHMULTI_SELECTDATA_SET_MTLOUTSLOT( int index , int subslot , int data );
void SCHMULTI_SELECTDATA_SET_MTLPJSTARTMODE( int index , int subslot , int data );
void SCHMULTI_SELECTDATA_SET_MTLPJIDRES( int index , int subslot , int data );
void SCHMULTI_SELECTDATA_SET_MTLPJIDEND( int index , int subslot , int data );
void SCHMULTI_SELECTDATA_SET_MTLPJID( int index , int subslot , int data );
int  SCHMULTI_SELECTDATA_GET_MTLPJID( int index , int subslot );
void SCHMULTI_SELECTDATA_SET_MTLPJNAME( int index , int subslot , char *data );
void SCHMULTI_SELECTDATA_SET_MTLRECIPEMODE( int index , int subslot , int data );
void SCHMULTI_SELECTDATA_SET_MTLRECIPENAME( int index , int subslot , char *data );
void SCHMULTI_SELECTDATA_CLEAR_CHECK_WITH_CJOB_AT_MANUAL( BOOL ManulmultiCJmode ); // 2004.04.27
//
void SCHMULTI_GET_PROCESSJOB_ERRORDATA( char *data ); // 2015.03.27
void SCHMULTI_GET_CONTROLJOB_ERRORDATA( char *data ); // 2015.03.27

void SCHMULTI_CONTROLLER_LOGGING( int );
void SCHMULTI_CONTROLLER_LOGSET( int Mode , int data );

int  SCHMULTI_CPJOB_RESET();

int  SCHMULTI_CONTROLJOB_INSERT( BOOL , BOOL );
int  SCHMULTI_CONTROLJOB_CHANGE();
int  SCHMULTI_CONTROLJOB_DELETE( int index );
int  SCHMULTI_CONTROLJOB_DELETE_CM( int Cass );
int  SCHMULTI_CONTROLJOB_DELSELECT( int User , int , int , int watfor );
int  SCHMULTI_CONTROLJOB_HEADOFQ();
int  SCHMULTI_CONTROLJOB_CHGSELECT();
int  SCHMULTI_CONTROLJOB_PAUSE( char *name );
int  SCHMULTI_CONTROLJOB_RESUME( char *name );
int  SCHMULTI_CONTROLJOB_STOP( char *name );
int  SCHMULTI_CONTROLJOB_ABORT( char *name );
BOOL SCHMULTI_CONTROLJOB_MANUAL_POSSIBLE();
int  SCHMULTI_CONTROLJOB_Queued_to_Select_Wait( BOOL Reset ); // 2012.09.26


//int  SCHMULTI_PROCESSJOB_INSERT( int Cass ); // 2016.09.01
int  SCHMULTI_PROCESSJOB_INSERT( int Cass , BOOL LockingNoError ); // 2016.09.01
int  SCHMULTI_PROCESSJOB_CHANGE();
int  SCHMULTI_PROCESSJOB_UPDATE();
int  SCHMULTI_PROCESSJOB_DELETE( int , int );
int  SCHMULTI_PROCESSJOB_PAUSE();
int  SCHMULTI_PROCESSJOB_RESUME();
int  SCHMULTI_PROCESSJOB_STOP();
int  SCHMULTI_PROCESSJOB_ABORT();
int  SCHMULTI_PROCESSJOB_MANUAL_INSERT_and_SETUP( int Cass , char *pjname , char *recipename , char *midname ); // 2004.04.27
void SCHMULTI_PROCESSJOB_MANUAL_SET_MTLSLOT( int pjindex , int index , int data ); // 2004.04.27
//
int  SCHMULTI_CASSETTE_VERIFY_MAKE_OK( int side );
void SCHMULTI_CASSETTE_VERIFY_CLEAR( int side , BOOL );
BOOL SCHMULTI_CASSETTE_VERIFY_MAKE_SET_AGAIN( int side ); // 2017.05.22
//
int  SCHMULTI_RUNJOB_PROCESSJOB_MAKE_PAUSE( int , char *data );
int  SCHMULTI_RUNJOB_PROCESSJOB_MAKE_RESUME( int , char *data );
int  SCHMULTI_RUNJOB_PROCESSJOB_MAKE_STOP( int , char *data );
int  SCHMULTI_RUNJOB_PROCESSJOB_MAKE_ABORT( int , char *data ); // 2004.06.15
//
BOOL SCHMULTI_RUNJOB_GET_STOPPED( int side , int pointer ); // 2015.03.27
void SCHMULTI_RUNJOB_RESET_FOR_STOP( int side , int pointer );
//
BOOL SCHMULTI_RUNJOB_GET_TUNING_INFO( int mode , int side , int pointer , int ch , int slot , char *allrcp );
int  SCHMULTI_RUNJOB_DELETE_TUNING_INFO( BOOL , int side , int pointer ); // 2011.06.15
//
int  SCHMULTI_RUNJOB_SET_PROCESSJOB_POSSIBLE();
int  SCHMULTI_RUNJOB_SET_CONTROLJOB_POSSIBLE();
int  SCHMULTI_RUNJOB_GET_CONTROLJOB_POSSIBLE( int , int cass , char *midstring , int *mode , int *Res , int *carrmode , int *outcassmode , int *movemode );
//
int  SCHMULTI_RECIPE_LOT_DATA_MODE( int side ); // 2015.10.12
//BOOL SCHMULTI_RECIPE_LOT_DATA_READ( int side , LOTStepTemplate * , LOTStepTemplate1G * , LOTStepTemplate2 * , LOTStepTemplate3 * , char * , int *mode ); // 2015.10.12
//BOOL SCHMULTI_RECIPE_LOT_DATA_READ( int side , LOTStepTemplate * , LOTStepTemplate1G * , LOTStepTemplate2 * , LOTStepTemplate3 * , char * ); // 2015.10.12 // 2016.07.13
BOOL SCHMULTI_RECIPE_LOT_DATA_READ( int side , LOTStepTemplate * , LOTStepTemplate1G * , LOTStepTemplate2 * , LOTStepTemplate3 * , LOTStepTemplate_Dummy_Ex * , char * ); // 2015.10.12 // 2016.07.13
BOOL SCHMULTI_RECIPE_LOT_DATA_SINGLE_READ( int side , int start , char *lotname , char *pjname , int *slot , int *nstart ); // 2004.09.24
BOOL SCHMULTI_RECIPE_LOT_DATA_SINGLE_GET_NAME( int side , char *lotname , char *pjname );
BOOL SCHMULTI_RECIPE_LOT_DATA_REMAP( int side , LOTStepTemplate *LOT_RECIPE , LOTStepTemplate3 * ); // 2015.10.12

BOOL SCHMULTI_MESSAGE_GET_ALL_SUB( int side , int pointer , char *data , int size , int ppid );
BOOL SCHMULTI_MESSAGE_GET_ALL( int side , int pointer , char *data , int size );
//BOOL SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( int side , int pointer , int side_x0 , int pointer_x0 , int side_x1 , int pointer_x1 , int side_x2 , int pointer_x2 , int side_x3 , int pointer_x3 , int side_x4 , int pointer_x4 , char *data , int size , BOOL incseps ); // 2014.02.28
BOOL SCHMULTI_MESSAGE_GET_ALL_for_PROCESS_LOG( int side , int pointer , int ch , int sx , int *mwafer , int *mside , int *mpointer , char *data , int size , BOOL incseps ); // 2014.02.28
BOOL SCHMULTI_MESSAGE_GET_PPID( int side , int pointer , int mode , int wfrpos , int option , BOOL dual , int ch , char *data , int size ); // 2007.08.14
//-------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_GET_PROCESSJOB_NAME( int side , int pointer , char *data , int size );
BOOL _i_SCH_MULTIJOB_GET_CONTROLJOB_NAME( int side , int pointer , char *data , int size );
//-------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_CHECK_POSSIBLE_PLACE_TO_CM( int side , int pointer ); // 2013.05.27
//-------------------------------------------------------------
void SCHMULTI_CASSETTE_VERIFY_ERROR_MESSAGE( int side , int code );
void SCHMULTI_MESSAGE_PROCESSJOB_REJECT( int action , int code , int side , int pt );
void SCHMULTI_MESSAGE_PROCESSJOB_ACCEPT( int action , int code , char *data , int side , int pt );
void SCHMULTI_MESSAGE_PROCESSJOB_NOTIFY( int action , int code , int option , char *data , int side , int pt );
void SCHMULTI_MESSAGE_CONTROLJOB_REJECT( int action , int code , int side , int pt );
void SCHMULTI_MESSAGE_CONTROLJOB_ACCEPT( int action , int code , char *data , int side , int pt );
void SCHMULTI_MESSAGE_CONTROLJOB_NOTIFY( int action , int code , char *data , int side , int pt );
//---------------------------------------------------------------------------------------
void SCHMULTI_MESSAGE_PROCESSJOB_FINISH_CHECK( int side , int error );
//---------------------------------------------------------------------------------------
BOOL SCHMULTI_PROCESSJOB_PROCESS_FIRST_START_CHECK( int side , int pointer , int WfrID );
BOOL SCHMULTI_PROCESSJOB_PROCESS_LAST_END_CHECK( int side , int pointer , int WfrID , int Next );

void SCHMULTI_RUNJOB_PJID_DATA_REBUILD(); // 2005.05.19

void SCHEDULING_DATA_STRUCTURE_JOB_LOG( char *filename , char *dispdata );

int  SCHMULTI_PROCESSJOB_DIRECT_START_INSERT( int Cass , char *recipename , int cmout , int startslot , int endslot , char *CarrName , BOOL , int , int ); // 2008.04.23

int  SCHMULTI_FORCE_MAKE_VERIFY_FOR_CM( int CassIndex , int Mode ); // 2019.04.12

//---------------------------------------------------------------------------------------
int  _i_SCH_MULTIJOB_GET_PROCESSJOB_POSSIBLE( int side , int pt , int mode );
int  _i_SCH_MULTIJOB_SET_CJPJJOB_RESULT( int side , int pt , int mode , int Result ); // 2018.11.14
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_PROCESSJOB_START_WAIT_CHECK( int side , int pointer ); // 2004.05.10
void _i_SCH_MULTIJOB_PROCESSJOB_END_CHECK( int side , int pointer );
//---------------------------------------------------------------------------------------
void _i_SCH_MULTIJOB_SUBSTRATE_MESSAGE( int Side , int pointer , int Type , int mode , int module , int where , int arm , int waferid , int srcwaferid );
//---------------------------------------------------------------------------------------
BOOL _i_SCH_MULTIJOB_RUNJOB_REMAP_OTHERCASS( int index , int trgCass , int srcCass ); // 2006.03.08
//---------------------------------------------------------------------------------------
//BOOL SCHMULTI_RUNJOB_CHECK_TUNING_INFO_SAME( int side1 , int pathin1 , int slot1 , int side2 , int pathin2 , int slot2 ); // 2014.10.31 // 2018.08.22
BOOL SCHMULTI_RUNJOB_CHECK_TUNING_INFO_SAME( int side1 , int pathin1 , int slot1 , int side2 , int pathin2 , int slot2 , int mode ); // 2014.10.31 // 2018.08.22

void SCHMULTI_RUNJOB_SET_DATABASE_LINK( int side );

char *SCHMULTI_RUNJOB_GET_DATABASE_LINK_PJ( int mode , int side , int pt ); // 2011.09.20

BOOL SCHMULTI_RUNJOB_GET_DATABASE_J_START( int side ); // 2012.06.28

void SCHMULTI_ReSet_For_Move( BOOL , int tside , int p2 , int side , int p1 );

void SCHMULTI_SET_LOG_MESSAGE( char *data );

BOOL SCHMULTI_RCPLOCKING_GET_FILENAME( int mode , int fileorg , char *pjname , char *RunGroup , char *rcpName , char *retname , int cnt );

#endif

