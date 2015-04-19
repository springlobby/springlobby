/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/spring/springprocess.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/

#include <wx/log.h>

#include "springprocess.h"
#include "spring.h"
#include "utils/platform.h"
#include "log.h"

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
	event.SetExtraLong(m_exit_code);
	m_sp.AddPendingEvent(event);
}

void* SpringProcess::Entry()
{
	slLogDebugFunc("");
	RunProcess(m_cmd, m_params);
	return NULL;
}
