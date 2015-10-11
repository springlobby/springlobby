!ifdef INSTALL
	!include "include/zipdll.nsh"
	SetOutPath "$INSTDIR"
	SetOverWrite on
	File /oname=$TEMP\springlobby.zip "${SPRINGLOBBY_ARCHIVE}"

	${If} ${FileExists} "$INSTDIR\uninst.exe"
		MessageBox MB_ICONINFORMATION|MB_YESNO "Spring is already installed in this directory, do you want to uninstall it before continueing?" /SD IDYES IDNO fail
		ExecWait '"$INSTDIR\uninst.exe" /S _?=$INSTDIR' $0
		${If} $0 != 0
			fail:
			Abort "Uninstallation failed, please choose a different installation Directory or cleanup the destination directory."
		${EndIf}
	${EndIf}

	${!echonow} "Processing: engine"

	!insertmacro ZIPDLL_EXTRACT "$TEMP\springlobby.zip" "$INSTDIR" "<ALL>"
	Delete "$TEMP\springlobby.zip"

!else
	${!echonow} "Processing: main: Uninstall"

	; Generated stuff from the installer
	Delete "$INSTDIR\${PRODUCT_NAME}.url"
	Delete "$INSTDIR\uninst.exe"

	; Generated stuff from running spring
	Delete "$INSTDIR\cache\ArchiveCache.lua"
	Delete "$INSTDIR\unitsync.log"
	Delete "$INSTDIR\infolog.txt"
	Delete "$INSTDIR\ext.txt"
	RmDir "$INSTDIR\demos"
	RmDir "$INSTDIR\maps"
	RmDir "$INSTDIR\games"

	; Registry Keys
	DeleteRegKey ${PRODUCT_ROOT_KEY} ${PRODUCT_KEY}
	DeleteRegKey HKCR "spring"

	MessageBox MB_YESNO|MB_ICONQUESTION "Do you want me to completely remove all spring related files?$\n\
			All maps, games, screenshots and your settings will be removed. $\n\
			CAREFULL: ALL CONTENTS OF YOUR SPRING INSTALLATION DIRECTORY WILL BE REMOVED! " \
			/SD IDNO IDNO skip_purge
		RmDir /r "$INSTDIR"
		RmDir /r "$DOCUMENTS\My Games\Spring"
		Delete "$LOCALAPPDATA\springsettings.cfg"
		Delete "$APPDATA\springlobby.conf"
	skip_purge:

!endif
