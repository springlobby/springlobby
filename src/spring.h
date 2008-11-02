#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

#include <wx/event.h>

class wxCommandEvent;
class SinglePlayerBattle;
class Battle;
class Ui;
class SpringProcess;
class wxSpringProcess;
class wxString;


class Spring: public wxEvtHandler
{
  public:
    Spring( Ui& ui);
    ~Spring();

    bool IsRunning();
    bool Run( Battle& battle );
    bool Run( SinglePlayerBattle& battle );

    //! executes spring with replay as parameter
    /*!
     * \param filename the full path for the replayfile
     */
    bool RunReplay ( wxString& filename );

    wxString WriteScriptTxt( Battle& battle );
    wxString WriteSPScriptTxt( SinglePlayerBattle& battle );
    void OnTerminated( wxCommandEvent& event );

  protected:
    Ui& m_ui;
    SpringProcess* m_process;
    wxSpringProcess* m_wx_process;
    bool m_running;

    DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H
