/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "slpaths.h"

#include <wx/string.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/log.h>

#include <lslunitsync/unitsync.h>
#include <lslutils/config.h>
#include <lslutils/misc.h>

#include "nonportable.h"
#include "helper/slconfig.h"

#include "platform.h"
#include "conversion.h"
#include "utils/version.h"
#include "log.h"



#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#endif

std::string SlPaths::m_user_defined_config_path = "";


std::string SlPaths::GetLocalConfigPath ()
{
	return GetExecutableFolder() + getSpringlobbyName(true) + ".conf";
}

std::string SlPaths::GetDefaultConfigPath ()
{
	return GetConfigfileDir() + getSpringlobbyName(true) + ".conf";
}

bool SlPaths::IsPortableMode ()
{
	if (!m_user_defined_config_path.empty()) {
		return false;
	}

	return LSL::Util::FileExists(GetLocalConfigPath());
}

//returns the path to springlobby.conf
std::string SlPaths::GetConfigPath ()
{
	if (!m_user_defined_config_path.empty()) {
		return m_user_defined_config_path;
	}
	return IsPortableMode() ?
		   GetLocalConfigPath() : GetDefaultConfigPath();
}

//returns the lobby cache path
std::string SlPaths::GetCachePath()
{
	const std::string path = LSL::Util::EnsureDelimiter(GetLobbyWriteDir() + "cache");
	if ( !wxFileName::DirExists(TowxString(path)) ) {
		if (!mkDir(path)) return "";
	}
	return path;
}

// ========================================================
std::map<std::string, LSL::SpringBundle> SlPaths::m_spring_versions;

std::map<std::string, LSL::SpringBundle> SlPaths::GetSpringVersionList()
{
	return m_spring_versions;
}

// adds path to pathlist if it exists
static void AddPath(const std::string& path, LSL::StringVector& pathlist)
{
	if (LSL::Util::FileExists(path)) {
		pathlist.push_back(path);
	}
}


//reads envvar, splits it by : and ; and add it to pathlist, when exists
void GetEnv(const std::string& name, LSL::StringVector& pathlist)
{
	const char* envvar= getenv(name.c_str());
	if (envvar == NULL) return;
	LSL::StringVector res = LSL::Util::StringTokenize(envvar, ";:");
	for (const std::string path:res) {
		AddPath(path, pathlist);
	}
}


// searches in OS standard paths for a system installed spring
bool SlPaths::LocateSystemInstalledSpring(LSL::SpringBundle& bundle)
{
	LSL::StringVector paths;

	GetEnv("SPRING_BUNDLE_DIR", paths);
	GetEnv("PATH", paths);
	GetEnv("%ProgramFiles%", paths);
	GetEnv("%ProgramFiles(x86)%", paths);
	GetEnv("%ProgramFiles(x86)%", paths);
	GetEnv("LD_LIBRARY_PATH", paths);
	GetEnv("LDPATH", paths);

	AddPath("/usr/local/lib/spring", paths);
	AddPath("/usr/local/lib64", paths);
	AddPath("/usr/local/games", paths);
	AddPath("/usr/local/games/lib", paths);
	AddPath("/usr/local/lib", paths);
	AddPath("/usr/lib64", paths);
	AddPath("/usr/lib", paths);
	AddPath("/usr/lib/spring", paths);
	AddPath("/usr/games", paths);
	AddPath("/usr/games/lib64", paths);
	AddPath("/usr/games/lib", paths);
	AddPath("/lib", paths);
	AddPath("/bin", paths);

	for (const std::string path: paths) {
		if (bundle.AutoComplete(path)) {
			return true;
		}
	}
	return false;
}


// returns my documents dir on windows, HOME on windows
static std::string GetMyDocumentsDir()
{
#ifdef WIN32
	char my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	if (result == S_OK) return std::string(my_documents);
	return "";
#else
	const char* envvar= getenv("HOME");
	if (envvar == NULL) return "";
	return std::string(envvar);
#endif
}

//returns possible paths where a spring bundles could be
void SlPaths::PossibleEnginePaths(LSL::StringVector& pl)
{
	pl.push_back(STD_STRING(wxFileName::GetCwd())); //current working directory
	pl.push_back(GetExecutableFolder()); //dir of springlobby.exe

	std::vector<std::string> basedirs;
	basedirs.push_back(GetDownloadDir());
	EngineSubPaths(basedirs, pl);
}

/* get all possible subpaths of basedirs with installed engines
* @param basdirs dirs in which to engines possible could be installed
*        basicly it returns the output of ls <basedirs>/engine/
* @param paths list of all paths found
*/
void SlPaths::EngineSubPaths(const LSL::StringVector& basedirs, LSL::StringVector& paths)
{
	for (const std::string basedir: basedirs) {
		const std::string enginedir = LSL::Util::EnsureDelimiter(LSL::Util::EnsureDelimiter(basedir)+ "engine");
		wxDir dir(TowxString(enginedir));

		if ( dir.IsOpened() ) {
			wxString filename;
			bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS|wxDIR_HIDDEN);
			while ( cont ) {
				paths.push_back(enginedir + STD_STRING(filename));
				cont = dir.GetNext(&filename);
			}
		}
	}
}

void SlPaths::RefreshSpringVersionList(bool autosearch, const LSL::SpringBundle* additionalbundle)
{
	/*
	FIXME: move to LSL's GetSpringVersionList() which does:

		1. search system installed spring + unitsync (both paths independant)
		2. search for user installed spring + unitsync (assume bundled)
		3. search / validate paths from config
		4. search path / unitsync given from user input

	needs to change to sth like: GetSpringVersionList(std::list<LSL::Bundle>)

	*/
	slLogDebugFunc("");
	std::list<LSL::SpringBundle> usync_paths;

	if (additionalbundle != NULL) {
		usync_paths.push_back(*additionalbundle);
	}

	if (autosearch) {
		std::vector<std::string> lobbysubpaths;
		std::vector<std::string> basedirs;
		basedirs.push_back(GetLobbyWriteDir());
		EngineSubPaths(basedirs, lobbysubpaths);
		for (const std::string path: lobbysubpaths) {
			LSL::SpringBundle bundle;
			bundle.path = path;
			usync_paths.push_back(bundle);
		}
		if (!SlPaths::IsPortableMode()) {
			LSL::SpringBundle systembundle;
			if (LocateSystemInstalledSpring(systembundle)) {
				usync_paths.push_back(systembundle);
			}

			std::vector<std::string> paths;
			PossibleEnginePaths(paths);
			for (const std::string path: paths) {
				LSL::SpringBundle bundle;
				bundle.path = path;
				usync_paths.push_back(bundle);
			}
		}
	}

	wxArrayString list = cfg().GetGroupList( _T( "/Spring/Paths" ) );
	const int count = list.GetCount();
	for ( int i = 0; i < count; i++ ) {
		LSL::SpringBundle bundle;
		const std::string configsection = STD_STRING(list[i]);
		bundle.unitsync = GetUnitSync(configsection);
		bundle.spring = GetSpringBinary(configsection);
		bundle.version = configsection;
		usync_paths.push_back(bundle);
	}

	cfg().DeleteGroup(_T("/Spring/Paths"));

	m_spring_versions.clear();
	try {
		const auto versions = LSL::usync().GetSpringVersionList( usync_paths );
		for(const auto pair : versions) {
			const LSL::SpringBundle& bundle = pair.second;
			const std::string version = bundle.version;
			m_spring_versions[version] = bundle;
			SetSpringBinary(version, bundle.spring);
			SetUnitSync(version, bundle.unitsync);
			SetBundle(version, bundle.path);
		}
	} catch (const std::runtime_error& e) {
		wxLogError(wxString::Format(_T("Couldn't get list of spring versions: %s"), e.what()));
	} catch ( ...) {
		wxLogError(_T("Unknown Execption caught in SlPaths::RefreshSpringVersionList"));
	}
}

std::string SlPaths::GetCurrentUsedSpringIndex()
{
	wxString index = wxEmptyString;
	cfg().Read(_T("/Spring/CurrentIndex"), &index);
	if(index.empty()) {
		// if no version set yet, select one, if available
		for(auto pair: m_spring_versions) {
			return pair.first;
		}
	}
	return STD_STRING(index);
}

void SlPaths::SetUsedSpringIndex( const std::string& index )
{
	cfg().Write( _T( "/Spring/CurrentIndex" ), TowxString(index) );
	ReconfigureUnitsync();
}

void SlPaths::ReconfigureUnitsync()
{
	LSL::Util::config().ConfigurePaths(
		SlPaths::GetCachePath(),
		SlPaths::GetUnitSync(),
		SlPaths::GetSpringBinary()
	);
}


void SlPaths::DeleteSpringVersionbyIndex( const std::string& index )
{
	cfg().DeleteGroup( _T( "/Spring/Paths/" ) + TowxString(index) );
	if ( GetCurrentUsedSpringIndex() == index ) SetUsedSpringIndex("");
}


std::string SlPaths::GetSpringConfigFilePath(const std::string& /*FIXME: implement index*/)
{
	std::string path;
	try {
		path = LSL::usync().GetConfigFilePath();
	} catch ( std::runtime_error& e) {
		wxLogError( wxString::Format( _T("Couldn't get SpringConfigFilePath, exception caught:\n %s"), e.what()  ) );
	}
	return path;
}

std::string SlPaths::GetUnitSync( const std::string& index )
{
	return STD_STRING(cfg().Read( _T( "/Spring/Paths/" ) + TowxString(index) + _T( "/UnitSyncPath" ), wxEmptyString ));
}

std::string SlPaths::GetSpringBinary( const std::string& index )
{
	return STD_STRING(cfg().Read( _T( "/Spring/Paths/" ) + TowxString(index) + _T( "/SpringBinPath" ), wxEmptyString ));
}

void SlPaths::SetUnitSync( const std::string& index, const std::string& path )
{
	cfg().Write( _T( "/Spring/Paths/" ) + TowxString(index) + _T( "/UnitSyncPath" ), TowxString(path));
	// reconfigure unitsync in case it is reloaded
	ReconfigureUnitsync();
}

void SlPaths::SetSpringBinary( const std::string& index, const std::string& path )
{
	cfg().Write( _T( "/Spring/Paths/" ) + TowxString(index) + _T( "/SpringBinPath" ), TowxString(path) );
}

void SlPaths::SetBundle( const std::string& index, const std::string& path )
{
	cfg().Write( _T( "/Spring/Paths/" ) + TowxString(index) + _T( "/SpringBundlePath" ), TowxString(path));
}

std::string SlPaths::GetChatLogLoc()
{
	const std::string path = GetLobbyWriteDir() + "chatlog";
	if ( !LSL::Util::FileExists(path) ) {
		if (!mkDir(path)) return "";
	}
	return LSL::Util::EnsureDelimiter(path);
}


bool SlPaths::IsSpringBin( const std::string& path )
{
	if ( !LSL::Util::FileExists(path) ) return false;
	if ( !wxFileName::IsFileExecutable(TowxString(path)) ) return false;
	return true;
}

std::string SlPaths::GetEditorPath( )
{
	wxString def = wxEmptyString;
#if defined(__WXMSW__)
	const wxChar sep = wxFileName::GetPathSeparator();
	const wxString sepstring = wxString(sep);
	def = wxGetOSDirectory() + sepstring + _T("system32") + sepstring + _T("notepad.exe");
	if ( !wxFile::Exists( def ) )
		def = wxEmptyString;
#endif
	return STD_STRING(cfg().Read( _T( "/GUI/Editor" ) , def ));
}

void SlPaths::SetEditorPath( const std::string& path )
{
	cfg().Write( _T( "/GUI/Editor" ) , TowxString(path));
}


std::string SlPaths::GetLobbyWriteDir()
{
	//FIXME: make configureable
	if (IsPortableMode()) {
		return LSL::Util::EnsureDelimiter(GetExecutableFolder());
	}
	return LSL::Util::EnsureDelimiter(GetConfigfileDir());
}

std::string SlPaths::GetUikeys(const std::string& index)
{
	return GetDataDir(index) + "uikeys.txt";
}

bool SlPaths::mkDir(const std::string& dir) {
	return wxFileName::Mkdir(TowxString(dir), 0755, wxPATH_MKDIR_FULL);
}

bool SlPaths::CreateSpringDataDir(const std::string& dir)
{
	if ( dir.empty() ) {
		return false;
	}
	const std::string directory = LSL::Util::EnsureDelimiter(dir);
	if ( !mkDir(directory) ||
			!mkDir(directory + "games") ||
			!mkDir(directory + "maps") ||
			!mkDir(directory + "demos") ||
			!mkDir(directory + "screenshots")) {
		return false;
	}
	LSL::usync().SetSpringDataPath(dir);
	return true;
}

std::string SlPaths::GetDataDir(const std::string& /*FIXME: implement index */)
{
	std::string dir;
	if (!LSL::usync().GetSpringDataPath(dir))
		return "";
	return LSL::Util::EnsureDelimiter(dir);
}

std::string VersionGetMajor(const std::string& version)
{
	const int pos = version.find(".");
	if (pos>0) {
		return version.substr(0, pos);
	}
	return version;
}

bool VersionIsRelease(const std::string& version) { //try to detect if a version is major
	const std::string allowedChars = "01234567890.";
	for(size_t i=0; i<version.length(); i++) {
		if (allowedChars.find(version[i]) == std::string::npos) { //found invalid char -> not stable version
			return false;
		}
	}
	return true;
}

bool VersionSyncCompatible(const std::string& ver1, const std::string& ver2)
{
	if (ver1 == ver2) {
		return true;
	}
	if ( (VersionIsRelease(ver1) &&  VersionIsRelease(ver2)) &&
	 (VersionGetMajor(ver1) == VersionGetMajor(ver2))) {
		return true;
	}
	return false;
}

std::string SlPaths::GetCompatibleVersion(const std::string& neededversion)
{
	const auto versionlist = SlPaths::GetSpringVersionList();
	for ( const auto pair : versionlist ) {
		const std::string ver = pair.first;
		const LSL::SpringBundle bundle = pair.second;
		if ( VersionSyncCompatible(neededversion, ver)) {
			return ver;
		}
	}
	return "";
}

std::string SlPaths::GetExecutable()
{
	return STD_STRING(wxStandardPathsBase::Get().GetExecutablePath());
}

std::string SlPaths::GetExecutableFolder()
{
	return LSL::Util::EnsureDelimiter(LSL::Util::ParentPath(GetExecutable()));
}

/**
	returns %APPDATA%\springlobby on windows, $HOME/.spring on linux
*/
std::string SlPaths::GetConfigfileDir()
{
	std::string path = LSL::Util::EnsureDelimiter(STD_STRING(wxStandardPaths::Get().GetUserConfigDir()));
#ifndef WIN32
	path += ".";
#endif
	path += getSpringlobbyName(true);
	return LSL::Util::EnsureDelimiter(path);
}

std::string SlPaths::GetDownloadDir()
{
#ifdef WIN32
	const std::string dir = LSL::Util::EnsureDelimiter(GetMyDocumentsDir()) +"My Games" + SEP + "Spring";
#else
	const std::string dir = GetMyDocumentsDir() + ".spring";
#endif
	wxString downloadDir = TowxString(dir);
	cfg().Read(_T("/Spring/DownloadDir"), &downloadDir);
	return LSL::Util::EnsureDelimiter(STD_STRING(downloadDir));
}

std::string SlPaths::GetUpdateDir()
{
	return SlPaths::GetLobbyWriteDir() + "update" + SEP;
}
