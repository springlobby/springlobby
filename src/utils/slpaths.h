#ifndef SPRINGLOBBY_SLPATHS_H
#define SPRINGLOBBY_SLPATHS_H

#include <map>
#include <list>
#include <cstddef>
#include <wx/filefn.h>

#include <lslunitsync/springbundle.h>
#include <lslunitsync/unitsync.h>

//! Interface for accessing paths for different files
// like configuration, logs ...
//
// the functions shall return the correct path taking
// IsPortableMode() into account
class SlPaths
{
public:
	//! used for passing config file at command line, empty if not set
	static wxString m_user_defined_config_path;
	static void SetUserConfigPath(const wxString& path);
	static bool IsPortableMode();
	static wxString GetLocalConfigPath();
	static wxString GetDefaultConfigPath();
	static wxString GetConfigPath();

	//! directory used to cache infomation about maps
	static wxString GetCachePath();

	/* ================================================================ */
	/** @name Spring locations
	 * @{
	 */

	static void RefreshSpringVersionList(bool autosearch=true, const LSL::SpringBundle* additionalbundle = NULL);
	static std::map<wxString, LSL::SpringBundle> GetSpringVersionList(); /// index -> version
	static wxString GetCurrentUsedSpringIndex();
	static void SetUsedSpringIndex(const wxString &index );
	static void DeleteSpringVersionbyIndex( const wxString& index );

	/// convenience wrappers to get current used version paths
	static wxString GetCurrentUsedDataDir();
	static wxString GetCurrentUsedUnitSync();
	static wxString GetCurrentUsedSpringBinary();
	//!@brief returns config file path unitsync uses, returns empty if unitsync isn't loaded
	static wxString GetCurrentUsedSpringConfigFilePath();

	static wxString GetUnitSync( const wxString& index );
	static wxString GetSpringBinary( const wxString& index );

	static void SetUnitSync( const wxString& index, const wxString& path );
	static void SetSpringBinary( const wxString& index, const wxString& path );
	//!@brief meaningful only on mac
	static void SetBundle( const wxString& index, const wxString& path );

	static wxString AutoFindSpringBin();
	static wxString AutoFindUnitSync( );
	static bool LocateSystemInstalledSpring(LSL::SpringBundle& bundle);

	/*@}*/

	static wxString GetChatLogLoc();

	static wxString GetEditorPath();
	static void SetEditorPath( const wxString& path );

	static wxString GetLobbyWriteDir();

	static wxString GetUikeys( const wxString& index );

	static wxString AutoFindUikeys();
	static wxString GetCurrentUsedUikeys();
	static bool SetupUserFolders(const wxString& dir);

private:
	//recursive create dir
	static bool mkDir(const wxString& dir);
	static bool IsSpringBin( const wxString& path );
	static std::map<wxString, LSL::SpringBundle> m_spring_versions;
};

#endif // SPRINGLOBBY_PATHLISTFACTORY_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2014

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


