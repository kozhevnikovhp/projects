# Microsoft Developer Studio Project File - Name="ControlSystemWin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ControlSystemWin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ControlSystemWin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ControlSystemWin.mak" CFG="ControlSystemWin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ControlSystemWin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ControlSystemWin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SkyFrame"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ControlSystemWin - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\*.exe ..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ControlSystemWin - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\*.exe ..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ControlSystemWin - Win32 Release"
# Name "ControlSystemWin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbstractForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Attached.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlledObjectsRecordset.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSystemDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSystemDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSystemFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSystemView.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSystemWin.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSystemWin.rc
# End Source File
# Begin Source File

SOURCE=.\DeleteObjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\DragTreeView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\DynamicLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLogForm.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLogRecordset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ExTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\MonitorAndControlForm.cpp
# End Source File
# Begin Source File

SOURCE=.\NewControlTargetDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectConnectionsRecordset.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectForm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TargetForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TargetInstance.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbstractForm.h
# End Source File
# Begin Source File

SOURCE=.\Attached.h
# End Source File
# Begin Source File

SOURCE=.\ControlledObjectsRecordset.h
# End Source File
# Begin Source File

SOURCE=.\ControlObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\ControlSystemAPI.h
# End Source File
# Begin Source File

SOURCE=.\ControlSystemDatabase.h
# End Source File
# Begin Source File

SOURCE=.\ControlSystemDoc.h
# End Source File
# Begin Source File

SOURCE=.\ControlSystemFrame.h
# End Source File
# Begin Source File

SOURCE=.\ControlSystemView.h
# End Source File
# Begin Source File

SOURCE=.\ControlSystemWin.h
# End Source File
# Begin Source File

SOURCE=.\DeleteObjectDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\DragTreeView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\DynamicLibrary.h
# End Source File
# Begin Source File

SOURCE=.\EventLogForm.h
# End Source File
# Begin Source File

SOURCE=.\EventLogRecordset.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ExTreeView.h
# End Source File
# Begin Source File

SOURCE=.\MonitorAndControlForm.h
# End Source File
# Begin Source File

SOURCE=.\NewControlTargetDialog.h
# End Source File
# Begin Source File

SOURCE=.\ObjectConnectionsRecordset.h
# End Source File
# Begin Source File

SOURCE=.\ObjectForm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TargetForm.h
# End Source File
# Begin Source File

SOURCE=.\TargetInstance.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ControlSystemDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\ControlSystemWin.ico
# End Source File
# Begin Source File

SOURCE=.\res\ControlSystemWin.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
