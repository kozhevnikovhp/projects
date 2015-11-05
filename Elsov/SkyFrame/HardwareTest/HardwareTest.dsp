# Microsoft Developer Studio Project File - Name="HardwareTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=HardwareTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HardwareTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HardwareTest.mak" CFG="HardwareTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HardwareTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HardwareTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "SkyFrame"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HardwareTest - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib Ws2_32.lib iphlpapi.lib /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy executable to public dir
PostBuild_Cmds=copy Release\*.exe ..\exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "HardwareTest - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib Ws2_32.lib iphlpapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "HardwareTest - Win32 Release"
# Name "HardwareTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbstractForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Advantech.cpp
# End Source File
# Begin Source File

SOURCE=.\AgilentPowerMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\Attached.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoTestPages.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoTestSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonTests.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionForm.cpp
# End Source File
# Begin Source File

SOURCE=.\DataForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Datum.cpp
# End Source File
# Begin Source File

SOURCE=.\Datum_500.cpp
# End Source File
# Begin Source File

SOURCE=.\Datum_M7.cpp
# End Source File
# Begin Source File

SOURCE=.\Datum_PSM_4900.cpp
# End Source File
# Begin Source File

SOURCE=.\DatumConventionalModem.cpp
# End Source File
# Begin Source File

SOURCE=.\DemodulatorBerTestForm.cpp
# End Source File
# Begin Source File

SOURCE=.\DemodulatorCarrierForm.cpp
# End Source File
# Begin Source File

SOURCE=.\DemodulatorDataForm.cpp
# End Source File
# Begin Source File

SOURCE=.\DemodulatorLnbStatusForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Device.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceForm.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceFromName.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceSubstitutor.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceTelnetSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\DownConverterForm.cpp
# End Source File
# Begin Source File

SOURCE=.\EthernetPortForm.cpp
# End Source File
# Begin Source File

SOURCE=.\EthernetStationsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\EthernetStatusForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Frad.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameRelayStationsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\FrequencyCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\FrequencyCounterForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Ftp.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareTest.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareTest.rc
# End Source File
# Begin Source File

SOURCE=.\HardwareTestDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareTestFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareTestView.cpp
# End Source File
# Begin Source File

SOURCE=.\HumanControlled.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IcmpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\IPInterfacesForm.cpp
# End Source File
# Begin Source File

SOURCE=.\IPRoutingTableForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\IPStaticRoutesForm.cpp
# End Source File
# Begin Source File

SOURCE=.\MeasureDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc_SoftwareVersions.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscEthernet.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscSdmsStartupConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Modem.cpp
# End Source File
# Begin Source File

SOURCE=.\ModemBatchTests.cpp
# End Source File
# Begin Source File

SOURCE=.\ModemComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ModemSdmsBatchTests.cpp
# End Source File
# Begin Source File

SOURCE=.\ModulatorBerTestForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ModulatorBucStatusForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ModulatorCarrierForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ModulatorDataForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\NetworkMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\NsgFrad.cpp
# End Source File
# Begin Source File

SOURCE=.\NsgScriptForm.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysicalPortsForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Port.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Portable.cpp
# End Source File
# Begin Source File

SOURCE=.\PowerMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\PowerMeterForm.cpp
# End Source File
# Begin Source File

SOURCE=.\Pozharsky.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PvcForm.cpp
# End Source File
# Begin Source File

SOURCE=.\PvcPropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\RadioDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\SDMS_OIDs.cpp
# End Source File
# Begin Source File

SOURCE=.\SdmsBatchTests.cpp
# End Source File
# Begin Source File

SOURCE=.\SdmsStartupConfigForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SerialPort.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialPortForm.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialStatusForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SNMP_stuff.cpp
# End Source File
# Begin Source File

SOURCE=.\SoftwareVersionForm.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticRoutePropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StationsForm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StelaM.cpp
# End Source File
# Begin Source File

SOURCE=.\StelaMBatchTests.cpp
# End Source File
# Begin Source File

SOURCE=.\SubstitutorForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TcpSockets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TcpThreadSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TelnetSockets.cpp
# End Source File
# Begin Source File

SOURCE=.\TelnetStationsForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Terminal.cpp
# End Source File
# Begin Source File

SOURCE=.\TerminalForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\ThreadObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Tranceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\TranceiverBatchTests.cpp
# End Source File
# Begin Source File

SOURCE=.\TranceiverForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Udp.cpp
# End Source File
# Begin Source File

SOURCE=.\UdpBasedTerminal.cpp
# End Source File
# Begin Source File

SOURCE=.\UpConverterForm.cpp
# End Source File
# Begin Source File

SOURCE=.\UpDownConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\Vertex.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveSat.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbstractForm.h
# End Source File
# Begin Source File

SOURCE=.\Advantech.h
# End Source File
# Begin Source File

SOURCE=.\AgilentPowerMeter.h
# End Source File
# Begin Source File

SOURCE=.\Attached.h
# End Source File
# Begin Source File

SOURCE=.\AutoTestPages.h
# End Source File
# Begin Source File

SOURCE=.\AutoTestSheet.h
# End Source File
# Begin Source File

SOURCE=.\CommonTests.h
# End Source File
# Begin Source File

SOURCE=.\ConnectionForm.h
# End Source File
# Begin Source File

SOURCE=.\DataForm.h
# End Source File
# Begin Source File

SOURCE=.\Datum.h
# End Source File
# Begin Source File

SOURCE=.\Datum_M500.h
# End Source File
# Begin Source File

SOURCE=.\Datum_M7.h
# End Source File
# Begin Source File

SOURCE=.\Datum_PSM_4900.h
# End Source File
# Begin Source File

SOURCE=.\DatumConventionalModem.h
# End Source File
# Begin Source File

SOURCE=.\DemodulatorBerTestForm.h
# End Source File
# Begin Source File

SOURCE=.\DemodulatorCarrierForm.h
# End Source File
# Begin Source File

SOURCE=.\DemodulatorDataForm.h
# End Source File
# Begin Source File

SOURCE=.\DemodulatorLnbStatusForm.h
# End Source File
# Begin Source File

SOURCE=.\Device.h
# End Source File
# Begin Source File

SOURCE=.\DeviceForm.h
# End Source File
# Begin Source File

SOURCE=.\DeviceFromName.h
# End Source File
# Begin Source File

SOURCE=.\DeviceSubstitutor.h
# End Source File
# Begin Source File

SOURCE=.\DeviceTelnetSocket.h
# End Source File
# Begin Source File

SOURCE=.\DownConverterForm.h
# End Source File
# Begin Source File

SOURCE=.\EthernetPortForm.h
# End Source File
# Begin Source File

SOURCE=.\EthernetStationsForm.h
# End Source File
# Begin Source File

SOURCE=.\EthernetStatusForm.h
# End Source File
# Begin Source File

SOURCE=.\Frad.h
# End Source File
# Begin Source File

SOURCE=.\FrameRelayStationsForm.h
# End Source File
# Begin Source File

SOURCE=.\FrequencyCounter.h
# End Source File
# Begin Source File

SOURCE=.\FrequencyCounterForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Ftp.h
# End Source File
# Begin Source File

SOURCE=.\HardwareComboBox.h
# End Source File
# Begin Source File

SOURCE=.\HardwareTest.h
# End Source File
# Begin Source File

SOURCE=.\HardwareTestDoc.h
# End Source File
# Begin Source File

SOURCE=.\HardwareTestFrame.h
# End Source File
# Begin Source File

SOURCE=.\HardwareTestView.h
# End Source File
# Begin Source File

SOURCE=.\HumanControlled.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\HyperLink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IcmpSockets.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpHelper.h
# End Source File
# Begin Source File

SOURCE=.\IPInterfacesForm.h
# End Source File
# Begin Source File

SOURCE=.\IPRoutingTableForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\IpSockets.h
# End Source File
# Begin Source File

SOURCE=.\IPStaticRoutesForm.h
# End Source File
# Begin Source File

SOURCE=.\MeasureDevice.h
# End Source File
# Begin Source File

SOURCE=.\Misc_SoftwareVersions.h
# End Source File
# Begin Source File

SOURCE=.\MiscEthernet.h
# End Source File
# Begin Source File

SOURCE=.\MiscsdmsStartupConfig.h
# End Source File
# Begin Source File

SOURCE=.\Modem.h
# End Source File
# Begin Source File

SOURCE=.\ModemBatchTests.h
# End Source File
# Begin Source File

SOURCE=.\ModemComboBox.h
# End Source File
# Begin Source File

SOURCE=.\ModemSdmsBatchTests.h
# End Source File
# Begin Source File

SOURCE=.\ModulatorBerTestForm.h
# End Source File
# Begin Source File

SOURCE=.\ModulatorBucStatusForm.h
# End Source File
# Begin Source File

SOURCE=.\ModulatorCarrierForm.h
# End Source File
# Begin Source File

SOURCE=.\ModulatorDataForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\NetworkMisc.h
# End Source File
# Begin Source File

SOURCE=.\NsgFrad.h
# End Source File
# Begin Source File

SOURCE=.\NsgScriptForm.h
# End Source File
# Begin Source File

SOURCE=.\PhysicalPortsForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Port.h
# End Source File
# Begin Source File

SOURCE=.\PowerMeter.h
# End Source File
# Begin Source File

SOURCE=.\PowerMeterForm.h
# End Source File
# Begin Source File

SOURCE=.\Pozharsky.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\PvcForm.h
# End Source File
# Begin Source File

SOURCE=.\PvcPropertiesDialog.h
# End Source File
# Begin Source File

SOURCE=.\RadioDevice.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SdmsBatchTests.h
# End Source File
# Begin Source File

SOURCE=.\SdmsStartupConfigForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SerialPort.h
# End Source File
# Begin Source File

SOURCE=.\SerialPortForm.h
# End Source File
# Begin Source File

SOURCE=.\SerialStatusForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\SNMP_stuff.h
# End Source File
# Begin Source File

SOURCE=.\SoftwareVersionForm.h
# End Source File
# Begin Source File

SOURCE=.\StaticRoutePropertiesDialog.h
# End Source File
# Begin Source File

SOURCE=.\StationsForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StelaM.h
# End Source File
# Begin Source File

SOURCE=.\StelaMBatchTests.h
# End Source File
# Begin Source File

SOURCE=.\SubstitutorForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TcpSockets.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TcpThreadSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\TelnetSockets.h
# End Source File
# Begin Source File

SOURCE=.\TelnetStationsForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Terminal.h
# End Source File
# Begin Source File

SOURCE=.\TerminalForm.h
# End Source File
# Begin Source File

SOURCE=.\Tranceiver.h
# End Source File
# Begin Source File

SOURCE=.\TranceiverBatchTests.h
# End Source File
# Begin Source File

SOURCE=.\TranceiverForm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonObjects\Udp.h
# End Source File
# Begin Source File

SOURCE=.\UdpBasedTerminal.h
# End Source File
# Begin Source File

SOURCE=.\UpConverterForm.h
# End Source File
# Begin Source File

SOURCE=.\UpDownConverter.h
# End Source File
# Begin Source File

SOURCE=.\Vertex.h
# End Source File
# Begin Source File

SOURCE=.\WaveSat.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\HardwareTest.ico
# End Source File
# Begin Source File

SOURCE=.\res\HardwareTest.rc2
# End Source File
# Begin Source File

SOURCE=.\res\HardwareTestDoc.ico
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
