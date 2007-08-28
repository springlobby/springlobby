#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

#include <wx/string.h>
#include <wx/event.h>

class SinglePlayerBattle;
class Battle;
class Ui;
class SpringProcess;


class Spring: public wxEvtHandler
{
  public:
    Spring( Ui& ui);
    ~Spring();

    bool IsRunning();
    bool Run( Battle& battle );
    bool Run( SinglePlayerBattle& battle );
    static bool TestSpringBinary();

    wxString GetScriptTxt( Battle& battle );
    wxString GetSPScriptTxt( SinglePlayerBattle& battle );
    void OnTerminated( wxCommandEvent& event );

  protected:
    Ui& m_ui;
    SpringProcess* m_process;
    bool m_running;

    DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H
