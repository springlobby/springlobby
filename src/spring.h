//
// Class: Spring
//

#ifndef _SPRING_H_
#define _SPRING_H_

#include <wx/string.h>
#include <wx/process.h>


class Battle;
class Ui;
class Spring;

class SpringProcess: public wxProcess
{
  public:
    SpringProcess( Spring& sp ): wxProcess(), m_sp(sp) {}
    ~SpringProcess() {}
    void OnTerminate( int pid, int status );
  protected:
    Spring& m_sp;
};


class Spring
{
  public:
    Spring( Ui& ui): m_ui(ui),m_process(NULL),m_running(false) {} 
    ~Spring() { if ( m_process != NULL ) delete m_process; }
  
    bool IsRunning() { return m_process != NULL; }
    bool Run( Battle& battle );

    wxString GetScriptTxt( Battle& battle );
    void OnTerminated();
  protected:

    Ui& m_ui;
    SpringProcess* m_process;
    bool m_running;
};


#endif  //_SPRING_H_

