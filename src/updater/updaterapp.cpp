/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: UpdaterApp
//

#include <wx/intl.h>
#include <wx/cmdline.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/msgdlg.h>

#include <fstream>

#include "updaterapp.h"
#include "utils/platform.h"
#include "utils/conversion.h"

IMPLEMENT_APP(UpdaterApp)

UpdaterApp::UpdaterApp():
	m_pid(0)
{
	SetAppName( _T("springlobby_updater") );
}

UpdaterApp::~UpdaterApp()
{
}

static wxString TrimQuotes(const wxString& str)
{
	wxString res = str;
	res.Replace(_T("\""), wxEmptyString, true);
	return res;
}

//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool UpdaterApp::OnInit()
{
	//this triggers the Cli Parser amongst other stuff
	if (!wxApp::OnInit())
		return false;

#if wxUSE_ON_FATAL_EXCEPTION
	wxHandleFatalExceptions( true );
#endif

	//this needs to called _before_ mainwindow instance is created
	wxInitAllImageHandlers();
	//TODO needed?
	wxImage::AddHandler(new wxPNGHandler);
	wxFileSystem::AddHandler(new wxZipFSHandler);

	bool ret = false;
	if (m_paramcount == 2) {
		ret = StartUpdate(TrimQuotes(m_source_dir), TrimQuotes(m_destination_dir));
	} else if ( m_paramcount == 5) {
		WaitForExit(m_pid);
		wxArrayString params;
		params.push_back(m_source_dir);
		params.push_back(m_destination_dir);
		RunProcess(m_updater_exe,  params, false, true); //start updater as admin for copying
		params.clear(); //start springlobby
		ret = RunProcess(m_springlobby_exe, params, true);
	}
	return ret;
}


//! @brief Finalizes the application
int UpdaterApp::OnExit()
{
	SetEvtHandlerEnabled(false);
	return 0;
}

//! @brief is called when the app crashes
void UpdaterApp::OnFatalException()
{
	wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
}

void UpdaterApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxCmdLineEntryDesc cmdLineDesc[] = {
		{ wxCMD_LINE_SWITCH, _("h"), _("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_PARAM,  NULL, NULL, _("<parent pid> <springlobby.exe> <updater.exe> <source dir> <destination dir>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
		{ wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0 }//while this throws warnings, it is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html
	};

	parser.SetDesc( cmdLineDesc );
	parser.SetSwitchChars (_T("-"));

}

//! @brief parses the command line
bool UpdaterApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
#if wxUSE_CMDLINE_PARSER
	if ( parser.Found(_T("help")) )
		return false; // not a syntax error, but program should stop if user asked for command line usage
	m_paramcount = parser.GetParamCount();
	if (m_paramcount == 2) {
		m_source_dir = parser.GetParam(0);
		m_destination_dir = parser.GetParam(1);
		return true;
	}
	if (m_paramcount == 5) {
		if (!parser.GetParam(0).ToLong(&m_pid)) {
			wxMessageBox(_T("Invalid pid %s"), parser.GetParam(0).c_str());
			return false;
		}
		m_springlobby_exe = parser.GetParam(1);
		m_updater_exe = parser.GetParam(2);
		m_source_dir = parser.GetParam(3);
		m_destination_dir = parser.GetParam(4);
		return true;
	}
#endif
	return false;
}

static wxString TrimSep(const wxString& path)
{
	const wxString sep = wxFileName::GetPathSeparator();
	if (path.EndsWith(sep)) {
		return path.SubString(0, path.length()-2);
	}
	return path;
}

bool UpdaterApp::StartUpdate( const wxString& source, const wxString& destination )
{
	bool success = CopyDirWithFilebackupRename( TrimSep(source), TrimSep(destination), true);
	if ( !success ) {
		const wxString msg =  _T("Copy failed: \n") + source + _T("\n") + destination;
		wxMessageBox(msg, _("Error") );
		return false;
	}
	return true;
}
