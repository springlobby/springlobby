/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class UpdaterMainwindow;

//! @brief SpringLobby wxApp
class UpdaterApp : public wxApp
{
  public:
    UpdaterApp();
    ~UpdaterApp();

    virtual bool OnInit();
    virtual int OnExit();

    virtual void OnFatalException();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);


  protected:

	wxString m_exe_to_update;
	wxString m_version;

    UpdaterMainwindow* m_updater_window;

private:
	std::ofstream* m_logstream_target;


};

DECLARE_APP(UpdaterApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
