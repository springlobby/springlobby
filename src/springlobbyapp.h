#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class wxTimer;
class wxTimerEvent;
class wxIcon;
class wxLocale;
class wxTranslationHelper;

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
    bool SelectLanguage();

    bool ParseCmdLine();

  protected:

    void SetupUserFolders();
    void InitCacheDir();

    wxTimer* m_timer;

    wxTranslationHelper* m_translationhelper;

    DECLARE_EVENT_TABLE()

    long m_log_verbosity;
    bool m_log_console;
    bool m_log_window_show;
    bool m_crash_handle_disable;
};

DECLARE_APP(SpringLobbyApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
