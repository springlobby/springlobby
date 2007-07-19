#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

#include <wx/string.h>

class Battle;
class Ui;
class SpringProcess;

class Spring
{
  public:
    Spring( Ui& ui);
    ~Spring();

    bool IsRunning();
    bool Run( Battle& battle );

    wxString GetScriptTxt( Battle& battle );
    void OnTerminated();

  protected:
    Ui& m_ui;
    SpringProcess* m_process;
    bool m_running;
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H
