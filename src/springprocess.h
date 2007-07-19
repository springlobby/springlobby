#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H

#include <wx/process.h>

class Spring;

class SpringProcess: public wxProcess
{
  public:
    SpringProcess( Spring& sp );
    ~SpringProcess();
    void OnTerminate( int pid, int status );

  protected:
    Spring& m_sp;
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
