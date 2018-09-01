/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>
//! @brief SpringLobby wxApp
class UpdaterApp : public wxApp
{
public:
	UpdaterApp();

private:
	virtual bool OnInit();
	virtual int OnRun();

	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	bool StartUpdate(const wxString& source, const wxString& destination, bool silent);

	wxString m_source_dir;
	wxString m_destination_dir;
	wxString m_springlobby_exe;
	wxString m_updater_exe;
	long m_pid;
	long m_paramcount;
};

DECLARE_APP(UpdaterApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
