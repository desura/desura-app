!include "MUI2.nsh"

; Desura.nsi
;
;--------------------------------


!define MUI_ICON ".\src\branding_desura\desura.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP ".\src\branding_desura\header.bmp"
#!define MUI_HEADERIMAGE_RIGHT

; The name of the installer
Name "Desura"

; The file to write
OutFile "DesuraInstaller.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Desura

; Registry key to check for directory (so if you install again, it will overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Desura" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Functions

Function RunDesura
SetOutPath $INSTDIR
Exec "$INSTDIR\desura.exe"
FunctionEnd

; Pages

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_CHECKED
!define MUI_FINISHPAGE_RUN_FUNCTION RunDesura
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

;--------------------------------

; The stuff to install

Section "Desura (required)"

  SetShellVarContext all

  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "build\RelWithDebInfo_Out\desura.exe"
  File "build\RelWithDebInfo_Out\desura_service.exe"
  File "build\RelWithDebInfo_Out\Desura_Uninstaller.exe"
  File "build\RelWithDebInfo_Out\dumpgen.exe"
  File "build\RelWithDebInfo_Out\java_launcher.exe"
  File "build\RelWithDebInfo_Out\mcf_util.exe"
  File "build\RelWithDebInfo_Out\toolhelper.exe"
  File "build\RelWithDebInfo_Out\utility.exe"
  File /r "build\RelWithDebInfo_Out\bin"
  File /r "build\RelWithDebInfo_Out\data"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Desura "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Desura" "DisplayName" "Desura"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Desura" "UninstallString" '"$INSTDIR\Desura_Uninstaller.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Desura" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Desura" "NoRepair" 1
  WriteUninstaller "Desura_Uninstaller.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  SetShellVarContext all

  CreateDirectory "$SMPROGRAMS\Desura"
  CreateShortcut "$SMPROGRAMS\Desura\Uninstall.lnk" "$INSTDIR\Desura_Uninstaller.exe" "" "$INSTDIR\Desura_Uninstaller.exe" 0
  CreateShortcut "$SMPROGRAMS\Desura\Desura.lnk" "$INSTDIR\Desura.exe" "" "$INSTDIR\Desura.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  SetShellVarContext all
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Desura"
  DeleteRegKey HKLM SOFTWARE\Desura

  ; Remove files and uninstaller
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Desura\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Desura"
  RMDir "$INSTDIR"

SectionEnd
