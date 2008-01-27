#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class Ui;
class wxTimer;
class wxIcon;

//! @brief SpringLobby wxApp
class SpringLobbyApp : public wxApp
{
  public:
    SpringLobbyApp();
    ~SpringLobbyApp();

    virtual bool OnInit();
    virtual int OnExit();

    virtual void OnFatalException();

    // System Events
    void OnTimer( wxTimerEvent& event );

    void InitDirs();

    bool ParseCmdLine();

  protected:

    wxTimer* m_timer;

    Ui* m_ui;

    DECLARE_EVENT_TABLE()

    long m_log_verbosity;
    bool m_force_log_verbosity;
    bool m_log_console;
    bool m_log_window_show;
    bool m_crash_handle_disable;
};

DECLARE_APP(SpringLobbyApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
