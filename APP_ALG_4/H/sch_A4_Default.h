#ifndef SCH_A4_DEFAULT_H
#define SCH_A4_DEFAULT_H

//===================================================================================================

typedef enum
{
	SDM4_PTYPE_LOTRUN ,
	SDM4_PTYPE_LOTFIRST ,
	SDM4_PTYPE_LOTEND ,
	SDM4_PTYPE_LOTFIRST0 ,
} CHECKORDER_FOR_MM;

//===================================================================================================
//#define	ONEBODY_BM_TIME_CHECK_COUNT	5000 // 2006.10.19
#define	ONEBODY_BM_TIME_CHECK_COUNT			10 // 2006.10.19
//#define	ONEBODY_BM_TIME_CHECK_COUNT_PRE		250 // 2006.10.26 // 2006.11.16
#define	ONEBODY_BM_TIME_CHECK_COUNT_PRE		500 // 2006.10.26 // 2006.11.16

//#define	ONEBODY_BM_TIME_CHECK_COUNT_OT_MAIN	500 // 2007.08.02
#define	ONEBODY_BM_TIME_CHECK_COUNT_OT_MAIN	3000 // 2008.05.08



#endif

