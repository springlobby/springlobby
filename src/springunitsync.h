#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

#include "globalsmanager.h"
#include "thread.h"
#include "mmoptionmodel.h"
#include "utils/globalevents.h"
#include "springunitsync_data.h"
#include "mru_cache.h"

#include <map>

#include <wx/image.h>
#include <wx/event.h>


#ifdef SL_QT_MODE
class QImage;
#endif

class wxImage;

extern const wxEventType UnitSyncAsyncOperationCompletedEvt;
struct GameOptions;
class wxDynamicLibrary;
class wxImage;
struct CachedMapInfo;
struct SpringMapInfo;
class SpringUnitSyncLib;

#ifdef SL_QT_MODE
class QImage;
#endif

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

class SpringUnitSync : public wxEvtHandler
{
private:
	SpringUnitSync();
	friend class GlobalObjectHolder<SpringUnitSync, LineInfo<SpringUnitSync> >;

public:
	virtual ~SpringUnitSync();

	enum GameFeature
	{
	  USYNC_Sett_Handler,
	  USYNC_GetInfoMap,
	  USYNC_GetDataDir,
	  USYNC_GetSkirmishAI
	};

	enum MediaType
	{
	  map,
	  mod
	};

	typedef std::map<wxString,mmOptionBool> OptionMapBool;
	typedef std::map<wxString,mmOptionFloat> OptionMapFloat;
	typedef std::map<wxString,mmOptionString> OptionMapString;
	typedef std::map<wxString,mmOptionList> OptionMapList;
	typedef std::map<wxString,mmOptionSection> OptionMapSection;

	typedef std::map<wxString,mmOptionBool>::iterator OptionMapBoolIter;
	typedef std::map<wxString,mmOptionFloat>::iterator OptionMapFloatIter;
	typedef std::map<wxString,mmOptionString>::iterator OptionMapStringIter;
	typedef std::map<wxString,mmOptionList>::iterator OptionMapListIter;
	typedef std::map<wxString,mmOptionSection>::iterator OptionMapSectionIter;

	typedef std::map<wxString,mmOptionBool>::const_iterator OptionMapBoolConstIter;
	typedef std::map<wxString,mmOptionFloat>::const_iterator OptionMapFloatConstIter;
	typedef std::map<wxString,mmOptionString>::const_iterator OptionMapStringConstIter;
	typedef std::map<wxString,mmOptionList>::const_iterator OptionMapListConstIter;
	typedef std::map<wxString,mmOptionSection>::const_iterator OptionMapSectionConstIter;

	int GetNumMods() const;
	wxArrayString GetModList() const;
	bool ModExists( const wxString& modname ) const;
	bool ModExists( const wxString& modname, const wxString& hash ) const;
    bool ModExistsCheckHash( const wxString& hash ) const;
	UnitSyncMod GetMod( const wxString& modname );
	UnitSyncMod GetMod( int index );
	//! this functions returns index CUSTOM ALPHBETICALLY SORTED, DO NOT USE TO ACCESS UNITSYNC DIRECTLY
	//! use m_unsorted_mod_array for real unitsync index
	int GetModIndex( const wxString& name ) const;
    GameOptions GetModOptions( const wxString& name );
	wxArrayString GetModDeps( const wxString& name ) const;

	int GetNumMaps() const;
	wxArrayString GetMapList() const;
	wxArrayString GetModValidMapList( const wxString& modname ) const;
	bool MapExists( const wxString& mapname ) const;
	bool MapExists( const wxString& mapname, const wxString& hash ) const;

    UnitSyncMap GetMap( const wxString& mapname );
	UnitSyncMap GetMap( int index );
    UnitSyncMap GetMapEx( const wxString& mapname );
	UnitSyncMap GetMapEx( int index );
    GameOptions GetMapOptions( const wxString& name );
    wxArrayString GetMapDeps( const wxString& name );

    //! function to fetch default singplayer/replay/savegame's default nick
		wxString GetDefaultNick();
		//! function to set default singplayer/replay/savegame's default nick
		void SetDefaultNick( const wxString& nick );
	//! this functions returns index CUSTOM ALPHBETICALLY SORTED, DO NOT USE TO ACCESS UNITSYNC DIRECTLY
	//! use m_unsorted_map_array for real unitsync index
	int GetMapIndex( const wxString& name ) const;

    wxArrayString GetSides( const wxString& modname  );
	wxImage GetSidePicture( const wxString& modname, const wxString& SideName ) const;
	wxImage GetImage( const wxString& modname, const wxString& image_path, bool useWhiteAsTransparent = true ) const;
#ifdef SL_QT_MODE
	QImage GetQImage( const wxString& modname, const wxString& image_path, bool useWhiteAsTransparent = true ) const;
#endif

    bool LoadUnitSyncLib( const wxString& unitsyncloc );
    void FreeUnitSyncLib();

	bool IsLoaded() const;

	wxString GetSpringVersion() const;
    //! function wich checks if the version returned from unitsync matches a table of supported feature
	bool VersionSupports( GameFeature feature ) const;

	void UnSetCurrentMod();

	wxArrayString GetAIList( const wxString& modname ) const;
	wxArrayString GetAIInfos( int index ) const;
    GameOptions GetAIOptions( const wxString& modname, int index );


	int GetNumUnits( const wxString& modname ) const;
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

	wxString GetTextfileAsString( const wxString& modname, const wxString& file_path );

	bool ReloadUnitSyncLib(  );
	void ReloadUnitSyncLib( GlobalEvents::GlobalEventData /*data*/ ) { ReloadUnitSyncLib(); }
	bool FastLoadUnitSyncLib( const wxString& unitsyncloc );
	bool FastLoadUnitSyncLibInit();

	void SetSpringDataPath( const wxString& path );
	bool GetSpringDataPath( wxString& path);

	wxArrayString GetPlaybackList( bool ReplayType = true ) const; //savegames otehrwise

	bool FileExists( const wxString& name ) const;

	wxString GetArchivePath( const wxString& name ) const;

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

	wxArrayString GetScreenshotFilenames() const;

    virtual GameOptions GetModCustomizations( const wxString& modname );
    virtual GameOptions GetSkirmishOptions( const wxString& modname, const wxString& skirmish_name );

	virtual void OnReload( wxCommandEvent& event );
	virtual void AddReloadEvent(  );

	wxArrayString FindFilesVFS( const wxString& pattern ) const;

  private:
	typedef std::map< std::pair<wxString,wxString>, wxString> ShortnameVersionToNameMap;
	ShortnameVersionToNameMap m_shortname_to_name_map;

    LocalArchivesVector m_maps_list; /// mapname -> hash
    LocalArchivesVector m_mods_list; /// modname -> hash
    LocalArchivesVector m_mods_unchained_hash; /// modname -> unchained hash
    LocalArchivesVector m_maps_unchained_hash; /// mapname -> unchained hash
    LocalArchivesVector m_mods_archive_name; /// modname -> archive name
    LocalArchivesVector m_maps_archive_name; /// mapname -> archive name
	wxArrayString m_map_array; // this vector is CUSTOM SORTED ALPHABETICALLY, DON'T USE TO ACCESS UNITSYNC DIRECTLY
	wxArrayString m_mod_array; // this vector is CUSTOM SORTED ALPHABETICALLY, DON'T USE TO ACCESS UNITSYNC DIRECTLY
	wxArrayString m_unsorted_map_array; // this is because unitsync doesn't have a search map index by name ..
	wxArrayString m_unsorted_mod_array; // this isn't necessary but makes things more symmetrical :P

    /// caches sett().GetCachePath(), because that method calls back into
    /// susynclib(), there's a good chance main thread blocks on some
    /// WorkerThread operation... cache is invalidated on reload.
    wxString m_cache_path;

    mutable wxCriticalSection m_lock;
	WorkerThread* m_cache_thread;
    EvtHandlerCollection m_evt_handlers;

    /// this cache facilitates async image fetching (image is stored in cache
    /// in background thread, then main thread gets it from cache)
    MostRecentlyUsedImageCache m_map_image_cache;
    /// this cache is a real cache, it stores minimaps with max size 100x100
    MostRecentlyUsedImageCache m_tiny_minimap_cache;

    /// this caches MapInfo to facilitate GetMapExAsync
    MostRecentlyUsedMapInfoCache m_mapinfo_cache;

    MostRecentlyUsedArrayStringCache m_sides_cache;

    //! this function returns only the cache path without the file extension,
    //! the extension itself would be added in the function as needed
    wxString GetFileCachePath( const wxString& name, const wxString& hash, bool IsMod );

    //! returns an array where each element is a line of the file
	wxArrayString GetCacheFile( const wxString& path ) const;
    //! write a file where each element of the array is a line
    void SetCacheFile( const wxString& path, const wxArrayString& data );

    bool _LoadUnitSyncLib( const wxString& unitsyncloc );
    void _FreeUnitSyncLib();

    MapInfo _GetMapInfoEx( const wxString& mapname );

    void PopulateArchiveList();

    wxImage _GetMapImage( const wxString& mapname, const wxString& imagename, wxImage (SpringUnitSyncLib::*loadMethod)(const wxString&) );
    wxImage _GetScaledMapImage( const wxString& mapname, wxImage (SpringUnitSync::*loadMethod)(const wxString&), int width, int height );

    void _GetMapImageAsync( const wxString& mapname, wxImage (SpringUnitSync::*loadMethod)(const wxString&), int evtHandlerId );

public:
	wxString GetNameForShortname( const wxString& shortname, const wxString& version ) const;
};

SpringUnitSync& usync();

struct GameOptions
{
  SpringUnitSync::OptionMapBool bool_map;
  SpringUnitSync::OptionMapFloat float_map;
  SpringUnitSync::OptionMapString string_map;
  SpringUnitSync::OptionMapList list_map;
  SpringUnitSync::OptionMapSection section_map;
};

/// Helper class for managing async operations safely
class UnitSyncAsyncOps
{
  public:
	UnitSyncAsyncOps( wxEvtHandler* evtHandler )
		: m_id( usync().RegisterEvtHandler( evtHandler ) )
	{}
	~UnitSyncAsyncOps() {
	  usync().UnregisterEvtHandler( m_id );
	}

	void GetMinimap( const wxString& mapname )                 { usync().GetMinimapAsync( mapname, m_id ); }
	void GetMinimap( const wxString& mapname, int w, int h )   { usync().GetMinimapAsync( mapname, w, h, m_id ); }
	void GetMetalmap( const wxString& mapname )                { usync().GetMetalmapAsync( mapname, m_id ); }
	void GetMetalmap( const wxString& mapname, int w, int h )  { usync().GetMetalmapAsync( mapname, w, h, m_id ); }
	void GetHeightmap( const wxString& mapname )               { usync().GetHeightmapAsync( mapname, m_id ); }
	void GetHeightmap( const wxString& mapname, int w, int h ) { usync().GetHeightmapAsync( mapname, w, h, m_id ); }
	void GetMapEx( const wxString& mapname )                   { usync().GetMapExAsync( mapname, m_id ); }

  private:
	int m_id;
};


#endif // SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNC_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

