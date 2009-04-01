#ifndef SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <map>

#include "mmoptionmodel.h"

class wxImage;

extern const wxEventType UnitSyncAsyncOperationCompletedEvt;

struct UnitSyncMod
{
  UnitSyncMod() : name(_T("")),hash(_T("")) { }
  wxString name;
  wxString hash;
};

struct StartPos
{
  int x;
  int y;
};

struct MapInfo
{
  wxString description;
  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;
  int posCount;
  StartPos positions[16];

  wxString author;
};

struct UnitSyncMap
{
  UnitSyncMap() : name(_T("")),hash(_T("")) { }
  wxString name;
  wxString hash;
  MapInfo info;
};

struct GameOptions;

 /** UnitSync interface definition.
 */
class IUnitSync
{
  public:
    virtual ~IUnitSync() { }

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

    /** @name Mods
     *@{
     */
    /** Fetch the number of mods available.
     */
    virtual int GetNumMods() = 0;

    /** Get a list of the mods available.
     */
    virtual wxArrayString GetModList() = 0;

    /** Check by name if a mod exists.
     */
    virtual bool ModExists( const wxString& modname ) = 0;

    /** Check by name and hash string if a mod exists.
     */
    virtual bool ModExists( const wxString& modname, const wxString& hash ) = 0;

    /** Check by hash string only if a mod exists.
     */
    virtual bool ModExistsCheckHash( const wxString& hash ) const = 0;

    /** Get a mod by name.
     */
    virtual UnitSyncMod GetMod( const wxString& modname ) = 0;

    /** Get a mod by index.
     */
    virtual UnitSyncMod GetMod( int index ) = 0;

    /** Fetch the index of a mod by name.
     */
    virtual int GetModIndex( const wxString& name ) = 0;

    /** Fetch the name of a mod archive by the mod index.
     */
    virtual wxString GetModArchive( int index ) = 0;

    /** Get the options for a mod by name.
     */
    virtual GameOptions GetModOptions( const wxString& name ) = 0;
    /**@}*/

    virtual wxArrayString GetModDeps( const wxString& name ) = 0;

    virtual int GetNumMaps() = 0;
    virtual wxArrayString GetMapList() = 0;
    virtual wxArrayString GetModValidMapList( const wxString& modname ) = 0;
    virtual bool MapExists( const wxString& mapname ) = 0;
    virtual bool MapExists( const wxString& mapname, const wxString& hash ) = 0;

    virtual UnitSyncMap GetMap( const wxString& mapname ) = 0;
    virtual UnitSyncMap GetMap( int index ) = 0;
    virtual UnitSyncMap GetMapEx( const wxString& mapname ) = 0;
    virtual UnitSyncMap GetMapEx( int index ) = 0;
    virtual wxString GetMapArchive( int index ) = 0;
    virtual GameOptions GetMapOptions( const wxString& name ) = 0;
    virtual wxArrayString GetMapDeps( const wxString& name ) = 0;

    virtual int GetMapIndex( const wxString& name ) = 0;
    virtual wxImage GetMinimap( const wxString& mapname ) = 0;
    virtual wxImage GetMinimap( const wxString& mapname, int width, int height ) = 0;
    virtual wxImage GetMetalmap( const wxString& mapname ) = 0;
    virtual wxImage GetMetalmap( const wxString& mapname, int width, int height ) = 0;
    virtual wxImage GetHeightmap( const wxString& mapname ) = 0;
    virtual wxImage GetHeightmap( const wxString& mapname, int width, int height ) = 0;

    virtual wxArrayString GetSides( const wxString& modname  ) = 0;
    virtual wxImage GetSidePicture( const wxString& modname, const wxString& SideName ) =0;

    virtual int GetNumUnits( const wxString& modname ) = 0;
    virtual wxArrayString GetUnitsList( const wxString& modname ) = 0;

    virtual bool LoadUnitSyncLib( const wxString& unitsyncloc ) = 0;
    virtual void FreeUnitSyncLib() = 0;

    virtual bool IsLoaded() = 0;

		virtual wxString GetDefaultNick() = 0;
		virtual void SetDefaultNick( const wxString& nick ) = 0;

    virtual wxString GetSpringVersion() = 0;
    virtual bool VersionSupports( GameFeature feature ) = 0;

    virtual wxArrayString GetAIList( const wxString& modname ) = 0;
    virtual wxArrayString GetAIInfos( int index ) = 0;

    virtual bool ReloadUnitSyncLib() = 0;

    virtual wxArrayString GetReplayList() = 0;

    virtual void SetSpringDataPath( const wxString& path ) = 0;

    virtual bool FileExists( const wxString& name ) = 0;

    virtual wxString GetArchivePath( const wxString& name ) = 0;

    virtual void PrefetchMap( const wxString& mapname ) = 0;

    virtual int RegisterEvtHandler( wxEvtHandler* evtHandler ) = 0;
    virtual void UnregisterEvtHandler( int evtHandlerId ) = 0;

    virtual void GetMinimapAsync( const wxString& mapname, int evtHandlerId ) = 0;
    virtual void GetMinimapAsync( const wxString& mapname, int width, int height, int evtHandlerId ) = 0;
    virtual void GetMetalmapAsync( const wxString& mapname, int evtHandlerId ) = 0;
    virtual void GetMetalmapAsync( const wxString& mapname, int width, int height, int evtHandlerId ) = 0;
    virtual void GetHeightmapAsync( const wxString& mapname, int evtHandlerId ) = 0;
    virtual void GetHeightmapAsync( const wxString& mapname, int width, int height, int evtHandlerId ) = 0;
    virtual void GetMapExAsync( const wxString& mapname, int evtHandlerId ) = 0;
};

IUnitSync& usync();

struct GameOptions
{
  IUnitSync::OptionMapBool bool_map;
  IUnitSync::OptionMapFloat float_map;
  IUnitSync::OptionMapString string_map;
  IUnitSync::OptionMapList list_map;
  IUnitSync::OptionMapSection section_map;
};

/// Helper class for managing async operations safely
class UnitSyncAsyncOps
{
  public:
    UnitSyncAsyncOps( wxEvtHandler* evtHandler ) {
      m_id = usync().RegisterEvtHandler( evtHandler );
    }
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

#endif // SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

