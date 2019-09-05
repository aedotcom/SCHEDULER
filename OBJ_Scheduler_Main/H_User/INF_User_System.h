#ifndef INF_USER_SYSTEM_H
#define INF_USER_SYSTEM_H



//===================================================================================================
// STACKSIZE
//
//	DEFAULT - 0x1000000
//	HALF    - 0x800000 
//
// Dfault Size
// 
//	MAX_CLUSTER_DEPTH - 50
//  MAX_PM_SLOT_DEPTH - 26
//
//===================================================================================================
#ifdef PM_CHAMBER_USER
//			//----------------------------------------------------------------
			// CUSTOMER_CODE	(2)				1019	MECA,GIGA,AP-BMP
			//----------------------------------------------------------------
//				#define MAX_PM_CHAMBER_DEPTH		30
				//
//				#define MAX_CASSETTE_SIDE			8
				//
				//----------------------------------------------------------------
				//----------------------------------------------------------------
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(9)						KINGSEMI(2)
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		100
				#define MAX_BM_CHAMBER_DEPTH		24
				#define MAX_TM_CHAMBER_DEPTH		8
				#define MAX_CASSETTE_SLOT_SIZE		50
				//
				#define MAX_CASSETTE_SIDE			8
				//
				#define MAX_CLUSTER_DEPTH			50
				//
				#define	MAX_PM_SLOT_DEPTH			2
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	//----------------------------------------------------------------
	//----------------------------------------------------------------
	//
	/*
	//----------------------------------------------------------------
	//		If you want to change then use this item
	//----------------------------------------------------------------
	//
	//- CUSTOMER BUILD INFO
	//
	   (1) 30_D150				:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(1)				1012	CA
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		30
				//
				#define MAX_CLUSTER_DEPTH			150
				//
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	   (2) 30_C8				:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(2)				1019	MECA,GIGA,AP-BMP
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		30
				//
				#define MAX_CASSETTE_SIDE			8
				//
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	   (3) 6_T2_B8_S200_C8_D4_L100	:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(3)				1027	KEK
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		6
				//
				#define MAX_BM_CHAMBER_DEPTH		8
				#define MAX_TM_CHAMBER_DEPTH		2
				#define MAX_CASSETTE_SLOT_SIZE		200
				//
				#define MAX_CASSETTE_SIDE			8
				//
				#define MAX_CLUSTER_DEPTH			4
				//
				#define	MAX_PM_SLOT_DEPTH			100
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	   (4) 100_B6_T6_S30_L2			:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(4)				1021	KS
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		100
				#define MAX_BM_CHAMBER_DEPTH		6
				#define MAX_TM_CHAMBER_DEPTH		6
				#define MAX_CASSETTE_SLOT_SIZE		30
				//
				#define	MAX_PM_SLOT_DEPTH			2
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	   (5) 30_C12				:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(5)						SEMES
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		30
				//
				#define MAX_CASSETTE_SIDE			12
				//
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	   (6) 30_S75				:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(6)						CA
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		30
				//
				#define MAX_CASSETTE_SLOT_SIZE		75
				//
				//----------------------------------------------------------------
				//----------------------------------------------------------------

	   (7) 99_B5_T2_D3_L2		:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(7)						AEHR
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		99
				#define MAX_BM_CHAMBER_DEPTH		5
				#define MAX_TM_CHAMBER_DEPTH		2
				//
				#define MAX_CLUSTER_DEPTH			3
				//
				#define	MAX_PM_SLOT_DEPTH			2
				//----------------------------------------------------------------
				//----------------------------------------------------------------

		(8) 100_C8_B12_T8_D50_L2		:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(8)						KINGSEMI
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		100
				#define MAX_BM_CHAMBER_DEPTH		12
				#define MAX_TM_CHAMBER_DEPTH		8
				#define MAX_CASSETTE_SLOT_SIZE		50
				//
				#define MAX_CASSETTE_SIDE			8
				//
				#define MAX_CLUSTER_DEPTH			50
				//
				#define	MAX_PM_SLOT_DEPTH			2
				//----------------------------------------------------------------
				//----------------------------------------------------------------

		(9) 100_C8_B12_T8_D50_L2		:
			//----------------------------------------------------------------
			// CUSTOMER_CODE	(9)						KINGSEMI(2)
			//----------------------------------------------------------------
				#define MAX_PM_CHAMBER_DEPTH		100
				#define MAX_BM_CHAMBER_DEPTH		24
				#define MAX_TM_CHAMBER_DEPTH		8
				#define MAX_CASSETTE_SLOT_SIZE		50
				//
				#define MAX_CASSETTE_SIDE			8
				//
				#define MAX_CLUSTER_DEPTH			50
				//
				#define	MAX_PM_SLOT_DEPTH			2
				//----------------------------------------------------------------
				//----------------------------------------------------------------
	//
	*/
	//----------------------------------------------------------------
	// Main Scheduler Excution File Name Rule :
	//----
	//		Sch_MFI{A}_B{B}_T{C}_S{D}_C{E}_D{F}_L{G}.exe
	//
	//	  ->  {A} , (Tag = N/A) is MAX_PM_CHAMBER_DEPTH	(default(not define) is 12)
	//	  ->  {B} , (Tag = 'B') is MAX_BM_CHAMBER_DEPTH	(default(not define) is	PM => 12 then 4)
	//																			PM => 30 then 10)
	//																			PM => 45 then 15)
	//	  ->  {C} , (Tag = 'T') is MAX_TM_CHAMBER_DEPTH	(default(not define) is	PM => 12 then 4)
	//																			PM => 30 then 6)
	//																			PM => 45 then 8)
	//	  ->  {D} , (Tag = 'S') is MAX_CASSETTE_SLOT_SIZE	(default(not define) is	PM => 12 then 30)
	//																				PM => 30 then 50)
	//																				PM => 45 then 40)
	//	  ->  {E} , (Tag = 'C') is MAX_CASSETTE_SIDE		(default(not define) is 4)
	//	  ->  {F} , (Tag = 'D') is MAX_CLUSTER_DEPTH		(default(not define) is 50)
	//	  ->  {G} , (Tag = 'L') is MAX_PM_SLOT_DEPTH		(default(not define) is 26)
	//----
	// (example)
	//
	//		In case of PM_CHAMBER_USER have to use below file.
	//		-> Sch_MFI6_B2_T2_S30_C4.exe
	//----------------------------------------------------------------
	//
#else
	#ifdef PM_CHAMBER_60
		//
		#define MAX_PM_CHAMBER_DEPTH		45
		//
	#else
		#ifdef PM_CHAMBER_30
			//
			#define MAX_PM_CHAMBER_DEPTH		30
			//
		#else
			//
			// Default
			//
		#endif
	#endif
#endif



//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//===================================================================================================
//
// DEFAULT AREA
//
//===============================================================================
#ifndef MAX_PM_CHAMBER_DEPTH
#define MAX_PM_CHAMBER_DEPTH						12
#endif
//===============================================================================
#ifndef MAX_BM_CHAMBER_DEPTH
	//
	#if		MAX_PM_CHAMBER_DEPTH == 30
			#define MAX_BM_CHAMBER_DEPTH			10
	#elif	MAX_PM_CHAMBER_DEPTH == 60
			#define MAX_BM_CHAMBER_DEPTH			15
	#else
			#define MAX_BM_CHAMBER_DEPTH			4
	#endif
	//
#endif
//===============================================================================
#ifndef MAX_TM_CHAMBER_DEPTH
	//
	#if		MAX_PM_CHAMBER_DEPTH == 30
			#define MAX_TM_CHAMBER_DEPTH			6
	#elif	MAX_PM_CHAMBER_DEPTH == 60
			#define MAX_TM_CHAMBER_DEPTH			8
	#else
			#define MAX_TM_CHAMBER_DEPTH			4
	#endif
	//
#endif
//===============================================================================
#ifndef MAX_CASSETTE_SLOT_SIZE
	//
	#if		MAX_PM_CHAMBER_DEPTH == 30
			#define MAX_CASSETTE_SLOT_SIZE			50
	#elif	MAX_PM_CHAMBER_DEPTH == 60
			#define MAX_CASSETTE_SLOT_SIZE			40
	#else
			#define MAX_CASSETTE_SLOT_SIZE			30
	#endif
	//
#endif
//===============================================================================
#ifndef MAX_CASSETTE_SIDE
#define MAX_CASSETTE_SIDE							4
#endif
//===============================================================================



#endif

