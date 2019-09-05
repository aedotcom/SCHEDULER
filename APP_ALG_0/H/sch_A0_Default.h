#ifndef SCH_A0_DEFAULT_H
#define SCH_A0_DEFAULT_H

//-------------------------------------------------------------------------------
enum {
	A0_SUB_ALGORITHM_DEFAULT ,
	A0_SUB_ALGORITHM_NO1 ,
	A0_SUB_ALGORITHM_NO2 ,
	A0_SUB_ALGORITHM_NO3 ,
	A0_SUB_ALGORITHM_NO4 ,
	A0_SUB_ALGORITHM_NO5 ,
	A0_SUB_ALGORITHM_NO6 ,
	A0_SUB_ALGORITHM_NO7 , // 2018.12.07
};

//-------------------------------------------------------------------------------
//RUN_ALGORITHM_DEFAULT
//	- A0_SUB_ALGORITHM_NO1	: C1021 ASHER-WET	:	Not Use
//	- A0_SUB_ALGORITHM_NO2	: C1026 LCD
//	- A0_SUB_ALGORITHM_NO3	: C1028 MASK ETCHER
//	- A0_SUB_ALGORITHM_NO4	: C1012
//	- A0_SUB_ALGORITHM_NO5	: 2018.05.10
//	- A0_SUB_ALGORITHM_NO6	: 2018.06.18		:	PM2 1slot pick with B Arm
//-------------------------------------------------------------------------------


//===================================================================================================

enum {
	SWITCH_WILL_GO_READY ,
	SWITCH_WILL_GO_INS ,
	SWITCH_WILL_GO_RUN ,
	SWITCH_WILL_GO_OUTS ,
	SWITCH_WILL_GO_OUT
};



#endif

