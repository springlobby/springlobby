/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "springprocess.h"
#include "spring.h"
#include "utils.h"

DEFINE_EVENT_TYPE( wxEVT_SPRING_EXIT )


SpringProcess::SpringProcess( Spring& sp ) :
  m_sp(sp)
{ }


SpringProcess::~SpringProcess()
{
  debug_func("");
}


void SpringProcess::SetCommand( const wxString& cmd )
{
  m_cmd = cmd;
}


void SpringProcess::OnExit()
{
  debug_func("");
  wxCommandEvent event( wxEVT_SPRING_EXIT, PROC_SPRING );
  m_sp.AddPendingEvent(event);
}


void* SpringProcess::Entry()
{
  debug_func("");
  system( STD_STRING(m_cmd).c_str() );
  debug("Spring closed.");
  return 0;
}

