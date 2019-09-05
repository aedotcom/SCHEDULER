#ifndef RESTART_H
#define RESTART_H

void INIT_SCH_RESTART_CONTROL( int apmode , int side );

int  SCH_RESTART_GET_IO_DATA( int ch , int sl , int *s , int *w , int *r );

void SCH_RESTART_SET_FOLDER( char *folder );

void SCH_RESTART_DELETE_INFO( BOOL cruse );

int  SCH_RESTART_GET_DATA_INFO( int controlmode , int side , int pt , int fmdl , int fslotarm , int smdl , int sslotarm , int rside , int rwfr );

int  SCH_RESTART_GET_DATA_ALL_INFO( int controlmode , int side , int pt , int cmd , int cslarm , int wfr_side , int wfr_sts , int *rmd , int *rslarm );

void SCH_RESTART_GET_DATA_INIT();

void SCH_RESTART_DATA_REMAP_AFTER_USER_PART( int side , int ftm );

#endif

