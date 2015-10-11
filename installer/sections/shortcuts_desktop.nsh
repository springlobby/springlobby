
!ifdef INSTALL

	SetOutPath "$INSTDIR"

	CreateShortCut "$DESKTOP\SpringLobby.lnk" "$INSTDIR\springlobby.exe"

!else

	Delete "$DESKTOP\SpringLobby.lnk"
	Delete "$DESKTOP\Spring lobby-client Zero-K.lnk"

!endif
