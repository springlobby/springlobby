/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/spring/springprocess.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include "springprocess.h"
#include "spring.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include <wx/log.h>

#ifdef __WXMSW__
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#endif


DEFINE_LOCAL_EVENT_TYPE( wxEVT_SPRING_EXIT )


SpringProcess::SpringProcess( Spring& sp ) :
		m_sp( sp ), m_exit_code( 0 )
{
	wxLogDebugFunc( wxEmptyString );
}


SpringProcess::~SpringProcess()
{
	wxLogDebugFunc( wxEmptyString );
}


void SpringProcess::SetCommand(const wxString& cmd, const wxArrayString& params)
{
	m_cmd = cmd;
	m_params = params;
}


void SpringProcess::OnExit()
{
	wxLogDebugFunc( wxEmptyString );
	wxCommandEvent event( wxEVT_SPRING_EXIT, PROC_SPRING );
	event.SetExtraLong( m_exit_code );
	m_sp.AddPendingEvent( event );
}


void* SpringProcess::Entry()
{
	wxLogDebugFunc( wxEmptyString );
	wxString params;
	for (wxString param: m_params) {
		if (!params.empty()) {
			params += _T(" ");
		}
		params += param;
	}
#ifdef __WXMSW__
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = m_cmd.t_str();
	ShExecInfo.lpParameters = params.t_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
#else
	const wxString cmd = m_cmd + params;
	m_exit_code = system( cmd.mb_str( wxConvUTF8 ) );
#endif
	wxLogMessage( _T( "Spring closed." ) );
	return 0;
}
