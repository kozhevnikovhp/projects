# Microsoft Developer Studio Project File - Name="Cuvs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Cuvs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cuvs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cuvs.mak" CFG="Cuvs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cuvs - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Cuvs - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SkyFrame"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cuvs - Win32 Release"

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
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Cuvs - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Cuvs - Win32 Release"
# Name "Cuvs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbstractForm.cpp
# End Source File
# Begin Source File

SOURCE=.\AllDevicesAndChildDevicesQuerySet.cpp
# End Source File
# Begin Source File

SOURCE=.\AllNetForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Attached.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelGroupsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelsRecordset.cpp
# End Source File
# Begin Source File

SOURCE=.\Cuvs.cpp
# End Source File
# Begin Source File

SOURCE=.\Cuvs.rc
# End Source File
# Begin Source File

SOURCE=.\CuvsDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\HardwareTest\Device.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceTypesSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\DragTreeView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ExTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\GetTmiParamSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Port.cpp
# End Source File
# Begin Source File

SOURCE=..\HardwareTest\RadioDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SerialPort.cpp
# End Source File
# Begin Source File

SOURCE=.\Station.cpp
# End Source File
# Begin Source File

SOURCE=.\StationDevicesForm.cpp
# End Source File
# Begin Source File

SOURCE=.\StationDevicesSet.cpp
# End Source File
# Begin Source File

SOURCE=.\StationPropertiesForm.cpp
# End Source File
# Begin Source File

SOURCE=.\StationsFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\StationsRecordSet.cpp
# End Source File
# Begin Source File

SOURCE=.\StationsView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\HardwareTest\StelaM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TcpSockets.cpp
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

SOURCE=..\..\..\CommonObjects\ThreadObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiAvailableParamsSet.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiQueryParametersForm.cpp
# End Source File
# Begin Source File

SOURCE=.\TmiToBeAskedSet.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbstractForm.h
# End Source File
# Begin Source File

SOURCE=.\AllDevicesAndChildDevicesQuerySet.h
# End Source File
# Begin Source File

SOURCE=.\AllNetForm.h
# End Source File
# Begin Source File

SOURCE=.\Attached.h
# End Source File
# Begin Source File

SOURCE=.\ChannelGroupsForm.h
# End Source File
# Begin Source File

SOURCE=.\ChannelsRecordset.h
# End Source File
# Begin Source File

SOURCE=.\Cuvs.h
# End Source File
# Begin Source File

SOURCE=.\CuvsDoc.h
# End Source File
# Begin Source File

SOURCE=.\DatabaseConstants.h
# End Source File
# Begin Source File

SOURCE=.\DeviceTypesSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\DragTreeView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ExTreeView.h
# End Source File
# Begin Source File

SOURCE=.\GetTmiParamSet.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Station.h
# End Source File
# Begin Source File

SOURCE=.\StationDevicesForm.h
# End Source File
# Begin Source File

SOURCE=.\StationDevicesSet.h
# End Source File
# Begin Source File

SOURCE=.\StationPropertiesForm.h
# End Source File
# Begin Source File

SOURCE=.\StationsFrame.h
# End Source File
# Begin Source File

SOURCE=.\StationsRecordSet.h
# End Source File
# Begin Source File

SOURCE=.\StationsView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\HardwareTest\StelaM.h
# End Source File
# Begin Source File

SOURCE=.\TmiAvailableParametersSet.h
# End Source File
# Begin Source File

SOURCE=.\TmiAvailableParamsSet.h
# End Source File
# Begin Source File

SOURCE=.\TmiParametersForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiQueryParametersForm.h
# End Source File
# Begin Source File

SOURCE=.\TmiToBeAskedSet.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Cuvs.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cuvs.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CuvsDoc.ico
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
