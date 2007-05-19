//
// Class: SpringLobbyApp
//

#ifndef _SPRINGLOBBYAPP_H_
#define _SPRINGLOBBYAPP_H_

#include <wx/app.h>
#include <wx/timer.h>

#define TIMER_ID 101
#define TIMER_INTERVAL 100

 
//! @brief Spring lobby wxApp
class SpringLobbyApp : public wxApp
{
  public:
    // SpringLobbyApp interface
    SpringLobbyApp() { m_timer = new wxTimer(this, TIMER_ID); }
    ~SpringLobbyApp() { delete m_timer; }
    
    virtual bool OnInit();
    virtual int OnExit();
    
    // System Events
    void OnTimer( wxTimerEvent& event );
  
  protected:
  
    wxTimer* m_timer;

  
    DECLARE_EVENT_TABLE()
};


DECLARE_APP(SpringLobbyApp)

#endif  //_SPRINGLOBBYAPP_H_

