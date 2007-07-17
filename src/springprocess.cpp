//
// Class: Spring
//

#include "springprocess.h"
#include "spring.h"

SpringProcess::SpringProcess( Spring& sp ) :
  m_sp(sp)
{ }

SpringProcess::~SpringProcess()
{ }

void SpringProcess::OnTerminate( int pid, int status )
{
  m_sp.OnTerminated();
}

