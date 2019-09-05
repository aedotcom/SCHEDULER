#ifndef INF_USER_SYSTEM_H
#define INF_USER_SYSTEM_H

//===================================================================================================

#ifdef PM_CHAMBER_60
	#define MAX_PM_CHAMBER_DEPTH		45
	#define MAX_BM_CHAMBER_DEPTH		15
	#define MAX_TM_CHAMBER_DEPTH		8
	//
	#define MAX_CASSETTE_SLOT_SIZE		40
	//
	#define MAX_CASSETTE_SIDE			4
#else
	#ifdef PM_CHAMBER_30
		#define MAX_PM_CHAMBER_DEPTH		30
		#define MAX_BM_CHAMBER_DEPTH		10
		#define MAX_TM_CHAMBER_DEPTH		6
		//
		#define MAX_CASSETTE_SLOT_SIZE		50
		//
		#define MAX_CASSETTE_SIDE			4
	#else
		#define MAX_PM_CHAMBER_DEPTH		12
		#define MAX_BM_CHAMBER_DEPTH		4
		#define MAX_TM_CHAMBER_DEPTH		4
		//
		#define MAX_CASSETTE_SLOT_SIZE		30
		//
		#define MAX_CASSETTE_SIDE			4
	#endif
#endif

//===================================================================================================

#endif

