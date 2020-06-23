FLATHUB=https://flathub.org/repo/flathub.flatpakrepo

flatpak remote-add --if-not-exists flathub $FLATHUB
flatpak install -y flathub org.freedesktop.Platform 18.08
flatpak install -y flathub org.freedesktop.Sdk 18.08
flatpak-builder --repo=repo ./build com.springrts.SpringLobby.json
flatpak build-bundle repo springlobby.flatpak com.springrts.SpringLobby --runtime-repo=$FLATHUB

