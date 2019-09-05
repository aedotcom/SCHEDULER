#ifndef SCH_HANDOFF_H
#define SCH_HANDOFF_H

int  Scheduler_HandOffIn_Part( int CHECKING_SIDE , int ch1 , int ch2 );
int  Scheduler_HandOffOut_Part( int CHECKING_SIDE , BOOL run , int *ch1 , int *ch2 );

int  Scheduler_HandOffIn_Ready_Part( int CHECKING_SIDE , int ch );
int  Scheduler_HandOffOut_End_Part( int CHECKING_SIDE , int ch , int pt , int w );
void Scheduler_HandOffOut_Clear(); // 2011.06.01
void Scheduler_HandOffOut_Skip_Set( int ch ); // 2011.06.01
int  Scheduler_HandOffOut_Skip_Get( int ch ); // 2011.06.01

#endif

