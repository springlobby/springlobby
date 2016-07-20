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
typedef std::vector<std::pair<std::string, std::string> > Paths;

static void getWritePaths(Paths& paths)
{
	paths.push_back(std::make_pair(SlPaths::GetLobbyWriteDir(), "LobbyWriteDir"));
	paths.push_back(std::make_pair(SlPaths::GetCachePath(), "CachePath"));
	paths.push_back(std::make_pair(SlPaths::GetExecutableFolder(), "ExecutableFolder"));
	paths.push_back(std::make_pair(SlPaths::GetDownloadDir(), "DownloadDir"));
	paths.push_back(std::make_pair(SlPaths::GetDataDir(), "Current SpringData:"));
}

std::string GetSpringlobbyInfo()
{
	static const std::string nl = std::string("\n");
	std::string res;
	res  = getSpringlobbyAgent() + nl;
	const bool configwriteable = wxFileName::IsFileWritable(TowxString(SlPaths::GetConfigPath()));
	res += stdprintf("SpringLobby config file: %s (%swritable)\n",
				 SlPaths::GetConfigPath().c_str(),
				 BtS(configwriteable, "", "not ").c_str());
	Paths paths;
	getWritePaths(paths);
	for (size_t i = 0; i < paths.size(); ++i) {
		const std::string path = paths[i].first;
		res += stdprintf("%s (%s)\n", paths[i].second.c_str(), path.c_str());
		const bool wx = wxFileName::IsDirWritable(path);
		const bool posix = access(path.c_str(), WRITABLE) == 0;
		bool tried = false;
		try {
			std::ofstream of;
			const wxString dummy_fn = path + wxFileName::GetPathSeparator() + _T("dummy.txt");
			of.open(STD_STRING(dummy_fn).c_str());

			if (of.is_open()) {
				of << "fhreuohgeiuhguie";
				of.flush();
				of.close();
				tried = wxRemoveFile(dummy_fn);
			}
		} catch (...) {
		}
		if (!wx || !posix | !tried){
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

