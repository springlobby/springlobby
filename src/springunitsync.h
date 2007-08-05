#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include "iunitsync.h"
#include "nonportable.h"

class wxImage;
class wxDynamicLibrary;
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

typedef int (USYNC_CALL_CONV *InitFindVFSPtr)(const char*);
typedef int (USYNC_CALL_CONV *FindFilesVFSPtr)(int, char*, int);
typedef int (USYNC_CALL_CONV *OpenFileVFSPtr)(const char*);
typedef int (USYNC_CALL_CONV *FileSizeVFSPtr)(int);
typedef int (USYNC_CALL_CONV *ReadFileVFSPtr)(int, void*, int);
typedef void (USYNC_CALL_CONV *CloseFileVFSPtr)(int);


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync(): m_loaded(false) { LoadUnitSyncLib(); }
    ~SpringUnitSync() { FreeUnitSyncLib(); }

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

    int GetSideCount( const std::string& modname );
    std::string GetSideName( const std::string& modname, int index );
    wxImage GetSidePicture(const std::string& SideName);

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

    UnitSyncMap m_map;

    void* _GetLibFuncPtr( const std::string& name );
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
