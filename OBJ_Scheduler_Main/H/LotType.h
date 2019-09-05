#ifndef LOTTYPE_H
#define LOTTYPE_H

typedef struct {
	char	LOT_ID[255+1];
	char	CLUSTER_RECIPE[MAX_CASSETTE_SLOT_SIZE][64+1];
	char	CLUSTER_RECIPE2[MAX_CASSETTE_SLOT_SIZE][64+1];
	int		CLUSTER_USING[MAX_CASSETTE_SLOT_SIZE];
	int		CLUSTER_EXTRA_STATION;
	int		CLUSTER_EXTRA_TIME;
} LOTStepTemplate;

typedef struct {
	char	CLUSTER_GROUP[MAX_CASSETTE_SLOT_SIZE][64+1];
	int		CLUSTER_USING[MAX_CASSETTE_SLOT_SIZE];
} LOTStepTemplate1G;

typedef struct {
	char	CLUSTER_RECIPE[64+1];
	int		CLUSTER_USING;
} LOTStepTemplate2;

typedef struct {
	int		SPECIAL_INSIDE_DATA[MAX_CASSETTE_SLOT_SIZE];
	char	SPECIAL_USER_DATA[MAX_CASSETTE_SLOT_SIZE][1024+1]; // 2005.01.24
	//
	int		USER_CONTROL_MODE[MAX_CASSETTE_SLOT_SIZE]; // 2011.12.08
	char	USER_CONTROL_DATA[MAX_CASSETTE_SLOT_SIZE][256+1]; // 2011.12.08
	//
	int		USER_SLOTFIX_MODE[MAX_CASSETTE_SLOT_SIZE]; // 2015.10.12	// 0:X 1:Slot 2~:Dummy
	int		USER_SLOTFIX_SLOT[MAX_CASSETTE_SLOT_SIZE]; // 2015.10.12	// Slot
	//
} LOTStepTemplate3;

typedef struct {
	int		MODULE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
	char	MODULE_PR_RECIPE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH][64+1];
	char	MODULE_IN_RECIPE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH][64+1];
	char	MODULE_OUT_RECIPE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH][64+1];
	int		EXTRA_STATION;
	int		EXTRA_TIME;
	int		HANDLING_SIDE;
} CLUSTERStepTemplate;

typedef struct {
	int		MODULE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
	char	*MODULE_PR_RECIPE2[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
	char	*MODULE_IN_RECIPE2[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
	char	*MODULE_OUT_RECIPE2[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
	int		EXTRA_STATION;
	int		EXTRA_TIME;
	int		HANDLING_SIDE;
} CLUSTERStepTemplate2;

typedef struct {
	int		SPECIAL_INSIDE_DATA; // 2014.06.23
	//
	char	*SPECIAL_USER_DATA; // 2005.01.24 // 2007.06.01
	char	*RECIPE_TUNE_DATA; // 2007.06.01
	//
	int		USER_CONTROL_MODE; // 2011.12.08
	char	*USER_CONTROL_DATA; // 2011.12.08
	//
} CLUSTERStepTemplate3;

typedef struct {
	int		LOT_SPECIAL_DATA;
	char	*LOT_USER_DATA;
	char	*CLUSTER_USER_DATA;
	char	*RECIPE_TUNE_DATA;
} CLUSTERStepTemplate4;

typedef struct {
	int		CASSETTE_IN_ORDER[MAX_CASSETTE_SLOT_SIZE];
	int		CASSETTE_OUT_PATH[MAX_CASSETTE_SLOT_SIZE];
	int		CASSETTE_OUT_SLOT[MAX_CASSETTE_SLOT_SIZE];
} CASSETTETemplate;

typedef struct {
	int		CASSETTE1;
	int		CASSETTE2;
	int		CASSETTE1_STATUS[MAX_CASSETTE_SLOT_SIZE];
	int		CASSETTE2_STATUS[MAX_CASSETTE_SLOT_SIZE];
} CASSETTEVerTemplate;


typedef struct { // 2016.07.13
	char	CLUSTER_RECIPE[MAX_CASSETTE_SLOT_SIZE][64+1];
	int		CLUSTER_USING[MAX_CASSETTE_SLOT_SIZE];
	//
	//  0 : Not Use
	//
	//    : PreUse	PostUse	FixedCluster(Pre)	FixedCluster(Post)	PostBMOutFirst
	//
	//  0 : X		X		X					X					X
	//
	//  1 : X		O		X					X					O
	//  2 : X		O		O					X					O
	//  3 : X		O		X					O					O
	//  4 : X		O		O					O					O
	//  5 : X		O		X					X					X
	//  6 : X		O		O					X					X
	//  7 : X		O		X					O					X
	//  8 : X		O		O					O					X
	//  9 : O		X		X					X					X
	// 10 : O		X		O					X					X
	// 11 : O		X		X					O					X
	// 12 : O		X		O					O					X
	// 13 : O		O		X					X					X
	// 14 : O		O		O					X					X
	// 15 : O		O		X					O					X
	// 16 : O		O		O					O					X
	//
} LOTStepTemplate_Dummy_Ex;


#endif
