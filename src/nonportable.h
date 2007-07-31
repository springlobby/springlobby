#ifndef SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H
#define SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H

#ifdef WIN32
  #define SPRING_VERSION_PARAM _T(" /V")
  #define DLLEXT _T(".dll")
  #define EXEEXT _T(".exe")
  #define CHOOSE_EXE _("Executables (*.exe)|*.exe|Any File (*.*)|*.*")
  #define CHOOSE_DLL _("Library (*.dll)|*.dll|Any File (*.*)|*.*")
  #define USYNC_CALL_CONV __stdcall
  #define MY_DLERROR() ("bork")
  #define PATH_SEP _T("\\")
  #define SPRING_BIN _T("spring.exe")
  #define DOS_TXT true
#else
  #define SPRING_VERSION_PARAM _T(" -V")
  #define DLLEXT _T(".so")
  #define EXEEXT _T("")
  #define CHOOSE_EXE _("Any file (*.*)|*.*")
  #define CHOOSE_DLL _("Library (*.so)|*.so|Any File (*.*)|*.*")
  #define USYNC_CALL_CONV
  #define MY_DLERROR() ("bork")
  #define PATH_SEP _T("/")
  #define SPRING_BIN _T("spring")
  #define DOS_TXT false
#endif

#endif // SPRINGLOBBY_HEADERGUARD_NONPORTABLE_H
