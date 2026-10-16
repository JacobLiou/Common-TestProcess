# Microsoft Developer Studio Project File - Name="TestProcess" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TestProcess - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TestProcess.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestProcess.mak" CFG="TestProcess - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestProcess - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TestProcess - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestProcess - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 dllpro.lib Gpib-32.obj /nologo /subsystem:windows /dll /machine:I386 /def:".\TestProcess.def" /out:"..\bin\TestProcess.dll" /implib:"../bin/Release/TestProcess.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TestProcess - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../lib/dllpro.lib ../lib/TaskMenuLib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../bin/TestProcess.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TestProcess - Win32 Release"
# Name "TestProcess - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BundleFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Calpolyn.cpp
# End Source File
# Begin Source File

SOURCE=.\DGEModule.cpp
# End Source File
# Begin Source File

SOURCE=.\FSTPScan.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphPro.cpp
# End Source File
# Begin Source File

SOURCE=.\MainTestPro.cpp
# End Source File
# Begin Source File

SOURCE=.\NetCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\OpComm.cpp
# End Source File
# Begin Source File

SOURCE=.\OpCommModBus.cpp
# End Source File
# Begin Source File

SOURCE=.\OpCommModBus.h
# End Source File
# Begin Source File

SOURCE=.\OpCRC32.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SwitchDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskPro.cpp
# End Source File
# Begin Source File

SOURCE=.\TestBaseItem.cpp
# End Source File
# Begin Source File

SOURCE=.\TestProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\TestProcess.def

!IF  "$(CFG)" == "TestProcess - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TestProcess - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TestProcess.rc
# End Source File
# Begin Source File

SOURCE=.\TestTaskManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TPInfoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLControl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BundleFile.h
# End Source File
# Begin Source File

SOURCE=.\Calpolyn.h
# End Source File
# Begin Source File

SOURCE=.\DGEModule.h
# End Source File
# Begin Source File

SOURCE=.\FSTPScan.h
# End Source File
# Begin Source File

SOURCE=.\GraphPro.h
# End Source File
# Begin Source File

SOURCE=.\MainTestPro.h
# End Source File
# Begin Source File

SOURCE=.\ModuleCMD.h
# End Source File
# Begin Source File

SOURCE=.\NetCommand.h
# End Source File
# Begin Source File

SOURCE=..\include\NetCommInterface.h
# End Source File
# Begin Source File

SOURCE=.\OpComm.h
# End Source File
# Begin Source File

SOURCE=.\OpCRC32.h
# End Source File
# Begin Source File

SOURCE=.\ProcessMsg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SwitchDriver.h
# End Source File
# Begin Source File

SOURCE=.\TaskPro.h
# End Source File
# Begin Source File

SOURCE=.\TestBaseItem.h
# End Source File
# Begin Source File

SOURCE=.\TestProcess.h
# End Source File
# Begin Source File

SOURCE=.\TestTaskManager.h
# End Source File
# Begin Source File

SOURCE=.\TPInfoManager.h
# End Source File
# Begin Source File

SOURCE=.\XMLControl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\TestProcess.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section TestProcess : {64F547A9-2297-11D5-86F8-00B0D0B398B0}
# 	2:21:DefaultSinkHeaderFile:op816x.h
# 	2:16:DefaultSinkClass:COp816X
# End Section
# Section TestProcess : {64F547A7-2297-11D5-86F8-00B0D0B398B0}
# 	2:5:Class:COp816X
# 	2:10:HeaderFile:op816x.h
# 	2:8:ImplFile:op816x.cpp
# End Section
