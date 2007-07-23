#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include "iunitsync.h"

// FIXME this definitely cannot be used when compiling for windows on mingw under linux
// FIXME wxDynamicLibrary instead?
#ifdef WIN32
  #include <windows.h>
  #include <wx/msw/winundef.h>
  #define USYNC_CALL_CONV __stdcall
  #define MY_GET_PROC_ADDR (void*)GetProcAddress
  #define MY_LOAD_LIBRARY(LOC) (LoadLibrary(LOC))
  #define MY_DLERROR() ("bork")
  #define MY_FREELIBRARY(LIB) (FreeLibrary(LIB))
  typedef HINSTANCE lib_handle_t;
  static char* dllname = "\\unitsync.dll";
#else
  #include <dlfcn.h>
  #define USYNC_CALL_CONV
  #define MY_GET_PROC_ADDR dlsym
  #define MY_LOAD_LIBRARY(LOC) (dlopen((LOC), RTLD_LOCAL | RTLD_LAZY))
  #define MY_DLERROR() (dlerror())
  #define MY_FREELIBRARY(LIB) (dlclose(LIB))
  typedef void* lib_handle_t;
  static char* dllname = "/unitsync.so";
#endif

class wxImage;
struct SpringMapInfo;

/*
const char* __stdcall GetSpringVersion()
*/

typedef int (USYNC_CALL_CONV *InitPtr)(bool, int);
typedef void (USYNC_CALL_CONV *UnInitPtr)();

typedef int (USYNC_CALL_CONV *GetMapCountPtr)();
typedef unsigned int (USYNC_CALL_CONV *GetMapChecksumPtr)(int);
typedef const char* (USYNC_CALL_CONV *GetMapNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetMapInfoExPtr)(const char*, SpringMapInfo*, int);
typedef void* (USYNC_CALL_CONV *GetMinimapPtr)(const char*, int);

typedef unsigned int (USYNC_CALL_CONV *GetPrimaryModChecksumPtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModIndexPtr)(const char*);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetPrimaryModArchivePtr)(int);

typedef int (USYNC_CALL_CONV *GetSideCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetSideNamePtr)(int);

typedef void (USYNC_CALL_CONV *AddAllArchivesPtr)(const char*);

typedef const char * (USYNC_CALL_CONV *GetFullUnitNamePtr)(int);
typedef const char * (USYNC_CALL_CONV *GetUnitNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetUnitCountPtr)();
typedef int (USYNC_CALL_CONV *ProcessUnitsNoChecksumPtr)();


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync(): m_loaded(false) { LoadUnitSyncLib(); }
    ~SpringUnitSync() { FreeUnitSyncLib(); }

    int GetNumMods();
    bool ModExists( const std::string& modname );
    UnitSyncMod GetMod( const std::string& modname );
    int GetModIndex( const std::string& name );
    std::string GetModArchive( int index );

    int GetNumMaps();
    bool MapExists( const std::string& mapname );
    bool MapExists( const std::string& mapname, const std::string hash );
    UnitSyncMap GetMap( const std::string&, bool getmapinfo = false );
    int GetMapIndex( const std::string& name );

    int GetSideCount( const std::string& modname );
    std::string GetSideName( const std::string& modname, int index );

    bool LoadUnitSyncLib();
    void FreeUnitSyncLib();

    bool IsLoaded();

    int GetNumUnits( const std::string& modname );
    int GetUnitIndex( const std::string& modname, const std::string& name );
    std::string GetFullUnitName( const std::string& modname, int index );

    wxImage GetMinimap( const std::string& mapname, int size );
    wxImage GetCachedMinimap( const std::string& mapname, int size );

    wxString GetCachedMinimapFileName( const std::string& mapname, int size );

    void ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out );

  private:
    bool m_loaded;

    lib_handle_t m_libhandle;

    InitPtr m_init;
    UnInitPtr m_uninit;

    GetMapCountPtr m_get_map_count;
    GetMapChecksumPtr m_get_map_checksum;
    GetMapNamePtr m_get_map_name;
    GetMapInfoExPtr m_get_map_info_ex;
    GetMinimapPtr m_get_minimap;

    GetPrimaryModChecksumPtr m_get_mod_checksum;
    GetPrimaryModIndexPtr m_get_mod_index;
    GetPrimaryModNamePtr m_get_mod_name;
    GetPrimaryModCountPtr m_get_mod_count;
    GetPrimaryModArchivePtr m_get_mod_archive;

    GetSideCountPtr m_get_side_count;
    GetSideNamePtr m_get_side_name;

    AddAllArchivesPtr m_add_all_archives;

    GetUnitCountPtr m_get_unit_count;
    GetUnitNamePtr m_get_unit_name;
    GetFullUnitNamePtr m_get_unit_full_name;
    ProcessUnitsNoChecksumPtr m_proc_units_nocheck;

    void* _GetLibFuncPtr( const std::string& name );
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
