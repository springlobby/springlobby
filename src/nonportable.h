/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H
#define SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H

#if defined(__WXMSW__)
  #define SPRING_VERSION_PARAM _T("/V")
  #define CHOOSE_EXE _("Executables (*.exe)|*.exe|Any File (*.*)|*.*")
  #define USYNC_CALL_CONV __stdcall
  #define SPRING_BIN _T("spring.exe")
  #define DOS_TXT true
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

