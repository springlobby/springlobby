/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <string>
#include <vector>
#include <fstream>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/filename.h>
#include <wx/string.h>
#include "utils/conversion.h"
#include "utils/slpaths.h"
#include "utils/version.h"

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#define WRITABLE W_OK
#elif defined(__WIN32__) || defined(_MSC_VER)
#include <io.h>
#define WRITABLE 02
#else
#error "os not supported"
#endif


inline std::string BtS(bool q, const std::string& yes = "yes", const std::string& no = "no")
{
	return q ? yes : no;
}

class PathInfo
{
public:
	PathInfo(const std::string& path, const std::string description, bool requireswrite)
	    : m_path(path)
	    , m_desc(description)
	    , m_requireswrite(requireswrite)
	{
	}

	std::string m_path;
	std::string m_desc;
	bool m_requireswrite;
};

typedef std::vector<PathInfo> Paths;

static void getWritePaths(Paths& paths)
{
	paths.push_back(PathInfo(SlPaths::GetLobbyWriteDir(), "LobbyWriteDir", true));
	paths.push_back(PathInfo(SlPaths::GetCachePath(), "CachePath", true));
	paths.push_back(PathInfo(SlPaths::GetExecutableFolder(), "ExecutableFolder", false));
	paths.push_back(PathInfo(SlPaths::GetDownloadDir(), "DownloadDir", true));
	paths.push_back(PathInfo(SlPaths::GetDataDir(), "Current SpringData:", true));
}

//FIXME: merge with basicly duplicate in slpaths.cpp
std::string GetSpringlobbyInfo()
{
	static const std::string nl = std::string("\n");
	std::string res;
	res = GetSpringlobbyAgent() + nl;
	const bool configwriteable = wxFileName::IsFileWritable(TowxString(SlPaths::GetConfigPath()));
	res += stdprintf("Configuration file: %s (%swritable)\n",
			 SlPaths::GetConfigPath().c_str(),
			 BtS(configwriteable, "", "not ").c_str());
	Paths paths;
	getWritePaths(paths);
	for (size_t i = 0; i < paths.size(); ++i) {
		std::string path = paths[i].m_path;

		if (path.empty()) {
			continue;
		}
		
#if defined(__WIN32__) || defined(_MSC_VER)
		path = Utf8ToLocalEncoding(path.c_str());
#endif
		res += stdprintf("%s (%s)\n", paths[i].m_desc.c_str(), path.c_str());
		const bool wx = wxFileName::IsDirWritable(path);
		const bool posix = access(path.c_str(), WRITABLE) == 0;
		bool tried = false;
		try {
			std::ofstream of;
			wxString dummy_fn = paths[i].m_path;
			if (!wxEndsWithPathSeparator(dummy_fn)) {
				dummy_fn += wxFileName::GetPathSeparator();
			}
			dummy_fn += _T("dummy.txt");

			std::string dummyFileString = dummy_fn.ToStdString();
#if defined(__WIN32__) || defined(_MSC_VER)
			dummyFileString = Utf8ToLocalEncoding(dummyFileString.c_str());
#endif
			of.open(dummyFileString);

			if (of.is_open()) {
				of << "fhreuohgeiuhguie";
				of.flush();
				of.close();
				tried = wxRemoveFile(dummyFileString);
			}
		} catch (...) {
		}
		if (paths[i].m_requireswrite && (!wx || !posix || !tried)) {
			wxLogError("%s is not writeable!", path.c_str());
		}
		res += stdprintf(("\tWX: %s POSIX: %s TRY: %s\n"), BtS(wx).c_str(), BtS(posix).c_str(), BtS(tried).c_str());
	}

	res += stdprintf("Current unitsync: %s\n", SlPaths::GetUnitSync().c_str());
	res += stdprintf("Current spring executable: %s\n", SlPaths::GetSpringBinary().c_str());
	res += stdprintf("Portable mode: %s\n", BtS(SlPaths::IsPortableMode()).c_str());

	res += stdprintf(("Compiled with wxWidgets %d.%d.%d.%d"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, wxSUBRELEASE_NUMBER) + nl;
	res += "Started with: \n";
	for (int i = 0; i < wxTheApp->argc; ++i)
		res += STD_STRING(wxTheApp->argv[i]) + std::string(" ");
	return res;
}
