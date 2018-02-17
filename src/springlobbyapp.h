/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class wxIcon;
class wxLocale;
class wxTranslationHelper;

//! @brief SpringLobby wxApp
class SpringLobbyApp : public wxApp
{
public:
	SpringLobbyApp();

	bool OnInit() override;
	int OnExit() override;

	void OnFatalException() override;

	// System Events
	bool SelectLanguage();

	void OnInitCmdLine(wxCmdLineParser& parser) override;
	bool OnCmdLineParsed(wxCmdLineParser& parser) override;


private:
	void OnQuit(wxCommandEvent& data);

	bool quit_called;

	wxTranslationHelper* m_translationhelper;

	long m_log_verbosity;
	bool m_log_console;
	bool m_log_window_show;
	bool m_crash_handle_disable;
	wxString m_appname;

	DECLARE_EVENT_TABLE()
};

DECLARE_APP(SpringLobbyApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
