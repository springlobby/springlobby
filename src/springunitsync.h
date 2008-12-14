#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include <list>
#include <map>

#include "iunitsync.h"
#include "thread.h"

class wxCriticalSection;
class wxDynamicLibrary;
class wxImage;
struct CachedMapInfo;
struct SpringMapInfo;
class SpringUnitSyncLib;

typedef std::map<wxString,wxString> LocalArchivesVector;


class MostRecentlyUsedImageCache
{
  public:
    MostRecentlyUsedImageCache(int max_size);

    void Add( const wxString& name, const wxImage& img );
    bool TryGet( const wxString& name, wxImage& img );
    void Clear();

  private:
    typedef std::pair<wxString, wxImage> CacheItem;
    typedef std::list<CacheItem> ImageList;
    typedef std::map<wxString, ImageList::iterator> IteratorMap;

    mutable wxCriticalSection m_lock;
    ImageList m_items;
    IteratorMap m_iterator_map;
    int m_size;
    const int m_max_size;
};


class SpringUnitSync : public IUnitSync
{
  public:
    SpringUnitSync();
    ~SpringUnitSync();

    int GetNumMods();
    wxArrayString GetModList();
    bool ModExists( const wxString& modname );
    bool ModExists( const wxString& modname, const wxString& hash );
    bool ModExistsCheckHash( const wxString& hash ) const;
    UnitSyncMod GetMod( const wxString& modname );
    UnitSyncMod GetMod( int index );
    int GetModIndex( const wxString& name );
    wxString GetModArchive( int index );
    GameOptions GetModOptions( const wxString& name );

    int GetNumMaps();
    wxArrayString GetMapList();
    wxArrayString GetModValidMapList( const wxString& modname );
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

    bool LoadUnitSyncLib( const wxString& unitsyncloc );
    void FreeUnitSyncLib();

    bool IsLoaded();

    wxString GetSpringVersion();
    //! function wich checks if the version returned from unitsync matches a table of supported feature
    bool VersionSupports( GameFeature feature );

    wxArrayString GetAIList( const wxString& modname );

    int GetNumUnits( const wxString& modname );
    wxArrayString GetUnitsList( const wxString& modname );

    /// get minimap with native width x height
    wxImage GetMinimap( const wxString& mapname );
    /// get minimap rescaled to given width x height
    wxImage GetMinimap( const wxString& mapname, int width, int height );
    /// get metalmap with native width x height
    wxImage GetMetalmap( const wxString& mapname );
    /// get metalmap rescaled to given width x height
    wxImage GetMetalmap( const wxString& mapname, int width, int height );
    /// get heightmap with native width x height
    wxImage GetHeightmap( const wxString& mapname );
    /// get heightmap rescaled to given width x height
    wxImage GetHeightmap( const wxString& mapname, int width, int height );

    bool ReloadUnitSyncLib();

    void SetSpringDataPath( const wxString& path );

    void GetReplayList(std::vector<wxString> &ret);

    bool FileExists( const wxString& name );

    wxString GetArchivePath( const wxString& name );

    /// loads and discards a maps minimap, metalmap and heightmap
    void CacheMap( const wxString& mapname );
    /// schedule a map for prefetching
    void PrefetchMap( const wxString& mapname );

    void GetMinimapAsync( const wxString& mapname, wxEvtHandler* evtHandler );
    void GetMetalmapAsync( const wxString& mapname, wxEvtHandler* evtHandler );
    void GetHeightmapAsync( const wxString& mapname, wxEvtHandler* evtHandler );

  private:

    LocalArchivesVector m_maps_list; /// maphash -> mapname
    LocalArchivesVector m_mods_list; /// modhash -> modname
    wxArrayString m_map_array;
    wxArrayString m_mod_array;

    mutable wxCriticalSection m_lock;
    WorkerThread m_cache_thread;
    MostRecentlyUsedImageCache m_map_image_cache;

    //! this function returns only the cache path without the file extension,
    //! the extension itself would be added in the function as needed
    wxString GetFileCachePath( const wxString& name, const wxString& hash, bool IsMod );

    //! returns an array where each element is a line of the file
    wxArrayString GetCacheFile( const wxString& path );
    //! write a file where each element of the array is a line
    void SetCacheFile( const wxString& path, const wxArrayString& data );

    bool _LoadUnitSyncLib( const wxString& unitsyncloc );
    void _FreeUnitSyncLib();

    bool _ModExists( const wxString& modname );
    UnitSyncMod _GetMod( int index );
    wxString _GetModArchive( int index );

    int _GetMapIndex( const wxString& name );
    UnitSyncMap _GetMap( int index, bool getmapinfo = false );
    UnitSyncMap _GetMap( const wxString& mapname, bool getmapinfo = false );
    UnitSyncMap _GetMapEx( const wxString& mapname, bool force = false );
    MapInfo _GetMapInfoEx( const wxString& mapname );

    void PopulateArchiveList();

    double _GetSpringVersion();

    wxImage _GetMapImage( const wxString& mapname, const wxString& imagename, wxImage (SpringUnitSyncLib::*loadMethod)(const wxString&) );
    wxImage _GetScaledMapImage( const wxString& mapname, wxImage (SpringUnitSync::*loadMethod)(const wxString&), int width, int height );

    void _GetMapImageAsync( const wxString& mapname, wxImage (SpringUnitSync::*loadMethod)(const wxString&), wxEvtHandler* evtHandler );
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
