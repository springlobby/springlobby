#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

#include <wx/string.h>
#include <wx/event.h>
#include <iostream>

class SinglePlayerBattle;
class Battle;
class Ui;
class SpringProcess;
class wxSpringProcess;


class Spring: public wxEvtHandler
{
  public:
    Spring( Ui& ui);
    ~Spring();

    bool IsRunning();
    bool Run( Battle& battle );
    bool Run( SinglePlayerBattle& battle );
    static bool TestSpringBinary();

    void WriteScriptTxt(std::ostream &output, Battle& battle );
    void WriteSPScriptTxt(std::ostream &output, SinglePlayerBattle& battle );
    void OnTerminated( wxCommandEvent& event );

  protected:
    Ui& m_ui;
    SpringProcess* m_process;
    wxSpringProcess* m_wx_process;
    bool m_running;

    DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H
