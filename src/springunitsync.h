#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include <map>

#include "iunitsync.h"
#include "nonportable.h"
#include "bimap.h"

class wxImage;
class wxDynamicLibrary;
struct SpringMapInfo;
struct CachedMapInfo;
class wxCriticalSection;


typedef std::map<wxString,CachedMapInfo> MapCacheType;
typedef codeproject::bimap<wxString,wxString> LocalArchivesVector;


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync(): m_map_count(0),m_mod_count(0),m_side_count(0) { _LoadMapInfoExCache(); }
    ~SpringUnitSync() { FreeUnitSyncLib(); _SaveMapInfoExCache(); }

    int GetNumMods();
    wxArrayString GetModList();
    bool ModExists( const wxString& modname );
    UnitSyncMod GetMod( const wxString& modname );
    UnitSyncMod GetMod( int index );
    int GetModIndex( const wxString& name );
    wxString GetModArchive( int index );
    GameOptions GetModOptions( const wxString& name );

    int GetNumMaps();
    wxArrayString GetMapList();
    bool MapExists( const wxString& mapname );
    bool MapExists( const wxString& mapname, const wxString hash );

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

    bool CacheMapInfo( const wxString& map );
    bool CacheMinimap( const wxString& map );
    bool CacheModUnits( const wxString& mod );
    bool ReloadUnitSyncLib();

    void SetSpringDataPath( const wxString& path );
    wxString GetSpringDataPath();

    bool FileExists( const wxString& name );

    wxString GetArchivePath( const wxString& name );

  private:

    static wxString _GetCachedMinimapFileName( const wxString& mapname, int width = -1, int height = -1 );

    UnitSyncMap m_map;

    LocalArchivesVector m_maps_list; /// maphash -> mapname
    LocalArchivesVector m_mods_list; /// modhash -> modname
    wxArrayString m_map_array;
    wxArrayString m_mod_array;

    wxArrayString m_mod_units;

    int m_map_count;
    int m_mod_count;
    int m_side_count;

    MapCacheType m_mapinfo;

    wxCriticalSection m_lock;


//    void* _GetLibFuncPtr( const wxString& name );
    MapInfo _GetMapInfoEx( const wxString& mapname );

    void _LoadMapInfoExCache();
    void _SaveMapInfoExCache();

    bool _LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void _FreeUnitSyncLib();

    bool _ModExists( const wxString& modname );
    UnitSyncMod _GetMod( int index );
    wxString _GetModArchive( int index );

    int _GetMapIndex( const wxString& name );
    UnitSyncMap _GetMap( int index, bool getmapinfo = false );
    UnitSyncMap _GetMap( const wxString& mapname, bool getmapinfo = false );
    UnitSyncMap _GetMapEx( const wxString& mapname, bool force = false );
    MapInfo _GetMapInfoEx( const wxString& mapname, bool force );
    wxImage _GetCachedMinimap( const wxString& mapname, int max_w, int max_h, bool store_size = false );

    void _ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out );
    void _ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const wxString& mapname );

    void PopulateArchiveList();

};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
