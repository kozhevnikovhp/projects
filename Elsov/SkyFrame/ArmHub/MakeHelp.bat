@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by ARMHUB.HPJ. >"hlp\ArmHub.hm"
echo. >>"hlp\ArmHub.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\ArmHub.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\ArmHub.hm"
echo. >>"hlp\ArmHub.hm"
echo // Prompts (IDP_*) >>"hlp\ArmHub.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\ArmHub.hm"
echo. >>"hlp\ArmHub.hm"
echo // Resources (IDR_*) >>"hlp\ArmHub.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\ArmHub.hm"
echo. >>"hlp\ArmHub.hm"
echo // Dialogs (IDD_*) >>"hlp\ArmHub.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\ArmHub.hm"
echo. >>"hlp\ArmHub.hm"
echo // Frame Controls (IDW_*) >>"hlp\ArmHub.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\ArmHub.hm"
REM -- Make help for Project ARMHUB


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\ArmHub.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\ArmHub.hlp" goto :Error
if not exist "hlp\ArmHub.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\ArmHub.hlp" Debug
if exist Debug\nul copy "hlp\ArmHub.cnt" Debug
if exist Release\nul copy "hlp\ArmHub.hlp" Release
if exist Release\nul copy "hlp\ArmHub.cnt" Release
echo.
goto :done

:Error
echo hlp\ArmHub.hpj(1) : error: Problem encountered creating help file

:done
echo.
