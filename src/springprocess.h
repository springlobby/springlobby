//
// Class: SpringProcess
//

#ifndef _SPRINGPROCESS_H_
#define _SPRINGPROCESS_H_

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

#endif  // _SPRINGPROCESS_H_

