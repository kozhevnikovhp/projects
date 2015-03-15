# Microsoft Developer Studio Project File - Name="GranSim2D_GUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GranSim2D_GUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GranSim2D_GUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GranSim2D_GUI.mak" CFG="GranSim2D_GUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GranSim2D_GUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GranSim2D_GUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Gena"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GranSim2D_GUI - Win32 Release"

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
# ADD LINK32 glu32.lib Ws2_32.lib OpenGL32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GranSim2D_GUI - Win32 Debug"

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

# Name "GranSim2D_GUI - Win32 Release"
# Name "GranSim2D_GUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\3DRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\3DView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Calculations.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\colorbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlForms.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Geometry2D.cpp
# End Source File
# Begin Source File

SOURCE=.\GranSim2D.cpp
# End Source File
# Begin Source File

SOURCE=.\GranSim2D.rc
# End Source File
# Begin Source File

SOURCE=..\GranSim2DDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\GranSim2DDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GranSim2DProjectView.cpp
# End Source File
# Begin Source File

SOURCE=.\GranSim2DRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\GranSim2DView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\OpenGLEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\OpenGLRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\OpenGLView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Portable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\RandomGen.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Calculations.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\colorbutton.h
# End Source File
# Begin Source File

SOURCE=.\ControlForms.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Geometry2D.h
# End Source File
# Begin Source File

SOURCE=.\GranSim2D.h
# End Source File
# Begin Source File

SOURCE=..\GranSim2DDesign.h
# End Source File
# Begin Source File

SOURCE=.\GranSim2DDoc.h
# End Source File
# Begin Source File

SOURCE=.\GranSim2DProjectView.h
# End Source File
# Begin Source File

SOURCE=.\GranSim2DRenderer.h
# End Source File
# Begin Source File

SOURCE=.\GranSim2DView.h
# End Source File
# Begin Source File

SOURCE=..\GuiAPI.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\OpenGLEditor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\Portable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonObjects\RandomGen.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\design_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\design_2d_view_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\GranSim2D.ico
# End Source File
# Begin Source File

SOURCE=.\res\GranSim2D.rc2
# End Source File
# Begin Source File

SOURCE=.\res\GranSim2DDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\GranSim2D.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
