/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_SLPATHS_H
#define SPRINGLOBBY_SLPATHS_H

#include <map>
#include <vector>
#include <cstddef>
#include <wx/filename.h>

#include <lslunitsync/springbundle.h>

//! Interface for accessing paths for different files
// like configuration, logs ...
//
// the functions shall return the correct path taking
// IsPortableMode() into account
class SlPaths
{
public:
	static void SetUserConfigPath(const std::string& path)
	{
		m_user_defined_config_path = path;
	}
	static bool IsPortableMode();
	static std::string GetLocalConfigPath();
	static std::string GetDefaultConfigPath();
	static std::string GetConfigPath();

	//! directory used to cache infomation about maps
	static std::string GetCachePath();

	/* ================================================================ */
	/** @name Spring locations
	 * @{
	 */

	static void RefreshSpringVersionList(bool autosearch = true, const LSL::SpringBundle* additionalbundle = NULL);
	static std::map<std::string, LSL::SpringBundle> GetSpringVersionList(); /// index -> version

	static std::string GetCurrentUsedSpringIndex();
	static void SetUsedSpringIndex(const std::string& index);

	static void ReconfigureUnitsync();

	static void DeleteSpringVersionbyIndex(const std::string& index = GetCurrentUsedSpringIndex());

	static std::string GetUnitSync(const std::string& index = GetCurrentUsedSpringIndex());
	static void SetUnitSync(const std::string& path, const std::string& index = GetCurrentUsedSpringIndex());

	static std::string GetSpringBinary(const std::string& index = GetCurrentUsedSpringIndex());
	static void SetSpringBinary(const std::string& path, const std::string& index = GetCurrentUsedSpringIndex());

	static std::string GetDataDir(const std::string& index = GetCurrentUsedSpringIndex());
	static std::string GetSpringConfigFilePath(const std::string& index = GetCurrentUsedSpringIndex());
	static std::string GetDownloadDir();
	static void SetDownloadDir(const std::string& newDir);

	/*Check all paths for write access*/
	static bool ValidatePaths();
	static bool CheckDirExistAndWritable(const std::string&);

	//!@brief meaningful only on mac
	static void SetBundle(const std::string& index, const std::string& path);


	/*@}*/

	static std::string GetExecutable();
	static std::string GetChatLogLoc();

	static std::string GetEditorPath();
	static void SetEditorPath(const std::string& path);

	static std::string GetLobbyWriteDir();

	static bool CreateSpringDataDir(const std::string& dir);
	static std::string GetCompatibleVersion(const std::string& neededversion);
	static std::string GetExecutableFolder();
	static std::string GetConfigfileDir();
	static std::string GetUpdateDir();
	static bool RmDir(const std::string& dir);
	//recursive create dir
	static bool mkDir(const std::string& dir);

	static std::string SantinizeFilename(const std::string& filename);

private:
	static void EngineSubPaths(const std::vector<std::string>& basedirs, std::vector<std::string>& paths);

	//! used for passing config file at command line, empty if not set
	static std::string m_user_defined_config_path;
	static bool IsSpringBin(const std::string& path);
	static std::map<std::string, LSL::SpringBundle> m_spring_versions;
	static void PossibleEnginePaths(std::vector<std::string>& pl);
};

#endif // SPRINGLOBBY_PATHLISTFACTORY_H
