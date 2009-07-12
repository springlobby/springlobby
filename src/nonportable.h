#ifndef SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H
#define SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H

#if defined(__WXMSW__)
  #define SPRING_VERSION_PARAM _T("/V")
  #define CHOOSE_EXE _("Executables (*.exe)|*.exe|Any File (*.*)|*.*")
  #define USYNC_CALL_CONV __stdcall
  #define SPRING_BIN _T("spring.exe")
  #define DOS_TXT true
  #define SL_DUMMY_COL //for custom virt listctrls
#elif defined(__WXGTK__) || defined(__WXX11__)
  #define SPRING_VERSION_PARAM _T("-V")
  #define CHOOSE_EXE _("Any file (*)|*")
  #define USYNC_CALL_CONV
  #define SPRING_BIN _T("spring")
  #define DOS_TXT false
#elif defined(__WXMAC__)
  #define SPRING_VERSION_PARAM _T("-V")
  #define CHOOSE_EXE _("App Bundles (*.app)|*.app|Any File (*.*)|*.*")
  #define USYNC_CALL_CONV
  #define SPRING_BIN _T("Spring.app")
  #define DOS_TXT false
#endif

#endif // SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

