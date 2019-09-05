#ifndef VERSION_H
#define VERSION_H

//=======================================================================================================================================================
//=======================================================================================================================================================

#define	REGIST_INFO_STRING "Scheduler Program 6.2"	// See Upgrade List // 2017.02.02
//#define	REGIST_INFO_STRING "Scheduler Program 6.1"	// See Upgrade List // 2014.04.07
//#define	REGIST_INFO_STRING "Scheduler Program 6.0"	// See Upgrade List // 2009.02.01
//#define	REGIST_INFO_STRING "Scheduler Program 5.8"	// See Upgrade List

//=======================================================================================================================================================
//=======================================================================================================================================================
#define	SCH_LASTUPDATE_DATE "2018/10/01/0000"							// VS2008 이상 최적화 문제 -> 최적화 사용 안함
//#define	SCH_LASTUPDATE_DATE "2017/02/02/0000"						//
//#define	SCH_LASTUPDATE_DATE "2015/04/30/2000"						// TM Error Prcs Timer set
//#define	SCH_LASTUPDATE_DATE "2015/04/07/1200"						// All DLL Mode
//#define	SCH_LASTUPDATE_DATE "2014/07/17/1200"						// DualTM Job Stop bug update 
//#define	SCH_LASTUPDATE_DATE "2014/06/23/1200"						// MultiJob_Lot_Read (MFI) , Append ClusterData 
//#define	SCH_LASTUPDATE_DATE "2014/02/08/1200"						// RCP LOCKING Update 
//#define	SCH_LASTUPDATE_DATE "2013/10/22/1200"						// CJOB SELECT CHECK BUG UPDATE for STOP/PAUSE/RESUME/ABORT 
//#define	SCH_LASTUPDATE_DATE "2013/10/11/1200"						// /MT Rebuild
//#define	SCH_LASTUPDATE_DATE "2013/10/02/1200"						// Handoff
//#define	SCH_LASTUPDATE_DATE "2013/09/03/1200"						// MoveMode
//#define	SCH_LASTUPDATE_DATE "2013/08/22/1200"						// SchCfg
//#define	SCH_LASTUPDATE_DATE "2013/08/15/2000"						// AfterMap
//#define	SCH_LASTUPDATE_DATE "2013/08/01/2000"						// UserOption
//#define	SCH_LASTUPDATE_DATE "2013/07/11/1200"						// ReLoad
//#define	SCH_LASTUPDATE_DATE "2013/06/19/1800"						// Process End Log , Restart
//#define	SCH_LASTUPDATE_DATE "2013/06/04/1000"						// Step Not Auto Remapping
//#define	SCH_LASTUPDATE_DATE "2013/05/10/1000"						// JOB, STARTJ Recipe Tune Bug
//#define	SCH_LASTUPDATE_DATE "2013/04/01/1000"						// AL Anim Bug
//#define	SCH_LASTUPDATE_DATE "2013/03/18/1200"						// _sleep->Sleep
//#define	SCH_LASTUPDATE_DATE "2013/03/08/1200"						// MultiProcess with Dummy
//#define	SCH_LASTUPDATE_DATE "2013/02/27/1200"						// SET_EXTRA_FLAG
//#define	SCH_LASTUPDATE_DATE "2013/02/13/2000"						// Recipe Tuning and Delete Bug
//#define	SCH_LASTUPDATE_DATE "2013/01/07/1200"						// WAFER_REMOVE_SET CTC , Randimize+Disable
//#define	SCH_LASTUPDATE_DATE "2012/12/09/1200"						// ?
//#define	SCH_LASTUPDATE_DATE "2012/11/07/1800"						// MaintInf(D)
//#define	SCH_LASTUPDATE_DATE "2012/09/17/1200"						// EIL FM
//#define	SCH_LASTUPDATE_DATE "2012/08/23/1800"						// PICK_ORDER , Driver Folder , SET_PROCESSING
//#define	SCH_LASTUPDATE_DATE "2012/07/20/1200"						// BM SPAWN Bug Update
//#define	SCH_LASTUPDATE_DATE "2012/05/04/1800"						// MaintInf DB
//#define	SCH_LASTUPDATE_DATE "2012/04/27/1400"						// EIL Auto
//#define	SCH_LASTUPDATE_DATE "2012/04/23/1200"						// Lot Pre Fail , Abort Bug Update
//#define	SCH_LASTUPDATE_DATE "2012/04/18/1200"						// DisableSkip Append
//#define	SCH_LASTUPDATE_DATE "2012/04/16/1200"						// Cycle Update
//#define	SCH_LASTUPDATE_DATE "2012/04/04/1200"						// EIL Update
//#define	SCH_LASTUPDATE_DATE "2012/03/30/1200"						// PRE_APPEND JOB update
//#define	SCH_LASTUPDATE_DATE "2012/03/27/1200"						// SET_FORCE_PRE_?? update
//#define	SCH_LASTUPDATE_DATE "2012/03/16/1800"						// EIL Multi INSERT/REMOVE
//#define	SCH_LASTUPDATE_DATE "2012/03/10/2200"						// GUI Pos Auto
//#define	SCH_LASTUPDATE_DATE "2012/02/07/1400"						// WAFER_RESTORE_SET Append
//#define	SCH_LASTUPDATE_DATE "2012/02/02/0000"						// Single_Chamber Disable Bug Update
//#define	SCH_LASTUPDATE_DATE "2012/01/29/1200"						// Mtlout Spawn Bug
//#define	SCH_LASTUPDATE_DATE "2012/01/21/1200"						// Maint_Interface DB Bug
//#define	SCH_LASTUPDATE_DATE "2012/01/20/1200"						// All Pause,Maint_Interface DB Bug
//#define	SCH_LASTUPDATE_DATE "2012/01/17/1800"						// Concurrent(Success Skip) Bug
//#define	SCH_LASTUPDATE_DATE "2012/01/10/1200"						// All Pause
//#define	SCH_LASTUPDATE_DATE "2011/12/13/1200"						// Restart Update
//#define	SCH_LASTUPDATE_DATE "2011/12/12/1200"						// User Interface DLL
//#define	SCH_LASTUPDATE_DATE "2011/12/06/1200"						// Comm Check PM Multi Slot
//#define	SCH_LASTUPDATE_DATE "2011/11/30/1200"						// MFIC
//#define	SCH_LASTUPDATE_DATE "2011/11/18/1200"						// Comm Check PM Multi Slot , extend Option dll , pre recipe read (64) bug
//#define	SCH_LASTUPDATE_DATE "2011/11/16/1200"						// DEFAULT_RECIPE_CHANGE
//#define	SCH_LASTUPDATE_DATE "2011/11/14/1200"						// MultiRecipe 4096
//#define	SCH_LASTUPDATE_DATE "2011/09/22/1800"						// Job Recipe Locking , MFIC -17,-18 Update
//#define	SCH_LASTUPDATE_DATE "2011/09/08/1200"						// Disable Bug Update
//#define	SCH_LASTUPDATE_DATE "2011/09/01/1200"						// Prcs Start PPID IO
//#define	SCH_LASTUPDATE_DATE "2011/08/25/1200"						// EIL Cassette Result Update
//#define	SCH_LASTUPDATE_DATE "2011/08/24/1200"						// MR
//#define	SCH_LASTUPDATE_DATE "2011/07/22/1200"						// LotPre Wait Bug
//#define	SCH_LASTUPDATE_DATE "2011/05/30/1600"						// Sequential Dummy Check Skip
//#define	SCH_LASTUPDATE_DATE "2011/05/20/1600"						// Sequential Lot Pre Bug
//#define	SCH_LASTUPDATE_DATE "2011/04/29/1200"						// Randomize Update
//#define	SCH_LASTUPDATE_DATE "2011/04/21/1200"						// FINISH Event
//#define	SCH_LASTUPDATE_DATE "2011/04/19/1800"						// MR
//#define	SCH_LASTUPDATE_DATE "2011/04/15/1800"						// EIL Manual
//#define	SCH_LASTUPDATE_DATE "2011/04/12/1400"						// ControlJob Delete
//#define	SCH_LASTUPDATE_DATE "2011/04/05/1800"						// SYS_ERROR Anim
//#define	SCH_LASTUPDATE_DATE "2011/02/21/1200"						// EIL Stop Append
//#define	SCH_LASTUPDATE_DATE "2011/01/06/1200"						// IO Name Search Optimize
//#define	SCH_LASTUPDATE_DATE "2010/12/22/1600"						// Max Slot Interface , FIC to FAL
//#define	SCH_LASTUPDATE_DATE "2010/12/17/1200"						// Max Slot Interface , LotPre Append
//#define	SCH_LASTUPDATE_DATE "2010/12/08/1200"						// _SCH_COMMON_RECIPE_ANALYSIS_BM_USEFLAG_SETTING Update
//#define	SCH_LASTUPDATE_DATE "2010/12/02/1200"						// Abnormal Stop Delay 2000
//#define	SCH_LASTUPDATE_DATE "2010/11/30/1200"						// Dummy message
//#define	SCH_LASTUPDATE_DATE "2010/11/26/1200"						// EIL Log
//#define	SCH_LASTUPDATE_DATE "2010/11/23/1200"						// Append _SCH_COMMON_ANIMATION_SOURCE_UPDATE
//#define	SCH_LASTUPDATE_DATE "2010/11/16/1600"						// LOT_LOG_MODE
//#define	SCH_LASTUPDATE_DATE "2010/11/16/1200"						// _SCH_CASSETTE_Reset_Side_Monitor Append Action , _SCH_SYSTEM_PMMODE_xET
//#define	SCH_LASTUPDATE_DATE "2010/11/11/1200"						// _SCH_CASSETTE_Reset_Side_Monitor Append Action , _SCH_SYSTEM_PMMODE_xET
//#define	SCH_LASTUPDATE_DATE "2010/11/09/1200"						// Append _SCH_COMMON_USER_DATABASE_REMAPPING
//#define	SCH_LASTUPDATE_DATE "2010/11/03/1200"						// Diff Check for Dummy Part Update
//#define	SCH_LASTUPDATE_DATE "2010/10/29/1200"						// Differ Cassette CASSETTE_ABSENT_RUN_DATA
//#define	SCH_LASTUPDATE_DATE "2010/10/26/1200"						// Recipe Locking Multi Copy
//#define	SCH_LASTUPDATE_DATE "2010/10/25/1600"						// EIL
//#define	SCH_LASTUPDATE_DATE "2010/10/18/1400"						// EIL
//#define	SCH_LASTUPDATE_DATE "2010/10/08/1200"						// MAINT
//#define	SCH_LASTUPDATE_DATE "2010/10/06/1400"						// MAINT
//#define	SCH_LASTUPDATE_DATE "2010/09/28/1600"						// Recipe Locking Delete 2
//#define	SCH_LASTUPDATE_DATE "2010/09/15/1200"						// EIL Interface
//#define	SCH_LASTUPDATE_DATE "2010/09/09/1200"						// Recipe Locking Delete
//#define	SCH_LASTUPDATE_DATE "2010/09/01/1800"						// Append Cluster Index
//#define	SCH_LASTUPDATE_DATE "2010/08/06/2000"						// Append Cluster Index
//#define	SCH_LASTUPDATE_DATE "2010/08/05/1400"						// TryEnter
//#define	SCH_LASTUPDATE_DATE "2010/07/09/1800"						// BM Arm Interlock Separate for FM/TM
//#define	SCH_LASTUPDATE_DATE "2010/05/27/1800"						// Parallel/BeginCheck(2)
//#define	SCH_LASTUPDATE_DATE "2010/05/09/2000"						// Dummy Wafer Result Set
//#define	SCH_LASTUPDATE_DATE "2010/04/23/1800"						// Parallel/BeginCheck
//#define	SCH_LASTUPDATE_DATE "2010/04/12/1200"						// Parallel
//#define	SCH_LASTUPDATE_DATE "2010/04/15/1600"						// Job GUI
//#define	SCH_LASTUPDATE_DATE "2010/04/10/1200"						// 1 Count Dummy Bug
//#define	SCH_LASTUPDATE_DATE "2010/04/10/0000"						// Parallel
//#define	SCH_LASTUPDATE_DATE "2010/04/02/1200"						// BM Start
//#define	SCH_LASTUPDATE_DATE "2010/03/22/2000"						// BM Start
//#define	SCH_LASTUPDATE_DATE "2010/03/19/0000"						// BM Start
//#define	SCH_LASTUPDATE_DATE "2010/03/14/1200"						// BM Start
//#define	SCH_LASTUPDATE_DATE "2010/03/08/1200"						// CASS INFO
//#define	SCH_LASTUPDATE_DATE "2010/03/06/1200"						// SET_PROCESSING
//#define	SCH_LASTUPDATE_DATE "2010/03/01/0000"						// ONE Select Buffering
//#define	SCH_LASTUPDATE_DATE "2010/02/22/0000"						// BM Start
//#define	SCH_LASTUPDATE_DATE "2010/02/01/1200"						// Lot Post ...
//#define	SCH_LASTUPDATE_DATE "2009/10/30/1200"						// Recipe Locking Bug Update
//#define	SCH_LASTUPDATE_DATE "2009/10/15/0000"						// PM Mode Randomize Bug Update
//#define	SCH_LASTUPDATE_DATE "2009/10/01/1000"						// PM Mode Randomize Bug Update
//#define	SCH_LASTUPDATE_DATE "2009/09/23/1000"						// FM/TM Flow Log Part Append for Dummy
//#define	SCH_LASTUPDATE_DATE "2009/08/21/1600"						// Process Part Status Bug Update
//#define	SCH_LASTUPDATE_DATE "2009/07/16/1600"						// Animation Commom Interface Append
//#define	SCH_LASTUPDATE_DATE "2009/06/23/1600"						// Enable/Disable Critical Section Append
//#define	SCH_LASTUPDATE_DATE "2009/06/09/1200"						// Job Check Bug Update
//#define	SCH_LASTUPDATE_DATE "2009/05/06/1200"						// FM Multi and recipe check update
//#define	SCH_LASTUPDATE_DATE "2009/04/03/1800"						// Job Multi CM Use
//#define	SCH_LASTUPDATE_DATE "2009/03/27/0000"						// FM - TM Critical section deadlock
//#define	SCH_LASTUPDATE_DATE "2009/03/20/0000"						// Flow Control Log Update
//#define	SCH_LASTUPDATE_DATE "2009/03/10/0000"						// Flow Control Log Update
//#define	SCH_LASTUPDATE_DATE "2009/03/03/2000"						// Sub System Bug Update
//#define	SCH_LASTUPDATE_DATE "2009/02/16/1600"						// Flow Control Update
//#define	SCH_LASTUPDATE_DATE "2009/02/12/1800"						// Batch Part Randomize Update
//#define	SCH_LASTUPDATE_DATE "2009/02/11/0000"						// Flow Control Update
//#define	SCH_LASTUPDATE_DATE "2009/02/02/1000"						// Flow Control Update
//#define	SCH_LASTUPDATE_DATE "2008/12/10/1800"						// TM Part Sys_Error Part Expand
//#define	SCH_LASTUPDATE_DATE "2008/10/17/1600"						// Optimize
//#define	SCH_LASTUPDATE_DATE "2008/10/02/1200"						// FM Robot Animation Bug Update
//#define	SCH_LASTUPDATE_DATE "2008/09/24/1800"						// Cassette Mapping Part update
//#define	SCH_LASTUPDATE_DATE "2008/08/27/1800"						// Cassette Supply Part update
//#define	SCH_LASTUPDATE_DATE "2008/07/30/1700"						// StartJ MID Probelm
//#define	SCH_LASTUPDATE_DATE "2008/07/29/1200"						// Thread FM-TM Deadlock in Endpart
//#define	SCH_LASTUPDATE_DATE "2008/07/24/1200"						// split check in Randomize
//#define	SCH_LASTUPDATE_DATE "2008/07/16/1200"						// CHECK_DIFFERENT_METHOD
//#define	SCH_LASTUPDATE_DATE "2008/07/15/1400"









//=======================================================================================================================================================
//=======================================================================================================================================================
//=======================================================================================================================================================
//=======================================================================================================================================================
//=======================================================================================================================================================
//=======================================================================================================================================================
//#define	REGIST_INFO_STRING "\vScheduler Program 5.6(2007.12.31)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.56(2006.09.19)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.56(2006.08.31)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.55(2006.08.01)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.55(2006.07.11)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.55(2006.06.23)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.55(2006.06.22)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.55(2006.06.14)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.55(2006.06.09)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.54(2006.04.13)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.53(2005.12.21)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.52(2005.12.13)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.51(2005.11.15)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.5(2005.10.26)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.4(2005.10.11)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.31(2005.08.30)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.3(2005.07.07)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.2(2005.03.21)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.11(2005.02.28)"	// CPU OverHead Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 5.1(2005.02.23)"	// See Upgrade List
//#define	REGIST_INFO_STRING "\vScheduler Program 5.07(2004.12.23)"	// Dual Arm Bug Modify for Algorithm 4
//#define	REGIST_INFO_STRING "\vScheduler Program 5.06(2004.12.14)"	// Recipe Info Dll API Append , Append Message File interface
//#define	REGIST_INFO_STRING "\vScheduler Program 5.05(2004.11.08)"	// Different Cluster Recipe Wafer Lock Bug Update for Single Swap
//#define	REGIST_INFO_STRING "\vScheduler Program 5.04(2004.10.12)"	// Cassette Place Switch Message Part Append
																			// Separate Option Append for Algorithm 7
//#define	REGIST_INFO_STRING "\vScheduler Program 5.03(2004.10.08)"	// Duplicate Process Problem Part Update with Event Type
//#define	REGIST_INFO_STRING "\vScheduler Program 5.02(2004.09.24)"	// Separate End Clean Part Modify for Algorithm 4
//#define	REGIST_INFO_STRING "\vScheduler Program 5.01(2004.09.08)"	// Concurrent AL Operation Append
//#define	REGIST_INFO_STRING "\vScheduler Program 5.00(2004.09.02)"	// Swapping Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.99(2004.08.19)"	// Transfer Interlock Check Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.98(2004.08.18)"	// Thread Spawn Check part Append
																			// Control not idle run bug update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.97(2004.08.10)"	// Window Instance Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.96(2004.06.29)"	// CJ/PJ Part Bug Update
																			// Append Global Pause/Resume Status and IO
//#define	REGIST_INFO_STRING "\vScheduler Program 4.95(2004.06.16)"	// CJ/PJ Part Bug Update
																			// Transfer part Bug Update
																			// Multiple Cassette Mapping Bug Update
																			// Append FA_REJECTMESSAGE_DISPLAY Option
																			// Append MESSAGE_USE_WHEN_ORDER Option
//#define	REGIST_INFO_STRING "\vScheduler Program 4.94(2004.06.02)"	// CJ/PJ Part Bug Update(STOP,ABORT)
																			// Default Recipe Set Part Expand
//#define	REGIST_INFO_STRING "\vScheduler Program 4.93(2004.05.11)"	// Swapping Bug Update
																			// CJ/PJ Part Bug Update
																			// Append CJ/PJ Style Option
																			// Append ClearOut BM Single Only Option
																			// Append Log Part for Pre/Post Process
//#define	REGIST_INFO_STRING "\vScheduler Program 4.922(2004.04.27)"	// Process Log Part Append for Module Side Log
																			// Multiple Cassette Mapping Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.921(2004.04.09)"	// Default Run Mode Change when CJ Run(Concurrent->Continue)
//#define	REGIST_INFO_STRING "\vScheduler Program 4.92(2004.03.25)"	// Swapping Stop Bug Update
																			// Abort_But_Remain_Check part Modify
																			// Log Append for Control Button
																			// Append Scheduler System IO Status
//#define	REGIST_INFO_STRING "\vScheduler Program 4.91(2004.03.10)"	// Single Swapping Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.90(2004.02.27)"	// Process Log Part Update for Multi PM
//#define	REGIST_INFO_STRING "\vScheduler Program 4.89(2004.02.24)"	// Algorithm 7 Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.88(2004.02.14)"	// Main THD Check Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.87(2004.02.10)"	// Algorithm 6 Update for Multi option Bug
																			// Dual TM Transfer Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.86(2004.01.06)"	// Algorithm 6 Update for Multi option at BM Pick
//#define	REGIST_INFO_STRING "\vScheduler Program 4.85(2003.12.11)"	// Disable Part Ready,End Update
																			// Algorithm 7 Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.84(2003.11.10)"	// Switch Part Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.83(2003.10.28)"	// Disable Part Ready,End Update
																			// Algorithm 7 Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.82(2003.10.21)"	// Transfer Bug Update (Arm Interlock)
																			// Disable Part Update
																			// Algorithm 7 Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.81(2003.10.17)"	// Transfer Bug Update (Arm Interlock)
																			// Disable Part Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.8(2003.10.09)"	// Cluster Include when Disable append & Disable Place Bug Update
																			// Algorithm 7 Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.75(2003.09.24)"	// Multijob can read lot recipe
//#define	REGIST_INFO_STRING "\vScheduler Program 4.74(2003.09.15)"	// BM Only First Ordering Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.73(2003.09.09)"	// BM Abort Check Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.72(2003.09.03)"	// Transfer Bug Update , Append User Select CommStatus IO Name
//#define	REGIST_INFO_STRING "\vScheduler Program 4.71(2003.08.23)"	// Algorithm 4 Dummy Bug update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.7(2003.08.12)"	// Disable Part Update(GUI,Event,Fail Scenario)
//#define	REGIST_INFO_STRING "\vScheduler Program 4.69(2003.08.08)"	// Disable Part Update , Module Status Part Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.68(2003.07.31)"	// Arm , Chamber Interlock Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.67(2003.07.23)"	// CJ/PJ Interface Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.66(2003.07.12)"	// Append Wait Receive Spawn Message
//#define	REGIST_INFO_STRING "\vScheduler Program 4.65(2003.07.07)"	// Sequential Mode Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.64(2003.07.04)"	// Module CommStatus Append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.63(2003.06.25)"	// Algorithm 4 Dummy Bug update & Transfer Possible Option Append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.62(2003.06.10)"	// Algorithm 6 Bug & Multi Cassette Supply update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.61(2003.06.02)"	// BM Ordering Get/Put Append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.6(2003.05.29)"	// Algorithm 6 Bug update
																			// Multi Cassette Sequential supply bug update
																			// Chamber Disappear Upload Message Append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.59(2003.05.28)"	// Algorithm 6 Bug update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.58(2003.05.26)"	// Enable Possible after disable when run
//#define	REGIST_INFO_STRING "\vScheduler Program 4.57(2003.05.21)"	// Arm , Chamber Interlock Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.56(2003.05.08)"	// Algorithm 6 Transfer update for Arm B2
//#define	REGIST_INFO_STRING "\vScheduler Program 4.55(2003.04.30)"	// Algorithm 6 Transfer update for Arm B2
//#define	REGIST_INFO_STRING "\vScheduler Program 4.54(2003.04.28)"	// Algorithm 6 Transfer Bug update
//																			// Multiple lot Single Dummy Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.53(2003.04.23)"	// Cassette Sequence supply append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.52(2003.04.21)"	// Algorithm 6 Transfer Bug update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.51(2003.03.21)"	// Control Job Start message Bug update
//#define	REGIST_INFO_STRING "\vScheduler Program 4.5(2003.02.12)"	// Multi Lot Dummy Access Append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.4(2003.02.05)"	// Double TM Slot Bug Update & TM Finger Interlock Append
//#define	REGIST_INFO_STRING "\vScheduler Program 4.32(2003.01.21)"	// Pre End & Lot End Synch bug update at multi run
//#define	REGIST_INFO_STRING "\vScheduler Program 4.31(2003.01.03)"	// Trouble TM 1 slot use bug Update at Host
//#define	REGIST_INFO_STRING "\vScheduler Program 4.3(2002.12.30)"	// Append Gui(Event) for CPJob Log Type , Sequentiel2
//#define	REGIST_INFO_STRING "\vScheduler Program 4.2(2002.12.19)"	// Update for Parallel CJ
//#define	REGIST_INFO_STRING "\vScheduler Program 4.1(2002.11.15)"	// Update First ordering for Multi TM 
//#define	REGIST_INFO_STRING "\vScheduler Program 4.03(2002.11.12)"	// Change order between PRJOB End Message and PRJOB log finish
																			// Update First ordering for Multi TM
//#define	REGIST_INFO_STRING "\vScheduler Program 4.02(2002.11.05)"	// TM Double Differ Recipe Type Set Bug Update
																			// Update First ordering for Multi TM
//#define	REGIST_INFO_STRING "\vScheduler Program 4.01(2002.10.11)"	// Append Multi Align
//#define	REGIST_INFO_STRING "\vScheduler Program 4.0(2002.10.05)"	// Append Default Recipe Change Event
																			// Append Cassette Supply Not Delay Option
																			// Algorithm 5 Append
//#define	REGIST_INFO_STRING "\vScheduler Program 3.99(2002.09.26)"	// Post Process Move Pick Deny Append
																			// Job Possible select Group
																			// Algorithm 4 Append
//#define	REGIST_INFO_STRING "\vScheduler Program 3.98(2002.08.27)"	// Post or Already Process Status Check Bug Update
																			// BM Slot Offset & BM Size Modify Message Append
//#define	REGIST_INFO_STRING "\vScheduler Program 3.97(2002.08.10)"	// Sdummy Differ Cassette Flow Check Bug Update
//#define	REGIST_INFO_STRING "\vScheduler Program 3.96(2002.07.30)"	// TM Double Differ Recipe Type Set Modify
																			// POST Run Recipe already check not run option append
//#define	REGIST_INFO_STRING "\vScheduler Program 3.95(2002.07.18)"	// Source Cassette Indexing bug update
//#define	REGIST_INFO_STRING "\vScheduler Program 3.94(2002.07.16)"	// SDummy Check time update for always
																			// First ordering deadlock bug update
																			// PROCESS Message insert for next pm
																			// BM Standalone Process
//#define	REGIST_INFO_STRING "\vScheduler Program 3.93(2002.07.10)"	// Error Out Cassette Setup
																			// SDummy Reset & stop seperate
																			// Pre Part Running Check for Impossible pick cm
																			// Dual Lot Finish Check delay remove
//#define	REGIST_INFO_STRING "\vScheduler Program 3.92(2002.06.28)"	// Order change for PRJOB START Messgae(06.24)
																			// Make Impossible pick from FM When SDummy Process Pick or Run(06.24)
																			// Make Default Log based log when Process Job but append option for select
																			// TM?.READY Message Bug Update
																			// Post Part Running Check for Impossible pick cm
//#define	REGIST_INFO_STRING "\vScheduler Program 3.91(2002.06.21)"	// First Ordering BM Move Skip Check Append
//#define	REGIST_INFO_STRING "\vScheduler Program 3.9(2002.06.17)"	// Wafer Remove Bug Update at FM Place Part
																			// When Dummy use , Fixed Cluster Adapt Option Append
																			// Window Instance Update
//#define	REGIST_INFO_STRING "\vScheduler Program 3.8(2002.06.01)"	// Single Dummy Library Interface Append
																			// First Ordering Option Append
																			// Post Process Check Bug Fixed
//#define	REGIST_INFO_STRING "\vScheduler Program 3.7(2002.05.25)"	// File Name can include group
																			// AGV Control Change for Always
																			// All Recipe Type Check connect DLL Interface
																			// WaferClear Upgrade for Distribute check
																			// Post Process Schedule Optimize
																			// PRJOB Data Log Message Part Append
																			// Single Dummy Operation Part Append
//#define	REGIST_INFO_STRING "Scheduler Program 3.61(2002.05.05)"	// Scheduler.cfg Save Part Update
//																			// MoveSend Check Part Update
//#define	REGIST_INFO_STRING "Scheduler Program 3.6(2002.04.27)"		// PM Pick Condition when ID Interlock Check
//#define	REGIST_INFO_STRING "Scheduler Program 3.53(2002.04.16)"	// PM Pick Condition when Next PM Free
//#define	REGIST_INFO_STRING "Scheduler Program 3.52(2002.04.10)"	// Post End Message Removed when abort
//#define	REGIST_INFO_STRING "Scheduler Program 3.51(2002.04.08)"	// Pre/Post Estimate time seperate check append
//																			// Fast Finger First
//																			// Fast PM Ready for Place
//#define	REGIST_INFO_STRING "Scheduler Program 3.5(2002.03.29)"		// BM Process , Wafer IO Read with Multiple
//#define	REGIST_INFO_STRING "Scheduler Program 3.41(2002.03.27)"	// Mapping IO Read part update for more fast at network Interface
//#define	REGIST_INFO_STRING "Scheduler Program 3.4(2002.03.26)"		// Pre/Post Process Skip Append
//																			// SWITCH2 Append
//#define	REGIST_INFO_STRING "Scheduler Program 3.3(2002.03.25)"		// Begin/End Part Waiting overhead update
//																			// Datalog Send Message Append
//																			// External Wafer Result check part append
//#define	REGIST_INFO_STRING "Scheduler Program 3.21(2002.03.20)"	// Control/Process JOB Bug Fixed
//#define	REGIST_INFO_STRING "Scheduler Program 3.2(2002.03.11)"		// Upgrade Disappear Wafer
//#define	REGIST_INFO_STRING "Scheduler Program 3.16(2002.03.06)"	// Chamber Interlock Check when Auto bug fixed
//#define	REGIST_INFO_STRING "Scheduler Program 3.15(2002.02.28)"	// Chamber Interlock Check when transfer bug fixed
//#define	REGIST_INFO_STRING "Scheduler Program 3.14(2002.02.27)"	// Module Info Message Bug Fixed
//#define	REGIST_INFO_STRING "Scheduler Program 3.13(2002.02.19)"	// Pre Recipe run with asynch
//																			// External Interface Animation Bug Fixed
//#define	REGIST_INFO_STRING "Scheduler Program 3.12(2002.02.16)"	// update to AL/IC Manual Transfer can possible
//#define	REGIST_INFO_STRING "Scheduler Program 3.11(2002.02.05)"	// Auto Handler Check Message when Scheduler Loading
//#define	REGIST_INFO_STRING "Scheduler Program 3.1(2002.02.04)"		// Carrier Clear part bug fixed
//#define	REGIST_INFO_STRING "Scheduler Program 3.03(2002.01.24)"	// AL/IC IO check change to dynamic
//#define	REGIST_INFO_STRING "Scheduler Program 3.02(2002.01.17)"	// Flow data set with 17 when Information check error
//#define	REGIST_INFO_STRING "Scheduler Program 3.01(2002.01.05)"	// CimSqCon Change Rebuild
//#define	REGIST_INFO_STRING "Scheduler Program 3.0(2002.01.03)"		// Modify Pick BM Check Bug
//#define	REGIST_INFO_STRING "Scheduler Program(2001.12.26)"			// Bug Fix Single mode stop error
//#define	REGIST_INFO_STRING "Scheduler Program(2001.12.17)"			// Check Load/unload/map
//#define	REGIST_INFO_STRING "Scheduler Program(2001.12.04)"			// Upgrade Read
//#define	REGIST_INFO_STRING "Scheduler Program(2001.10.15)"			// Upgrade Expand TM
//#define	REGIST_INFO_STRING "Scheduler Program(2001.09.17)"			// Upgrade Disappear Wafer
//#define	REGIST_INFO_STRING "Scheduler Program(2001.09.13)"			// Upgrade Animation+Switch Priority
//#define	REGIST_INFO_STRING "Scheduler Program(2001.09.05)"			// Upgrade Animation
//#define	REGIST_INFO_STRING "Scheduler Program(2001.08.30)"			// Upgrade Maint Transfer Interface
//#define	REGIST_INFO_STRING "Scheduler Program(2001.08.23)"			// Update Transfer(Double)
//#define	REGIST_INFO_STRING "Scheduler Program(2001.08.22)"			// Update Robot Anim
//#define	REGIST_INFO_STRING "Scheduler Program(2001.08.09)"			// Update Dummy + Alarm Log
//#define	REGIST_INFO_STRING "Scheduler Program(2001.08.03)"			// Append Dummy
//#define	REGIST_INFO_STRING "Scheduler Program(2001.07.29)"			// upto PM60
//#define	REGIST_INFO_STRING "Scheduler Program(2001.06.25)"


#endif

