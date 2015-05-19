/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "infodialog.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/app.h>
#include <wx/version.h>
#include <vector>
#include <fstream>
#include <utility>
#include <wx/filename.h>

#include <lslutils/misc.h>

#include "settings.h"
#include "utils/conversion.h"
#include "utils/version.h"
#include "utils/platform.h"
#include "utils/slpaths.h"

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#define WRITABLE W_OK
#elif defined(__WIN32__) || defined(_MSC_VER)
#include <io.h>
#define WRITABLE 02
#else
#error "os not supported"
#endif

inline wxString BtS(bool q, std::string y = "yes", std::string n = "no")
{
	return q ? TowxString(y) : TowxString(n);
}

InfoDialog::InfoDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("Paths"), wxDefaultPosition, wxSize(620, 400), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	typedef std::vector<std::pair<std::string, wxString> > Paths;
	Paths paths;
	paths.push_back(std::make_pair(SlPaths::GetLobbyWriteDir(), _T("LobbyWriteDir")));
	paths.push_back(std::make_pair(SlPaths::GetCachePath(), _T("CachePath")));
	paths.push_back(std::make_pair(SlPaths::GetExecutableFolder(), _T("ExecutableFolder")));
	paths.push_back(std::make_pair(SlPaths::GetDownloadDir(), _T("DownloadDir")));
	paths.push_back(std::make_pair(SlPaths::GetDataDir(), _T("Current SpringData:")));

	wxTextCtrl* out = new wxTextCtrl(this, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize,
					 wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL);

	*out << TowxString(getSpringlobbyAgent()) + _T("\n");
	*out << wxString::Format(_T("SpringLobby config file: %s (%s writable)\n"),
				 TowxString(SlPaths::GetConfigPath()).c_str(),
				 BtS(wxFileName::IsFileWritable(TowxString(SlPaths::GetConfigPath())), "", "not").c_str());

	for (size_t i = 0; i < paths.size(); ++i) {
		const wxString path = TowxString(paths[i].first);
		*out << wxString::Format(_T("%s (%s)\n"), paths[i].second.c_str(), path.c_str());
		wxString dummy_fn = path + wxFileName::GetPathSeparator() + _T("dummy.txt");
		const bool wx = wxFileName::IsDirWritable(path);
		const bool posix = access(STD_STRING(path).c_str(), WRITABLE) == 0;
		bool tried = false;
		try {
			std::ofstream of;
			of.open(STD_STRING(dummy_fn).c_str());

			if (of.is_open()) {
				of << "fhreuohgeiuhguie";
				of.flush();
				of.close();
				tried = wxRemoveFile(dummy_fn);
			}
		} catch (...) {
		}
		*out << wxString::Format(_T("\tWX: %s POSIX: %s TRY: %s\n"), BtS(wx).c_str(), BtS(posix).c_str(), BtS(tried).c_str());
	}

	*out << wxString::Format(_T("Current unitsync: %s\n"), TowxString(SlPaths::GetUnitSync()).c_str());
	*out << wxString::Format(_T("Current spring executable: %s\n"), TowxString(SlPaths::GetSpringBinary()).c_str());
	*out << wxString::Format(_T("Current uikeys.txt: %s\n"), TowxString(SlPaths::GetUikeys()).c_str());

	*out << wxString::Format(_T("Portable mode: %s\n"), BtS(SlPaths::IsPortableMode()).c_str());

	*out << wxString::Format(_T("Compiled with wxWidgets %d.%d.%d.%d"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, wxSUBRELEASE_NUMBER) + _T("\n");
	*out << _T("Started with: \n");
	for (int i = 0; i < wxTheApp->argc; ++i)
		*out << wxTheApp->argv[i] << _T(" ");
	m_main_sizer->Add(out, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(m_main_sizer);
	Layout();
}

InfoDialog::~InfoDialog()
{
}

