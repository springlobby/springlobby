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


typedef std::map<std::string,CachedMapInfo> MapCacheType;


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync(): m_map_count(0),m_mod_count(0),m_side_count(0) { _LoadMapInfoExCache(); }
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

    UnitSyncMap GetMap( const std::string& mapname );
    UnitSyncMap GetMap( int index );
    UnitSyncMap GetMapEx( const std::string& mapname );
    UnitSyncMap GetMapEx( int index );

    int GetMapIndex( const std::string& name );

    int GetSideCount( const std::string& modname );
    std::string GetSideName( const std::string& modname, int index );
    wxImage GetSidePicture( const std::string& modname, const std::string& SideName );

    bool LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void FreeUnitSyncLib();

    bool IsLoaded();

    std::string GetSpringVersion();
    //! function wich checks if the version returned from unitsync matches a table of supported feature
    bool VersionSupports( GameFeature feature );

    wxArrayString GetAIList();

    int GetNumUnits( const std::string& modname );
    wxArrayString GetUnitsList( const std::string& modname );

    wxImage GetMinimap( const std::string& mapname, int max_w, int max_h, bool store_size = false );

    bool CacheMapInfo( const wxString& map );
    bool CacheMinimap( const wxString& map );
    bool CacheModUnits( const wxString& mod );
    bool ReloadUnitSyncLib();

  private:

    static wxString _GetCachedMinimapFileName( const std::string& mapname, int width = -1, int height = -1 );

    UnitSyncMap m_map;

    wxArrayString m_mod_units;

    int m_map_count;
    int m_mod_count;
    int m_side_count;

    MapCacheType m_mapinfo;

    wxCriticalSection m_lock;


//    void* _GetLibFuncPtr( const std::string& name );
    MapInfo _GetMapInfoEx( const std::string& mapname );

    void _LoadMapInfoExCache();
    void _SaveMapInfoExCache();

    bool _LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc );
    void _FreeUnitSyncLib();

    bool _ModExists( const std::string& modname );
    UnitSyncMod _GetMod( int index );
    std::string _GetModArchive( int index );

    int _GetMapIndex( const std::string& name );
    UnitSyncMap _GetMap( int index, bool getmapinfo = false );
    UnitSyncMap _GetMap( const std::string& mapname, bool getmapinfo = false );
    UnitSyncMap _GetMapEx( const std::string& mapname, bool force = false );
    MapInfo _GetMapInfoEx( const std::string& mapname, bool force );
    wxImage _GetCachedMinimap( const std::string& mapname, int max_w, int max_h, bool store_size = false );

    void _ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out );
    void _ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const std::string& mapname );

};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
