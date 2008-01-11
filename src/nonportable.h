#ifndef SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H
#define SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H

#ifdef WXMSW
  #define SPRING_VERSION_PARAM _T("/V")
  #define CHOOSE_EXE _("Executables (*.exe)|*.exe|Any File (*.*)|*.*")
  #define CHOOSE_DLL _("Library (*.dll)|*.dll|Any File (*.*)|*.*")
  #define DLL_EXTENSION _T(".dll")
  #define USYNC_CALL_CONV __stdcall
  #define SPRING_BIN _T("spring.exe")
  #define UNITSYNC_BIN _T("unitsync.dll")
  #define DOS_TXT true
#elifdef WXGTK
  #define SPRING_VERSION_PARAM _T("-V")
  #define CHOOSE_EXE _("Any file (*)|*")
  #define CHOOSE_DLL _("Library (*.so)|*.so|Any File (*.*)|*.*")
  #define DLL_EXTENSION _T(".so")
  #define USYNC_CALL_CONV
  #define SPRING_BIN _T("spring")
  #define UNITSYNC_BIN _T("unitsync.so")
  #define DOS_TXT false
#elifdef WXMAC
  #define SPRING_VERSION_PARAM _T("-V")
  #define CHOOSE_EXE _("App Bundles (*.app)|*.app|Any File (*.*)|*.*")
  #define CHOOSE_DLL _("Library (*.dylib)|*.dylib|Any File (*.*)|*.*")
  #define DLL_EXTENSION _T(".dylib")
  #define USYNC_CALL_CONV
  #define SPRING_BIN _T("spring")
  #define UNITSYNC_BIN _T("unitsync.dylib")
  #define DOS_TXT false
#endif

#endif // SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H
