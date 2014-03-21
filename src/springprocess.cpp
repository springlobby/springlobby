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

static wxString escapeStr(const wxString& str)
{
	if (str.Find(_T(" ")) == wxNOT_FOUND)
		return str;
	return _T("\"") + str + _T("\"");
}

static int runProcess(const wxString& cmd, const wxArrayString& params)
{
	wxString paramstring;
	for (wxString param: params) {
		if (!paramstring.empty()) {
			paramstring += _T(" ");
		}
		paramstring += escapeStr(param);
	}
#ifdef __WXMSW__
	SHELLEXECUTEINFO ShExecInfo;
	DWORD exitCode = 0;

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = cmd.t_str();
	ShExecInfo.lpParameters = paramstring.t_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	GetExitCodeProcess(ShExecInfo.hProcess, &exitCode);
	return exitCode;
#else
	wxString realcmd = escapeStr(cmd);
	if (!paramstring.empty()) {
		realcmd += _T(" ") + paramstring;
	}
	return system( realcmd.mb_str( wxConvUTF8 ) );
#endif
}

void* SpringProcess::Entry()
{
	wxLogDebugFunc( wxEmptyString );
	runProcess(m_cmd, m_params);
	wxLogMessage( _T( "Spring closed." ) );
	return NULL;
}

