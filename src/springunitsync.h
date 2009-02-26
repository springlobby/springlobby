#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include <list>
#include <map>

#include "iunitsync.h"
#include "thread.h"
#include "utils.h"

class wxCriticalSection;
class wxDynamicLibrary;
class wxImage;
struct CachedMapInfo;
struct SpringMapInfo;
class SpringUnitSyncLib;

typedef std::map<wxString,wxString> LocalArchivesVector;


/// Thread safe MRU cache (works like a std::map but has maximum size)
template<typename TKey, typename TValue>
class MostRecentlyUsedCache
{
  public:
    MostRecentlyUsedCache(int max_size)
    : m_size(0), m_max_size(max_size), m_cache_hits(0), m_cache_misses(0)
    {
    }

    ~MostRecentlyUsedCache()
    {
      wxLogDebugFunc( _T("cache hits: ") + TowxString( m_cache_hits ) );
      wxLogDebugFunc( _T("cache misses: ") + TowxString( m_cache_misses ) );
    }

    void Add( const TKey& name, const TValue& img )
    {
      wxCriticalSectionLocker lock(m_lock);
      while ( m_size >= m_max_size ) {
        --m_size;
        m_iterator_map.erase( m_items.back().first );
        m_items.pop_back();
      }
      ++m_size;
      m_items.push_front( CacheItem( name, img ) );
      m_iterator_map[name] = m_items.begin();
    }

    bool TryGet( const TKey& name, TValue& img )
    {
      wxCriticalSectionLocker lock(m_lock);
      typename IteratorMap::iterator it = m_iterator_map.find( name );
      if ( it == m_iterator_map.end() ) {
        ++m_cache_misses;
        return false;
      }
      // reinsert at front, so that most recently used items are always at front
      m_items.push_front( *it->second );
      m_items.erase( it->second );
      it->second = m_items.begin();
      // return image
      img = it->second->second;
      ++m_cache_hits;
      return true;
    }

    void Clear()
    {
      wxCriticalSectionLocker lock(m_lock);
      m_size = 0;
      m_items.clear();
      m_iterator_map.clear();
    }

  private:
    typedef std::pair<TKey, TValue> CacheItem;
    typedef std::list<CacheItem> CacheItemList;
    typedef std::map<TKey, typename CacheItemList::iterator> IteratorMap;

    mutable wxCriticalSection m_lock;
    CacheItemList m_items;
    IteratorMap m_iterator_map;
    int m_size;
    const int m_max_size;
    int m_cache_hits;
    int m_cache_misses;
};

typedef MostRecentlyUsedCache<wxString,wxImage> MostRecentlyUsedImageCache;
typedef MostRecentlyUsedCache<wxString,MapInfo> MostRecentlyUsedMapInfoCache;


/// Thread safe mapping from evtHandlerId to wxEvtHandler*
class EvtHandlerCollection
{
  public:
    EvtHandlerCollection() : m_last_id(0) {}

    int Add( wxEvtHandler* evtHandler );
    void Remove( int evtHandlerId );
    void PostEvent( int evtHandlerId, wxEvent& evt );

  private:
    typedef std::map<int, wxEvtHandler*> EvtHandlerMap;

    mutable wxCriticalSection m_lock;
    EvtHandlerMap m_items;
    int m_last_id;
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
    wxArrayString GetModDeps( const wxString& name );

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
    wxArrayString GetMapDeps( const wxString& name );

    //! function to fetch default singplayer/replay/savegame's default nick
		wxString GetDefaultNick();
		//! function to set default singplayer/replay/savegame's default nick
		void SetDefaultNick( const wxString& nick );

    int GetMapIndex( const wxString& name );

    wxArrayString GetSides( const wxString& modname  );
    wxImage GetSidePicture( const wxString& modname, const wxString& SideName );

    bool LoadUnitSyncLib( const wxString& unitsyncloc );
    void FreeUnitSyncLib();

    bool IsLoaded();

    wxString GetSpringVersion();
    //! function wich checks if the version returned from unitsync matches a table of supported feature
    bool VersionSupports( GameFeature feature );

    wxArrayString GetAIList( const wxString& modname );
    wxArrayString GetAIInfos( int index );

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

    /// schedule a map for prefetching
    void PrefetchMap( const wxString& mapname );

    int RegisterEvtHandler( wxEvtHandler* evtHandler );
    void UnregisterEvtHandler( int evtHandlerId );
    void PostEvent( int evtHandlerId, wxEvent& evt ); // helper for WorkItems

    void GetMinimapAsync( const wxString& mapname, int evtHandlerId );
    void GetMinimapAsync( const wxString& mapname, int width, int height, int evtHandlerId );
    void GetMetalmapAsync( const wxString& mapname, int evtHandlerId );
    void GetMetalmapAsync( const wxString& mapname, int width, int height, int evtHandlerId );
    void GetHeightmapAsync( const wxString& mapname, int evtHandlerId );
    void GetHeightmapAsync( const wxString& mapname, int width, int height, int evtHandlerId );
    void GetMapExAsync( const wxString& mapname, int evtHandlerId );

    wxArrayString GetScreenshotFilenames();

  private:

    LocalArchivesVector m_maps_list; /// maphash -> mapname
    LocalArchivesVector m_mods_list; /// modhash -> modname
    wxArrayString m_map_array;
    wxArrayString m_mod_array;

    /// caches sett().GetCachePath(), because that method calls back into
    /// susynclib(), there's a good chance main thread blocks on some
    /// WorkerThread operation... cache is invalidated on reload.
    wxString m_cache_path;

    mutable wxCriticalSection m_lock;
    WorkerThread m_cache_thread;
    EvtHandlerCollection m_evt_handlers;

    /// this cache facilitates async image fetching (image is stored in cache
    /// in background thread, then main thread gets it from cache)
    MostRecentlyUsedImageCache m_map_image_cache;
    /// this cache is a real cache, it stores minimaps with max size 100x100
    MostRecentlyUsedImageCache m_tiny_minimap_cache;

    /// this caches MapInfo to facilitate GetMapExAsync
    MostRecentlyUsedMapInfoCache m_mapinfo_cache;

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

    void _GetMapImageAsync( const wxString& mapname, wxImage (SpringUnitSync::*loadMethod)(const wxString&), int evtHandlerId );
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
