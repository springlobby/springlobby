//
// Class: SpringLobbyApp
//

#ifndef _SPRINGLOBBYAPP_H_
#define _SPRINGLOBBYAPP_H_

#include <wx/app.h>

class Ui;
class wxTimer;

//! @brief Spring lobby wxApp
class SpringLobbyApp : public wxApp
{
  public:
    SpringLobbyApp();
    ~SpringLobbyApp();

    virtual bool OnInit();
    virtual int OnExit();

    // System Events
    void OnTimer( wxTimerEvent& event );

  protected:

    wxTimer* m_timer;

    Ui* m_ui;

    DECLARE_EVENT_TABLE()
};


DECLARE_APP(SpringLobbyApp)

#endif  //_SPRINGLOBBYAPP_H_

