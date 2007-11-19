#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include <map>

#include "iunitsync.h"
#include "nonportable.h"

class wxImage;
class wxDynamicLibrary;
struct SpringMapInfo;
struct CachedMapInfo;
class wxCriticalSection;

typedef const char* (USYNC_CALL_CONV *GetSpringVersionPtr)();

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

typedef int (USYNC_CALL_CONV *InitFindVFSPtr)(const char*);
typedef int (USYNC_CALL_CONV *FindFilesVFSPtr)(int, char*, int);
typedef int (USYNC_CALL_CONV *OpenFileVFSPtr)(const char*);
typedef int (USYNC_CALL_CONV *FileSizeVFSPtr)(int);
typedef int (USYNC_CALL_CONV *ReadFileVFSPtr)(int, void*, int);
typedef void (USYNC_CALL_CONV *CloseFileVFSPtr)(int);


typedef std::map<std::string,CachedMapInfo> MapCacheType;


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync(): m_loaded(false),m_map_count(0),m_mod_count(0),m_side_count(0) { _LoadMapInfoExCache(); }
    ~SpringUnitSync() { FreeUnitSyncLib(); _SaveMapInfoExCache(); }

    int GetNumMods();
    bool ModExists( const std::string& modname );
    UnitSyncMod GetMod( const std::string& modname );
    UnitSyncMod GetMod( int index );
    int GetModIndex( const std::string& name );
    std::string GetModArchive( int index );

    int GetNumMaps();
    bool MapExists( const std::string& mapname );
    bool MapExists( const std::string& mapname, const std::string hash );
    UnitSyncMap GetMap( const std::string&, bool getmapinfo = false );
    UnitSyncMap GetMap( int index, bool getmapinfo = false );

    int GetMapIndex( const std::string& name );

    void SetCurrentMod( const std::string& modname );
    int GetSideCount();
    std::string GetSideName( int index );
    wxImage GetSidePicture( const std::string& SideName );

    bool LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void FreeUnitSyncLib();

    bool IsLoaded();

    std::string GetSpringVersion();

    wxArrayString GetAIList();
    wxString GetBotLibPath( const wxString& botlibname );

    int GetNumUnits();
    //int GetUnitIndex( const std::string& name );
    //std::string GetFullUnitName( int index );
    wxArrayString GetUnitsList();

    wxImage GetMinimap( const std::string& mapname, int max_w, int max_h, bool store_size = false );

    wxArrayString GetReplayList();

  private:
    bool m_loaded;

    wxDynamicLibrary* m_libhandle;

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

    InitFindVFSPtr m_init_find_vfs;
    FindFilesVFSPtr m_find_files_vfs;
    OpenFileVFSPtr m_open_file_vfs;
    FileSizeVFSPtr m_file_size_vfs;
    ReadFileVFSPtr m_read_file_vfs;
    CloseFileVFSPtr m_close_file_vfs;

    GetSpringVersionPtr m_get_spring_version;

    UnitSyncMap m_map;

    std::string m_current_mod;
    wxArrayString m_mod_units;

    int m_map_count;
    int m_mod_count;
    int m_side_count;

    MapCacheType m_mapinfo;

    wxCriticalSection m_lock;

    void* _GetLibFuncPtr( const std::string& name );
    MapInfo _GetMapInfoEx( const std::string& mapname );

    void _LoadMapInfoExCache();
    void _SaveMapInfoExCache();

    bool _LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void _FreeUnitSyncLib();

    int _GetModIndex( const std::string& name );
    bool _ModExists( const std::string& modname );
    UnitSyncMod _GetMod( int index );
    std::string _GetModArchive( int index );

    int _GetMapIndex( const std::string& name );
    UnitSyncMap _GetMap( int index, bool getmapinfo = false );
    UnitSyncMap _GetMap( const std::string& mapname, bool getmapinfo = false );

    wxImage _GetCachedMinimap( const std::string& mapname, int max_w, int max_h, bool store_size = false );
    wxString _GetCachedMinimapFileName( const std::string& mapname, int width = -1, int height = -1 );

    void _ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out );
    void _ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out );
    void _ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const std::string& mapname );

};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
