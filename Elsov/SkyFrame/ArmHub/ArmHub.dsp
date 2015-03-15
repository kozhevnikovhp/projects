# Microsoft Developer Studio Project File - Name="ArmHub" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ArmHub - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ArmHub.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ArmHub.mak" CFG="ArmHub - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ArmHub - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ArmHub - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SkyFrame"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ArmHub - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ArmHub - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ArmHub - Win32 Release"
# Name "ArmHub - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbstractForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmHub.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmHub.rc
# End Source File
# Begin Source File

SOURCE=.\ArmHubDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmHubFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Attached.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IcmpSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\IpAddressForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpThreadedSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryParametersForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Scheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\Station.cpp
# End Source File
# Begin Source File

SOURCE=.\StationForm.cpp
# End Source File
# Begin Source File

SOURCE=.\StationsView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TcpSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\TelnetForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TelnetSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Terminal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiBstStatusForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiIpStatisticsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiModemStatusForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiNetDevicesStatisticsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiOptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiStatisticsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiUdpStatisticsForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Udp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbstractForm.h
# End Source File
# Begin Source File

SOURCE=.\ArmHub.h
# End Source File
# Begin Source File

SOURCE=.\ArmHubDoc.h
# End Source File
# Begin Source File

SOURCE=.\ArmHubFrame.h
# End Source File
# Begin Source File

SOURCE=.\Attached.h
# End Source File
# Begin Source File

SOURCE=.\IpAddressForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpThreadedSockets.h
# End Source File
# Begin Source File

SOURCE=.\LogFile.h
# End Source File
# Begin Source File

SOURCE=.\QueryParametersForm.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Scheduler.h
# End Source File
# Begin Source File

SOURCE=.\Station.h
# End Source File
# Begin Source File

SOURCE=.\StationForm.h
# End Source File
# Begin Source File

SOURCE=.\StationsView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TelnetForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiBstStatusForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiIpStatisticsForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiModemStatusForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiNetDeviceStatisticsForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiOptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\TmiStatisticsForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiUdpStatisticsForm.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ArmHub.ico
# End Source File
# Begin Source File

SOURCE=.\res\ArmHub.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ArmHubDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
