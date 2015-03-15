# Microsoft Developer Studio Project File - Name="Region" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Region - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Region.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Region.mak" CFG="Region - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Region - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Region - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SkyFrame"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Region - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\*.exe ..\..\exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Region - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Region - Win32 Release"
# Name "Region - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\HardwareTest\Advantech.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\AgilentPowerMeter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\Datum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\Device.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\DeviceSubstitutor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\FrequencyCounter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\IcmpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\IpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\MeasureDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\Modem.cpp
# End Source File
# Begin Source File

SOURCE=..\OneObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Port.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Portable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\PowerMeter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\RadioDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\Radyne.cpp
# End Source File
# Begin Source File

SOURCE=.\Region.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\SDMS_OIDs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\SerialPort.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\SNMP_stuff.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\StelaM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\TcpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\TelnetSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Terminal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\ThreadObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\Tranceiver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Udp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\UdpBasedTerminal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\Vertex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\HardwareTest\WaveSat.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\OneObject.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\ThreadObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Udp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
