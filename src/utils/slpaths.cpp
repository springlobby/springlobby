/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "slpaths.h"

#include "../nonportable.h"
#include "../helper/slconfig.h"

#include "platform.h"
#include "conversion.h"
#include "debug.h"
#include "utils/version.h"

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

#include <lslunitsync/unitsync.h>
#include <lslutils/config.h>

std::string SlPaths::m_user_defined_config_path = "";

const wxChar sep = wxFileName::GetPathSeparator();
const wxString sepstring = wxString(sep);
#ifdef WIN32
#define PATH_DELIMITER '\\'
#else
#define PATH_DELIMITER '/'
#endif


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
	return wxFileName::FileExists(TowxString(GetLocalConfigPath()));
}

std::string SlPaths::GetConfigPath ()
{
	if (!m_user_defined_config_path.empty()) {
		return m_user_defined_config_path;
	}
	return IsPortableMode() ?
		   GetLocalConfigPath() : GetDefaultConfigPath();
}

std::string SlPaths::GetCachePath()
{
	const std::string path = EnsureDelimiter(GetLobbyWriteDir() + "cache")																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							;
	if ( !wxFileName::DirExists(TowxString(path)) ) {
		if ( !wxFileName::Mkdir(TowxString(path), 0755  ) ) return "";
	}
	return path;
}

// ========================================================
std::map<std::string, LSL::SpringBundle> SlPaths::m_spring_versions;

std::map<std::string, LSL::SpringBundle> SlPaths::GetSpringVersionList()
{
	return m_spring_versions;
}

bool SlPaths::LocateSystemInstalledSpring(LSL::SpringBundle& bundle)
{
	wxPathList pl;
    pl.AddEnvList( _T( "%ProgramFiles%" ) );

    pl.AddEnvList( _T( "LDPATH" ) );
    pl.AddEnvList( _T( "LD_LIBRARY_PATH" ) );
    pl.AddEnvList( _T( "SPRING_BUNDLE_DIR" ) ); //folder which contains a bundle, for example {SPRING_BUNDLE_DIR]/spring {SPRING_BUNDLE_DIR}/libunitsync.so

    pl.Add( _T( "/usr/local/lib/spring" ) );
    pl.Add( _T( "/usr/local/lib64" ) );
    pl.Add( _T( "/usr/local/games" ) );
    pl.Add( _T( "/usr/local/games/lib" ) );
    pl.Add( _T( "/usr/local/lib" ) );
    pl.Add( _T( "/usr/lib64" ) );
    pl.Add( _T( "/usr/lib" ) );
    pl.Add( _T( "/usr/lib/spring" ) );
    pl.Add( _T( "/usr/games" ) );
    pl.Add( _T( "/usr/games/lib64" ) );
    pl.Add( _T( "/usr/games/lib" ) );

	for (const wxString path: pl) {
		if (bundle.AutoComplete(STD_STRING(path))) {
			return true;
		}
	}
	return false;
}

//returns possible paths where a spring bundles could be
void SlPaths::PossibleEnginePaths(wxPathList &pl)
{
    pl.Add( wxFileName::GetCwd() ); //current working directory
    pl.Add( TowxString(GetExecutableFolder())); //dir of springlobby.exe

	std::vector<std::string> basedirs, paths;
	const std::string homedir = EnsureDelimiter(STD_STRING(wxStandardPaths::Get().GetDocumentsDir()));
	basedirs.push_back(homedir + ".spring");
	basedirs.push_back(homedir + "My Games" + PATH_DELIMITER + "Spring" + PATH_DELIMITER);
	EngineSubPaths(basedirs, paths);

    for(auto path: paths) { //fill result
        pl.Add(TowxString(path));
	}
}

// get all possible subpaths of basedirs with installed engines
void SlPaths::EngineSubPaths(const std::vector<std::string>& basedirs, std::vector<std::string>& paths)
{
	for (auto basedir: basedirs) {
		const std::string enginedir = EnsureDelimiter(EnsureDelimiter(basedir)+ "engine");
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
	wxLogDebugFunc( wxEmptyString );
	std::list<LSL::SpringBundle> usync_paths;

	if (additionalbundle != NULL) {
		usync_paths.push_back(*additionalbundle);
	}

	if (autosearch) {
		std::vector<std::string> lobbysubpaths;
		std::vector<std::string> basedirs;
		basedirs.push_back(GetLobbyWriteDir());
		EngineSubPaths(basedirs, lobbysubpaths);
		for (const auto path: lobbysubpaths) {
			LSL::SpringBundle bundle;
			bundle.path = path;
			usync_paths.push_back(bundle);
		}
		if (!SlPaths::IsPortableMode()) {
			LSL::SpringBundle systembundle;
			if (LocateSystemInstalledSpring(systembundle)) {
				usync_paths.push_back(systembundle);
			}

			wxPathList paths;
			PossibleEnginePaths(paths);
			for (const auto path: paths) {
				LSL::SpringBundle bundle;
				bundle.path = STD_STRING(path);
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
	if ( !wxFileName::DirExists(TowxString(path)) ) {
		if ( !wxFileName::Mkdir(TowxString(path), 0755  ) ) return "";
	}
	return EnsureDelimiter(path);
}


bool SlPaths::IsSpringBin( const std::string& path )
{
	if ( !wxFile::Exists(TowxString(path)) ) return false;
	if ( !wxFileName::IsFileExecutable(TowxString(path)) ) return false;
	return true;
}

std::string SlPaths::GetEditorPath( )
{
	wxString def = wxEmptyString;
#if defined(__WXMSW__)
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
		return EnsureDelimiter(GetExecutableFolder());
	}
	return EnsureDelimiter(GetConfigfileDir());
}

std::string SlPaths::GetUikeys(const std::string& index)
{
	return GetDataDir(index) + "uikeys.txt";
}

bool SlPaths::mkDir(const std::string& dir) {
	return wxFileName::Mkdir(TowxString(dir), 0, wxPATH_MKDIR_FULL);
}

bool SlPaths::CreateSpringDataDir(const std::string& dir)
{
	if ( dir.empty() ) {
		return false;
	}
	const std::string directory = EnsureDelimiter(dir);
	if ( !mkDir(directory) ||
			!mkDir(directory + "games") ||
			!mkDir(directory + "maps") ||
			!mkDir(directory + "demos") ||
			!mkDir(directory + "screenshots")) {
		return false;
	}
	if (LSL::usync().IsLoaded()) {
		LSL::usync().SetSpringDataPath(dir);
	}
	return true;
}

std::string SlPaths::EnsureDelimiter(const std::string& path)
{
	std::string dir = path;
	if ( !path.empty() && (path[path.length()-1] != PATH_DELIMITER )) {
		dir += PATH_DELIMITER;
	}
	return dir;
}

std::string SlPaths::GetDataDir(const std::string& /*FIXME: implement index */)
{
	std::string dir;
	if (!LSL::usync().GetSpringDataPath(dir))
		return "";
	return EnsureDelimiter(dir);
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

std::string SlPaths::GetExecutableFolder()
{
	return EnsureDelimiter(STD_STRING(wxStandardPathsBase::Get().GetExecutablePath().BeforeLast( wxFileName::GetPathSeparator())));
}

std::string SlPaths::GetUserDataDir()
{
	return EnsureDelimiter(STD_STRING(wxStandardPaths::Get().GetUserConfigDir()));
}

std::string SlPaths::GetConfigfileDir()
{
	std::string path = GetUserDataDir();
#ifndef __WXMSW__
	path += ".";
#endif
	path += getSpringlobbyName(true);
	return EnsureDelimiter(path);
}
