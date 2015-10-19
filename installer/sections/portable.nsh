!ifdef INSTALL

	SetOutPath "$INSTDIR"
	!insertmacro createemptyfile "$INSTDIR\springlobby.conf"
	WriteINIStr "$INSTDIR\springlobby.conf" Spring DownloadDir $INSTDIR
!else

	Delete "$INSTDIR\springsettings.cfg"
	Delete "$INSTDIR\springlobby.conf"

!endif
