# define installer name
!define APPNAME "OpenJournal"
!define COMPANYNAME "B&GInc"
!define DESCRIPTION "A simple journal, note taking, reminder application."

!define VERSIONMAJOR 0
!define VERSIONMINOR 0
!define VERSIONBUILD 1
OutFile "C:\OpenJournalInstaller.exe"
 
# set desktop as install directory
InstallDir "$PROGRAMFILES64\${APPNAME}"
Name "${COMPANYNAME} - ${APPNAME}"

# default section start
Section
 
# define output path
SetOutPath $INSTDIR
 
# specify file to go in output path
File /r "../src\${APPNAME}\*"
File "icon.ico"
 
# define uninstaller name
WriteUninstaller $INSTDIR\uninstaller.exe

# Create shortcut
SetShellVarContext all
CreateDirectory "$SMPROGRAMS\${COMPANYNAME}"
CreateShortCut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\icon.ico"
SetShellVarContext current
 
 
#-------
# default section end
SectionEnd
 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
Section "Uninstall"
 
# Always delete uninstaller first
Delete $INSTDIR\uninstaller.exe
Delete "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk"
Delete $INSTDIR\icon.ico
 
# now delete installed file
Delete $INSTDIR\*
 
# Delete the directory
RMDir /r $INSTDIR
SectionEnd
