!ifdef INSTALL

	SetOutPath "$INSTDIR"
	!insertmacro createemptyfile "$INSTDIR\springlobby.conf"

!else

	Delete "$INSTDIR\springsettings.cfg"
	Delete "$INSTDIR\springlobby.conf"

!endif
