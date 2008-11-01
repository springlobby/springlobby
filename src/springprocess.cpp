/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "springprocess.h"
#include "spring.h"
#include "utils.h"
#include <wx/log.h>


DEFINE_LOCAL_EVENT_TYPE( wxEVT_SPRING_EXIT )


SpringProcess::SpringProcess( Spring& sp ) :
  m_sp(sp)
{
  wxLogDebugFunc( _T("") );
}


SpringProcess::~SpringProcess()
{
  wxLogDebugFunc( _T("") );
}


void SpringProcess::SetCommand( const wxString& cmd )
{
  m_cmd = cmd;
}


void SpringProcess::OnExit()
{
  wxLogDebugFunc( _T("") );
  wxCommandEvent event( wxEVT_SPRING_EXIT, PROC_SPRING );
  m_sp.AddPendingEvent(event);
}


void* SpringProcess::Entry()
{
  wxLogDebugFunc( _T("") );
  system( m_cmd.mb_str(wxConvUTF8) );
  wxLogMessage(_T("Spring closed."));
  return 0;
}

wxSpringProcess::wxSpringProcess( Spring& sp ) :
  m_sp(sp)
{
  wxLogDebugFunc( _T("") );
}


wxSpringProcess::~wxSpringProcess()
{
  wxLogDebugFunc( _T("") );
}

void wxSpringProcess::OnTerminate( int pid, int status )
{
  wxCommandEvent event( wxEVT_SPRING_EXIT, PROC_SPRING );
  m_sp.AddPendingEvent(event);
}

