#ifndef SPRINGLOBBY_HEADERGUARD_SpringUnitSync_H
#define SPRINGLOBBY_HEADERGUARD_SpringUnitSync_H

#include "springunitsync.h"

#if 0

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <map>



 /** UnitSync interface definition.
 */
class SpringUnitSync
{
  public:
		virtual void OnReload( wxCommandEvent& event ) = 0;


    /** @name Mods
     *@{
     */
    /** Fetch the number of mods available.
     */
	virtual int GetNumMods() const = 0;

    /** Get a list of the mods available.
     */
	virtual wxArrayString GetModList() const = 0;

    /** Check by name if a mod exists.
     */
	virtual bool ModExists( const wxString& modname ) const = 0;

    /** Check by name and hash string if a mod exists.
     */
	virtual bool ModExists( const wxString& modname, const wxString& hash ) const = 0;

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
	virtual int GetModIndex( const wxString& name ) const = 0;

    /** Get the options for a mod by name.
     */
    virtual GameOptions GetModOptions( const wxString& name ) = 0;
    /**@}*/

	virtual wxArrayString GetModDeps( const wxString& name ) const = 0;

	/** Un-loads current mod in unitsync.
	  */
	virtual void UnSetCurrentMod() = 0;

	virtual int GetNumMaps() const = 0;
	virtual wxArrayString GetMapList() const = 0;
	virtual wxArrayString GetModValidMapList( const wxString& modname ) const = 0;
	virtual bool MapExists( const wxString& mapname ) const = 0;
	virtual bool MapExists( const wxString& mapname, const wxString& hash ) const = 0;

	virtual UnitSyncMap GetMap( const wxString& mapname ) = 0;
	virtual UnitSyncMap GetMap( int index ) = 0;
    virtual UnitSyncMap GetMapEx( const wxString& mapname ) = 0;
    virtual UnitSyncMap GetMapEx( int index ) = 0;
    virtual GameOptions GetMapOptions( const wxString& name ) = 0;
    virtual wxArrayString GetMapDeps( const wxString& name ) = 0;

	virtual int GetMapIndex( const wxString& name ) const = 0;
    virtual wxImage GetMinimap( const wxString& mapname ) = 0;
    virtual wxImage GetMinimap( const wxString& mapname, int width, int height ) = 0;
    virtual wxImage GetMetalmap( const wxString& mapname ) = 0;
    virtual wxImage GetMetalmap( const wxString& mapname, int width, int height ) = 0;
    virtual wxImage GetHeightmap( const wxString& mapname ) = 0;
    virtual wxImage GetHeightmap( const wxString& mapname, int width, int height ) = 0;

	virtual wxArrayString GetSides( const wxString& modname  ) = 0;
	virtual wxImage GetSidePicture( const wxString& modname, const wxString& SideName )  const = 0;
	virtual wxImage GetImage( const wxString& modname, const wxString& image_path, bool useWhiteAsTransparent = true ) const = 0;
#ifdef SL_QT_MODE
	virtual QImage GetQImage( const wxString& modname, const wxString& image_path, bool useWhiteAsTransparent = true ) const = 0;
#endif
	virtual wxString GetTextfileAsString( const wxString& modname, const wxString& file_path ) =0;

	virtual int GetNumUnits( const wxString& modname ) const = 0;
	virtual wxArrayString GetUnitsList( const wxString& modname ) = 0;

    virtual bool LoadUnitSyncLib( const wxString& unitsyncloc ) = 0;
    virtual void FreeUnitSyncLib() = 0;

	virtual bool IsLoaded() const = 0;

    virtual wxString GetDefaultNick() = 0;
    virtual void SetDefaultNick( const wxString& nick ) = 0;

	virtual wxString GetSpringVersion() const = 0;
	virtual bool VersionSupports( GameFeature feature ) const = 0;

	virtual wxArrayString GetAIList( const wxString& modname ) const = 0;
	virtual wxArrayString GetAIInfos( int index ) const = 0;
    virtual GameOptions GetAIOptions( const wxString& modname, int index ) = 0;

    virtual bool ReloadUnitSyncLib(  ) = 0;
    virtual void ReloadUnitSyncLib( GlobalEvents::GlobalEventData data ) = 0;
	virtual bool FastLoadUnitSyncLib( const wxString& unitsyncloc ) = 0;
	virtual bool FastLoadUnitSyncLibInit() = 0;

	virtual wxArrayString GetPlaybackList( bool ReplayType = true ) const = 0; //savegames otherwise

    virtual void SetSpringDataPath( const wxString& path ) = 0;

	virtual bool FileExists( const wxString& name ) const = 0;

	virtual wxString GetArchivePath( const wxString& name ) const = 0;

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

	virtual wxArrayString GetScreenshotFilenames() const = 0;

    /** \param name Modname **/
    virtual GameOptions GetModCustomizations( const wxString& modname ) = 0;
    virtual GameOptions GetSkirmishOptions( const wxString& modname, const wxString& skirmish_name ) = 0;

	//! this only generates a wxUnitsyncReloadEvent type wxCommandEvent and appends it to itself
	virtual void AddReloadEvent() = 0;

	virtual wxArrayString FindFilesVFS( const wxString& pattern ) const = 0;

    private:
		SpringUnitSync( const SpringUnitSync& );

	public:
		virtual wxString GetNameForShortname( const wxString& shortname, const wxString& version ) const = 0;
};

#endif // 0

#endif // SPRINGLOBBY_HEADERGUARD_SpringUnitSync_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

