/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_SLPATHS_H
#define SPRINGLOBBY_SLPATHS_H

#include <map>
#include <cstddef>
#include <wx/filefn.h>

#include <lslunitsync/springbundle.h>

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
	static std::string GetCachePath();

	/* ================================================================ */
	/** @name Spring locations
	 * @{
	 */

	static void RefreshSpringVersionList(bool autosearch=true, const LSL::SpringBundle* additionalbundle = NULL);
	static std::map<wxString, LSL::SpringBundle> GetSpringVersionList(); /// index -> version
	static bool LocateSystemInstalledSpring(LSL::SpringBundle& bundle);

	static wxString GetCurrentUsedSpringIndex();
	static void SetUsedSpringIndex(const wxString &index);

	static void ReconfigureUnitsync();

	static void DeleteSpringVersionbyIndex( const wxString& index = GetCurrentUsedSpringIndex() );

	static wxString GetUnitSync(const wxString& index = GetCurrentUsedSpringIndex());
	static void SetUnitSync(const wxString& path, const wxString& index = GetCurrentUsedSpringIndex());

	static wxString GetSpringBinary(const wxString& index = GetCurrentUsedSpringIndex() );
	static void SetSpringBinary(const wxString& path, const wxString& index = GetCurrentUsedSpringIndex());

	static wxString GetUikeys(const wxString& index = GetCurrentUsedSpringIndex());
	static wxString GetDataDir(const wxString& index = GetCurrentUsedSpringIndex());
	static wxString GetSpringConfigFilePath(const wxString& index = GetCurrentUsedSpringIndex());

	//!@brief meaningful only on mac
	static void SetBundle( const wxString& index, const wxString& path );


	/*@}*/

	static wxString GetChatLogLoc();

	static wxString GetEditorPath();
	static void SetEditorPath( const wxString& path );

	static std::string GetLobbyWriteDir();

	static bool CreateSpringDataDir(const wxString& dir);
	static std::string GetCompatibleVersion(const std::string& neededversion);

private:
	//recursive create dir
	static bool mkDir(const wxString& dir);
	static bool IsSpringBin( const wxString& path );
	static std::map<wxString, LSL::SpringBundle> m_spring_versions;
	static std::string EnsureDelimiter(const std::string& path);
};

#endif // SPRINGLOBBY_PATHLISTFACTORY_H
