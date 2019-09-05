#ifndef SCH_A0_SUB_SP2_H
#define SCH_A0_SUB_SP2_H

void SCHEDULER_CONTROL_INIT_SCHEDULER_AL0_SUB2( int , int );

BOOL SCHEDULER_CONTROL_Chk_ARMINLTKS_RECOVER_AFTER_PLACE_A0SUB2( int tms ); // 2007.09.06

BOOL SCHEDULER_CONTROL_Chk_ARMINLTKS_PICK_FROM_CM_A0SUB2( int armdata , int side , int pt ); // 2007.09.06

BOOL SCHEDULER_CONTROL_Chk_ARMINLTKS_DATA_A0SUB2( int side , int pt ); // 2007.09.28

void SCHEDULER_CONTROL_Reset_CASSETTE_DATA_A0SUB2( int side , int pt , BOOL ioalso );

int  SCHEDULER_CONTROL_Set_CASSETTE_APPEND_A0SUB2_PLUSDLL( int tord , int pos , int mode , int opt , int incm , int outcm , int inslot , int outslot , int spid , char *pm1recipe , char *pm2recipe , char *pm3recipe , char *pm4recipe , char *pm5recipe , char *pm6recipe , char *sublogfolder );

int  SCHEDULER_CONTROL_Chk_CASSETTE_APPEND_A0SUB2( int side );

void SCHEDULER_CONTROL_Chk_CASSETTE_REFRESH_A0SUB2( int side ); // 2007.11.21

void SCHEDULER_CONTROL_Chk_CASSETTE_DISABLE_A0SUB2( int side );

void SCHEDULER_CONTROL_Return_CASSETTE_Operation_A0SUB2( int side , int i , int mode , int ch );

char *SCHEDULER_CONTROL_Get_SLOT_FOLDER_A0SUB2( int slot );

void SCHEDULER_CONTROL_Chk_DISABLE_DATABASE_SKIP_A0SUB2( int side , int pt );


#endif

