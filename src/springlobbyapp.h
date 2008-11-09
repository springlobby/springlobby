#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class wxTimer;
class wxIcon;
class wxLocale;
class HttpDownloader;
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

  protected:

    void SetupUserFolders();
    void InitCacheDir();

    wxTimer* m_timer;

    wxTranslationHelper* m_translationhelper;
    HttpDownloader* m_otadownloader ;

    DECLARE_EVENT_TABLE()
};

DECLARE_APP(SpringLobbyApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
