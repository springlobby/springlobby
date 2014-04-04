/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: UpdaterApp
//

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <wx/cmdline.h>
#include <wx/image.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/msgdlg.h>

#include <fstream>

#include "updaterapp.h"
#include "updatermainwindow.h"
#include "utils/platform.h"
#include "utils/conversion.h"

IMPLEMENT_APP(UpdaterApp)

UpdaterApp::UpdaterApp():
	m_pid(0),
	m_updater_window(NULL)
{
	SetAppName( _T("springlobby_updater") );
}

UpdaterApp::~UpdaterApp()
{
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

	m_updater_window = new UpdaterMainwindow();
	m_updater_window->Show( true );
	SetTopWindow( m_updater_window );
	bool ret = false;
	if (m_paramcount == 2) {
		ret = StartUpdate(m_source_dir, m_destination_dir);
	} else if ( m_paramcount == 5) {
		WaitForExit(m_pid);
		wxArrayString params;
		params.push_back(m_source_dir);
		params.push_back(m_destination_dir);
		RunProcess(m_updater_exe,  params, false, true); //start updater as admin for copying
		params.clear(); //start springlobby
		ret = RunProcess(m_springlobby_exe, params, true);
	}

	m_updater_window->Close();
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

#ifdef WIN32
#define SEP '\\'
#else
#define SEP '/'
#endif

static bool CheckDir(wxString& dir)
{
	if (dir[dir.length()-1] == SEP)
		dir = dir.BeforeLast(SEP);
	if (wxDirExists(dir)) return true;
	wxMessageBox(_T("%s doesn't exist!"), dir.c_str());
	return false;
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
		if (!CheckDir(m_source_dir)) return false;
		if (!CheckDir(m_destination_dir)) return false;
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


bool UpdaterApp::StartUpdate( const wxString& source, const wxString& destination )
{
	wxString sep = wxFileName::GetPathSeparator();
	if ( !wxFileName::IsDirWritable( destination ) ) {
		wxMessageBox(_("Unable to write to the lobby installation directory.\nPlease update manually or enable write permissions for the current user."), _("Error"));
		return false;
	}
	bool success = CopyDirWithFilebackupRename( source, destination);
	if ( !success ) {
		wxString msg =  _T("Copy failed: \n") + source + _T("\n") + destination;
		wxMessageBox(msg, _("Error") );
		return false;
	}
	return true;
}
