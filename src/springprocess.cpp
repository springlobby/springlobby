/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/spring/springprocess.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/
#include "springprocess.h"

#include <wx/log.h>

#include "log.h"
#include "spring.h"
#include "utils/platform.h"

DEFINE_LOCAL_EVENT_TYPE(wxEVT_SPRING_EXIT)

SpringProcess::SpringProcess(Spring& sp)
    : m_sp(sp)
    , m_exit_code(0)
{
	slLogDebugFunc("");
}


SpringProcess::~SpringProcess()
{
	slLogDebugFunc("");
}


void SpringProcess::SetCommand(const wxString& cmd, const wxArrayString& params)
{
	m_cmd = cmd;
	m_params = params;
}


void SpringProcess::OnExit()
{
	slLogDebugFunc("");
	wxCommandEvent event(wxEVT_SPRING_EXIT, PROC_SPRING);
	event.SetInt(m_exit_code);
	m_sp.AddPendingEvent(event);
}

void* SpringProcess::Entry()
{
	slLogDebugFunc("");
	m_exit_code = RunProcess(m_cmd, m_params);
	return NULL;
}
