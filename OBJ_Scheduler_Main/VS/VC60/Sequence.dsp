# Microsoft Developer Studio Project File - Name="Sequence" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Sequence - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Sequence.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sequence.mak" CFG="Sequence - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sequence - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release2" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release3" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release_M" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release2_M" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release3_M" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Debug2" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Debug3" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Debug_M" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Debug2_M" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Debug3_M" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 ReleaseU" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 ReleaseU_M" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Sequence - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_12" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimSqCon.lib CimSqConIO.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_Main.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map /debug

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release2"
# PROP BASE Intermediate_Dir "Release2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_Main30.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release3"
# PROP BASE Intermediate_Dir "Release3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_Main60.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_M"
# PROP BASE Intermediate_Dir "Release_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_12" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_12" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_MFI.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release2_M"
# PROP BASE Intermediate_Dir "Release2_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_MFI30.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release3_M"
# PROP BASE Intermediate_Dir "Release3_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_MFI60.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "PM_CHAMBER_12" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_12" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT BASE LINK32 /map /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"Debug\Sch_Main(D).exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug2"
# PROP BASE Intermediate_Dir "Debug2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "App\h" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT BASE LINK32 /map /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"Debug\Sch_Main30(D).exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug3"
# PROP BASE Intermediate_Dir "Debug3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "App\h" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT BASE LINK32 /map /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"Debug\Sch_Main60(D).exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_M"
# PROP BASE Intermediate_Dir "Debug_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_12" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_12" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT BASE LINK32 /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"Debug\Sch_MFI(D).exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug2_M"
# PROP BASE Intermediate_Dir "Debug2_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT BASE LINK32 /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"Debug\Sch_MFI30(D).exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug3_M"
# PROP BASE Intermediate_Dir "Debug3_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /ZI /Od /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_60" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT BASE LINK32 /map
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"Debug\Sch_MFI60(D).exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Sequence___Win32_ReleaseU"
# PROP BASE Intermediate_Dir "Sequence___Win32_ReleaseU"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_USER" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimSqCon.lib CimSqConIO.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_Main30.exe" /libpath:"k:\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"Release\Sch_Mainxx_Bxx_Txx_Sxx_Cxx_Dxx_Lxx.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Sequence___Win32_ReleaseU_M"
# PROP BASE Intermediate_Dir "Sequence___Win32_ReleaseU_M"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_30" /D "_SCH_MIF" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /I "." /I "h" /I "k:\h" /I "k:\h\schinf" /I "h_User" /I "..\App_Alg_0\h" /I "..\App_Alg_4\h" /I "..\App_Alg_6\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_SCH_MAIN" /D "PM_CHAMBER_USER" /D "_SCH_MIF" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimSqCon.lib CimSqConIO.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\scheduler\Sch_MFI30.exe" /libpath:"k:\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimsqconDll.lib CimsqconDllMain.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"Release\Sch_MFIxx_Bxx_Txx_Sxx_Cxx_Dxx_Lxx.exe" /libpath:"k:\lib"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "Sequence - Win32 Release"
# Name "Sequence - Win32 Release2"
# Name "Sequence - Win32 Release3"
# Name "Sequence - Win32 Release_M"
# Name "Sequence - Win32 Release2_M"
# Name "Sequence - Win32 Release3_M"
# Name "Sequence - Win32 Debug"
# Name "Sequence - Win32 Debug2"
# Name "Sequence - Win32 Debug3"
# Name "Sequence - Win32 Debug_M"
# Name "Sequence - Win32 Debug2_M"
# Name "Sequence - Win32 Debug3_M"
# Name "Sequence - Win32 ReleaseU"
# Name "Sequence - Win32 ReleaseU_M"
# Begin Group "Kornic Library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=K:\lib\kutlAnal.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlEtc.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlStr.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlTime.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlgui.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlgui2.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\SchJobIntf.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\SchCfgIntf.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\FILEtoSMIntf.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlfile2.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Group "FA (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\FA_Handling.h
# End Source File
# End Group
# Begin Group "GUI (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\Gui_Handling.h
# End Source File
# End Group
# Begin Group "DLL (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\DLL_Interface.h
# End Source File
# End Group
# Begin Group "EQUIP_IO (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\Equip_Handling.h
# End Source File
# Begin Source File

SOURCE=.\H\IO_Address.h
# End Source File
# Begin Source File

SOURCE=.\H\IO_Part_data.h
# End Source File
# Begin Source File

SOURCE=.\H\IO_Part_Log.h
# End Source File
# Begin Source File

SOURCE=.\H\iolog.h
# End Source File
# End Group
# Begin Group "RECIPE (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\RcpFile_Handling.h
# End Source File
# End Group
# Begin Group "ETC (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\Alarm_Analysis.h
# End Source File
# Begin Source File

SOURCE=.\H\File_ReadInit.h
# End Source File
# Begin Source File

SOURCE=.\H\System_Tag.h
# End Source File
# Begin Source File

SOURCE=.\H\Timer_Handling.h
# End Source File
# Begin Source File

SOURCE=.\H\User_Parameter.h
# End Source File
# Begin Source File

SOURCE=.\H\Utility.h
# End Source File
# End Group
# Begin Group "ROBOT (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\Robot_Animation.h
# End Source File
# Begin Source File

SOURCE=.\H\Robot_Handling.h
# End Source File
# End Group
# Begin Group "SCHEDULING (H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\LotType.h
# End Source File
# Begin Source File

SOURCE=.\H\MR_Data.h
# End Source File
# Begin Source File

SOURCE=.\H\MultiJob.h
# End Source File
# Begin Source File

SOURCE=.\H\MultiReg.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_CassetteResult.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_CassetteSupply.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_cassmap.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_CommonUser.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_EIL.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_estimate.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_FMArm_Multi.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_HandOff.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_Interlock.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_main.h
# End Source File
# Begin Source File

SOURCE=.\H\Sch_Multi_ALIC.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_OneSelect.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_prepost.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_PrePrcs.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_prm.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_prm_Cluster.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_prm_FBTPM.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_prm_Log.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_ProcessMonitor.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_ReStart.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_sdm.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_sub.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_Transfer.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_TransferMaintInf.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_TransferUser.h
# End Source File
# Begin Source File

SOURCE=.\H\sch_Util.h
# End Source File
# End Group
# Begin Group "MAIN(H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H\default.h
# End Source File
# Begin Source File

SOURCE=.\H\EQ_Enum.h
# End Source File
# Begin Source File

SOURCE=.\H\Errorcode.h
# End Source File
# Begin Source File

SOURCE=.\H\Version.h
# End Source File
# End Group
# Begin Group "ENGINE(H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=K:\h\kutlStr.h
# End Source File
# Begin Source File

SOURCE=K:\h\SchJobIntf.h
# End Source File
# End Group
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Group "FA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\FA_Hand_Load.c
# End Source File
# Begin Source File

SOURCE=.\Source\FA_Hand_PrePost.c
# End Source File
# Begin Source File

SOURCE=".\Source\FA_Hand_Unload(M).c"
# End Source File
# Begin Source File

SOURCE=.\Source\FA_Hand_Unload.c
# End Source File
# Begin Source File

SOURCE=.\Source\FA_Handling.c
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Gui_Hand_FA.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_Fail.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_FixCluster.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_Interlock.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_Module.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_PrePost.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_RbAnim.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_RbCal.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_Recipe.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_Status.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_SwitchSide.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_System.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Hand_TrClear.c
# End Source File
# Begin Source File

SOURCE=.\Source\Gui_Handling.c
# End Source File
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\DLL_Interface.c
# End Source File
# Begin Source File

SOURCE=.\Source\DLL_InterfaceSCH.c
# End Source File
# End Group
# Begin Group "EQUIP_IO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Equip_Handling.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_ALIC.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_BEGINEND.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_BMSIDE.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_CARR.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_ETC.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_FM.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_PROCESS.c
# End Source File
# Begin Source File

SOURCE=.\Source\Equip_Handling_TM.c
# End Source File
# Begin Source File

SOURCE=.\Source\IO_Part_data.c
# End Source File
# Begin Source File

SOURCE=.\Source\IO_Part_Log.c
# End Source File
# Begin Source File

SOURCE=.\Source\iolog.c
# End Source File
# End Group
# Begin Group "RECIPE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\RcpFile_Handling.c
# End Source File
# End Group
# Begin Group "ROBOT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Robot_Animation.c
# End Source File
# Begin Source File

SOURCE=.\Source\Robot_Animation_ETC.c
# End Source File
# Begin Source File

SOURCE=.\Source\Robot_Animation_FM.c
# End Source File
# Begin Source File

SOURCE=.\Source\Robot_Animation_TM.c
# End Source File
# Begin Source File

SOURCE=.\Source\Robot_Handling.c
# End Source File
# End Group
# Begin Group "ETC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Alarm_Analysis.c
# End Source File
# Begin Source File

SOURCE=.\Source\comm_check.c
# End Source File
# Begin Source File

SOURCE=.\Source\event_Handling.c
# End Source File
# Begin Source File

SOURCE=.\Source\File_ReadInit.c
# End Source File
# Begin Source File

SOURCE=.\Source\prg_code.c
# End Source File
# Begin Source File

SOURCE=.\Source\system_Tag.c
# End Source File
# Begin Source File

SOURCE=.\Source\Timer_Handling.c
# End Source File
# Begin Source File

SOURCE=.\Source\User_Parameter.c
# End Source File
# Begin Source File

SOURCE=.\Source\Utility.c
# End Source File
# End Group
# Begin Group "SCHEDULING"

# PROP Default_Filter ""
# Begin Group "COMMON"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\MR_Data.c
# End Source File
# Begin Source File

SOURCE=.\Source\MultiJob.c
# End Source File
# Begin Source File

SOURCE=.\Source\Multireg.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_CassetteResult.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_CassetteSupply.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_cassmap.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Common.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_EIL.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Estimate.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_FMARM_Multi.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_HandOff.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Interlock.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro_ALIC.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro_BM.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro_Flow.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro_FM.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro_Process.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Macro_TM.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Main.c
# End Source File
# Begin Source File

SOURCE=.\Source\Sch_Multi_ALIC.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_OneSelect.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_PrepPost.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_PrePrcs.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_prm.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_prm_Cluster.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_prm_ClusterI.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_prm_FBTPM.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_prm_Log.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_ProcessMonitor.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_RecipeHandling.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_ReStart.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Run.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_sdm.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_sub.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_subi.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_Util.c
# End Source File
# End Group
# Begin Group "Main-Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\sch_Run_Common.c
# End Source File
# End Group
# Begin Group "TRANSFER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\sch_Transfer.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_TransferMaintInf.c
# End Source File
# Begin Source File

SOURCE=.\Source\sch_TransferUser.c
# End Source File
# End Group
# Begin Group "COMMON-USER(INF)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\sch_CommonUser.c
# End Source File
# End Group
# End Group
# Begin Group "INTERFACE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\INF_Extern.c
# End Source File
# End Group
# End Group
# Begin Group "APPLICATION"

# PROP Default_Filter ""
# Begin Group "INFORMATION"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Info\A0-S0-FMDbl.JPG"

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\Info\A6-S0-DoubleTM.JPG"

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Info\SchedulerItem.txt

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Info\SchedulerUpdate.txt
# End Source File
# End Group
# Begin Group "SOURCE"

# PROP Default_Filter ""
# Begin Group "ALG-0"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_CommonUser.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Dll.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Equip.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Event.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Init.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_log.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Param.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Run.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Run_F.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Run_F_DBL1.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Run_F_Def.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_Run_F_SW.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_sub.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_sub_F_sw.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_sub_Sp2.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_sub_Sp3.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_sub_Sp4.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\Source\sch_A0_subBM.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ALG-4"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_CommonUser.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Dll.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Equip.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Event.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Init.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_log.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Param.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Run.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_Run_F.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_sdm.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_sub.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\Source\sch_A4_subBM.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ALG-6"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_CommonUser.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Dll.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Equip.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Event.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Init.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_log.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Param.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Run.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Run_Cyclon.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_Run_F.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_sub.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\Source\sch_A6_subBM.c

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "HEADER"

# PROP Default_Filter ""
# Begin Group "ALG-0(H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_CommonUser.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Default.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Dll.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Equip.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Event.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_F.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Init.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_log.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Main.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_Param.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_sub.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_sub_F_dbl1.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_sub_F_sw.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_sub_sp2.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_sub_sp3.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_sub_sp4.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_0\H\sch_A0_subBM.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ALG-4(H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_CommonUser.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Default.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Dll.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Equip.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Event.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Init.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_log.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Main.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_Param.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_sdm.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_sub.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_4\H\sch_A4_subBM.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ALG-6(H)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_CommonUser.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Default.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Dll.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Equip.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Event.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Init.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_log.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Main.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_Param.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_sub.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\APP_ALG_6\H\sch_A6_subBM.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "INF(DEFAULT)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=K:\h\SchInf\INF_CimSeqnc.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_default.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_EQ_Enum.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_Equip_Handling.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_Errorcode.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_IO_Part_data.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_IO_Part_Log.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_iolog.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_LotType.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_MultiJob.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_MultiReg.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_Robot_Handling.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_CassetteResult.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_CassetteSupply.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_CommonUser.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_FMArm_Multi.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_Macro.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_Sch_Multi_ALIC.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_OneSelect.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_prepost.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_PrePrcs.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_prm.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_prm_Cluster.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_prm_FBTPM.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_sdm.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_sub.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_sch_UserFlow.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_Scheduler.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_System_Tag.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_Timer_Handling.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=K:\h\SchInf\INF_User_Parameter.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# End Group
# Begin Group "INF(USER)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\H_User\INF_User_System.h

!IF  "$(CFG)" == "Sequence - Win32 Release"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug2_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug3_M"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU"

!ELSEIF  "$(CFG)" == "Sequence - Win32 ReleaseU_M"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Image\main.ico
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# End Target
# End Project
