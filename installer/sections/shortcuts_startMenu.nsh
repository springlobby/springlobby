!ifdef INSTALL

	SetOutPath "$INSTDIR"
	; Main shortcuts
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	SetOutPath "$INSTDIR"

	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\SpringLobby.lnk" "$INSTDIR\springlobby.exe"

	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall SpringLobby.lnk" "$INSTDIR\uninst.exe"

	WriteINIStr "$SMPROGRAMS\${PRODUCT_NAME}\Read Me First.URL" "InternetShortcut" "URL" "http://springrts.com/wiki/Read_Me_First"

!else

	; Shortcuts
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\SpringLobby.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall SpringLobby.lnk"

	Delete "$SMPROGRAMS\${PRODUCT_NAME}\Read Me First.URL"

	; delete the folders
	RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

!endif
