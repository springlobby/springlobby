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

typedef std::map<wxString,wxString> LocalArchivesVector;


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync() { }
    ~SpringUnitSync() { FreeUnitSyncLib(); }

    int GetNumMods();
    wxArrayString GetModList();
    bool ModExists( const wxString& modname );
    bool ModExists( const wxString& modname, const wxString& hash );
    UnitSyncMod GetMod( const wxString& modname );
    UnitSyncMod GetMod( int index );
    int GetModIndex( const wxString& name );
    wxString GetModArchive( int index );
    GameOptions GetModOptions( const wxString& name );

    int GetNumMaps();
    wxArrayString GetMapList();
    bool MapExists( const wxString& mapname );
    bool MapExists( const wxString& mapname, const wxString& hash );

    UnitSyncMap GetMap( const wxString& mapname );
    UnitSyncMap GetMap( int index );
    UnitSyncMap GetMapEx( const wxString& mapname );
    UnitSyncMap GetMapEx( int index );
    wxString GetMapArchive( int index );
    GameOptions GetMapOptions( const wxString& name );

    int GetMapIndex( const wxString& name );

    int GetSideCount( const wxString& modname );
    wxString GetSideName( const wxString& modname, int index );
    wxImage GetSidePicture( const wxString& modname, const wxString& SideName );

    bool LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void FreeUnitSyncLib();

    bool IsLoaded();

    wxString GetSpringVersion();
    //! function wich checks if the version returned from unitsync matches a table of supported feature
    bool VersionSupports( GameFeature feature );

    wxArrayString GetAIList( const wxString& modname );

    int GetNumUnits( const wxString& modname );
    wxArrayString GetUnitsList( const wxString& modname );

    wxImage GetMinimap( const wxString& mapname, int max_w, int max_h, bool store_size = false );

    bool CacheMinimap( const wxString& map );
    bool ReloadUnitSyncLib();

    void SetSpringDataPath( const wxString& path );
    wxString GetSpringDataPath();

    wxArrayString GetReplayList();

    bool FileExists( const wxString& name );

    wxString GetArchivePath( const wxString& name );

  private:

    wxString _GetCachedMinimapFileName( const wxString& mapname, int width = -1, int height = -1 );

    UnitSyncMap m_map;

    LocalArchivesVector m_maps_list; /// maphash -> mapname
    LocalArchivesVector m_mods_list; /// modhash -> modname
    wxArrayString m_map_array;
    wxArrayString m_mod_array;

    wxCriticalSection m_lock;


//    void* _GetLibFuncPtr( const wxString& name );

    //! this function returns only the cache path without the file extension, the extension itself would be added in the function as needed
    wxString GetFileCachePath( const wxString& name, const wxString& hash, bool IsMod );

    //! returns an array where each element is a line of the file
    wxArrayString GetCacheFile( const wxString& path );
    //! write a file where each element of the array is a line
    void SetCacheFile( const wxString& path, const wxArrayString& data );

    bool _LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void _FreeUnitSyncLib();

    bool _ModExists( const wxString& modname );
    UnitSyncMod _GetMod( int index );
    wxString _GetModArchive( int index );

    int _GetMapIndex( const wxString& name );
    UnitSyncMap _GetMap( int index, bool getmapinfo = false );
    UnitSyncMap _GetMap( const wxString& mapname, bool getmapinfo = false );
    UnitSyncMap _GetMapEx( const wxString& mapname, bool force = false );
    MapInfo _GetMapInfoEx( const wxString& mapname );
    wxImage _GetCachedMinimap( const wxString& mapname, int max_w, int max_h, bool store_size = false );

    void PopulateArchiveList();

};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

