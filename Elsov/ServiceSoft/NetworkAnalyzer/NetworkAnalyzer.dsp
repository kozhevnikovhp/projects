# Microsoft Developer Studio Project File - Name="NetworkAnalyzer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NetworkAnalyzer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NetworkAnalyzer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NetworkAnalyzer.mak" CFG="NetworkAnalyzer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NetworkAnalyzer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NetworkAnalyzer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ServiceSoft"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NetworkAnalyzer - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Ws2_32.lib OpenGL32.lib glu32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "NetworkAnalyzer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 glu32.lib Ws2_32.lib OpenGL32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NetworkAnalyzer - Win32 Release"
# Name "NetworkAnalyzer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\CommonObjects\3DRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\3DView.cpp
# End Source File
# Begin Source File

SOURCE=.\Adapter.cpp
# End Source File
# Begin Source File

SOURCE=.\AddressTable.cpp
# End Source File
# Begin Source File

SOURCE=.\BackgroundThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Configurator.cpp
# End Source File
# Begin Source File

SOURCE=.\HostTrafficDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IcmpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\NetInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzer.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzer.rc
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzerFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzerView.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkDocAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkFrameAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\OdbcDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGLChartCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\OpenGLRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\OpenGLView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Portable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Rip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SnifferSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\SpySocketThread.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ThreadObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Udp.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldPlaneRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldPlaneView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\CommonObjects\3DRenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\3DView.h
# End Source File
# Begin Source File

SOURCE=.\Adapter.h
# End Source File
# Begin Source File

SOURCE=.\AddressTable.h
# End Source File
# Begin Source File

SOURCE=.\BackgroundThread.h
# End Source File
# Begin Source File

SOURCE=.\Configurator.h
# End Source File
# Begin Source File

SOURCE=.\DatabaseAPI.h
# End Source File
# Begin Source File

SOURCE=.\GuiAPI.h
# End Source File
# Begin Source File

SOURCE=.\HostTrafficDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IcmpSockets.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpSockets.h
# End Source File
# Begin Source File

SOURCE=.\NetInterface.h
# End Source File
# Begin Source File

SOURCE=.\Network.h
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzer.h
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzerDoc.h
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzerFrame.h
# End Source File
# Begin Source File

SOURCE=.\NetworkAnalyzerView.h
# End Source File
# Begin Source File

SOURCE=.\NetworkDocAPI.h
# End Source File
# Begin Source File

SOURCE=.\NetworkFrameAPI.h
# End Source File
# Begin Source File

SOURCE=.\OdbcDatabase.h
# End Source File
# Begin Source File

SOURCE=.\OpenGLChartCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\OpenGLRenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\OpenGLView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Portable.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Rip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SnifferSockets.h
# End Source File
# Begin Source File

SOURCE=.\SpySocketThread.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ThreadObject.h
# End Source File
# Begin Source File

SOURCE=.\TreeViewGui.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Udp.h
# End Source File
# Begin Source File

SOURCE=.\WorldPlaneRenderer.h
# End Source File
# Begin Source File

SOURCE=.\WorldPlaneView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\NetworkAnalyzer.ico
# End Source File
# Begin Source File

SOURCE=.\res\NetworkAnalyzer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\NetworkAnalyzerDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\world_plane_view_toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
