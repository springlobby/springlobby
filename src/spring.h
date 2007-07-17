//
// Class: Spring
//

#ifndef _SPRING_H_
#define _SPRING_H_

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

#endif  //_SPRING_H_

