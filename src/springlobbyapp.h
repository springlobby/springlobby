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

	virtual bool OnInit();
	virtual int OnExit();

	virtual void OnFatalException();

	// System Events
	bool SelectLanguage();

	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

	void OnQuit(wxCommandEvent& data);

private:
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
