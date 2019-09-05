# Microsoft Developer Studio Project File - Name="Scheduler_Interface" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Scheduler_Interface - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Scheduler_Interface.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Scheduler_Interface.mak" CFG="Scheduler_Interface - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Scheduler_Interface - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Scheduler_Interface - Win32 Release2" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Scheduler_Interface - Win32 Release"

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
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconio.lib /nologo /subsystem:console /pdb:none /machine:I386 /libpath:"k:\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimSqConIO.lib /nologo /subsystem:console /out:"Sch_Api.lib" /pdb:none /machine:I386 /libpath:"k:\lib"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /I "H" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /out:"k:\lib\SchInterface.lib"

!ELSEIF  "$(CFG)" == "Scheduler_Interface - Win32 Release2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Scheduler_Interface___Win32_Release2"
# PROP BASE Intermediate_Dir "Scheduler_Interface___Win32_Release2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release2"
# PROP Intermediate_Dir "Release2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimSqConIO.lib /nologo /subsystem:console /out:"Sch_Api.lib" /pdb:none /machine:I386 /libpath:"k:\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib CimSqConIO.lib /nologo /subsystem:console /out:"Sch_Api.lib" /pdb:none /machine:I386 /libpath:"k:\lib"
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE CPP /nologo /MT /W3 /I "H" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /I "H" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_REFRENCE" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /out:"k:\lib\SchInterface.lib"
# ADD LIB32 /out:"k:\lib\SchInterfaceR.lib"

!ENDIF 

# Begin Target

# Name "Scheduler_Interface - Win32 Release"
# Name "Scheduler_Interface - Win32 Release2"
# Begin Group "Macro(H)"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\H\Macro.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\Source\sch_Macro_Interface.c
# End Source File
# End Group
# End Target
# End Project
