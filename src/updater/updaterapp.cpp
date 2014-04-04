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
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/log.h>
#include <wx/msgdlg.h>

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
#include <wx/utils.h>
#include <wx/wfstream.h>
#include <fstream>
#include <string>
#include <vector>

#include "updaterapp.h"
#include "utils/platform.h"
#include "versionchecker.h"
#include "updatermainwindow.h"
#include "utils/conversion.h"

IMPLEMENT_APP(UpdaterApp)

UpdaterApp::UpdaterApp():
	m_updater_window( 0 )
{
	SetAppName( _T("springlobby_updater") );

	const std::string filename = STD_STRING(wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("update.log") ).c_str();
	m_logstream_target = new std::ofstream(filename);
	if ( (!m_logstream_target->good()) || (!m_logstream_target->is_open() )) {
		printf("Error opening %s\n", filename.c_str());
	}
}

UpdaterApp::~UpdaterApp()
{
    m_logstream_target->flush();
    m_logstream_target->close();
    delete m_logstream_target;
}


//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool UpdaterApp::OnInit()
{
    //this triggers the Cli Parser amongst other stuff
    if (!wxApp::OnInit())
        return false;

    assert( m_logstream_target );
    wxLogStream* m_log_stream = new wxLogStream( m_logstream_target );
    m_log_stream->SetLogLevel( wxLOG_Trace );
    wxLog::SetActiveTarget( m_log_stream );
    wxLogMessage( _T("m_source_dir ") + m_source_dir);
    wxLogMessage( _T("m_destination_dir ") + m_destination_dir);

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

	bool ret = StartUpdate(m_source_dir, m_destination_dir);
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
    wxCmdLineEntryDesc cmdLineDesc[] =
    {
		{ wxCMD_LINE_SWITCH, _("h"), _("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_PARAM,  NULL, NULL, _("<parent pid> <springlobby.exe> <updater.exe> <source dir> <destination dir>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
		{ wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0 }//while this throws warnings, it is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html
    };

    parser.SetDesc( cmdLineDesc );
    parser.SetSwitchChars (_T("-"));

}

static bool CheckDir(const wxString& dir)
{
	if (wxDirExists(dir)) return true;
	wxLogError(_T("%s doesn't exist!"), dir.c_str());
	return false;
}


//! @brief parses the command line and sets global app options like log verbosity or log target
bool UpdaterApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	#if wxUSE_CMDLINE_PARSER
	if ( parser.Found(_T("help")) )
		return false; // not a syntax error, but program should stop if user asked for command line usage
	if (parser.GetParamCount() == 2){
		m_source_dir = parser.GetParam(0);
		m_destination_dir = parser.GetParam(1);
		if (!CheckDir(m_source_dir)) return false;
		if (!CheckDir(m_destination_dir)) return false;
		return true;
	}
	if (parser.GetParamCount() == 5){
		long pid;
		if (parser.GetParam(0).ToLong(&pid)) {
			return false;
		}
		WaitForExit(pid);

		wxArrayString params;
		params.push_back(parser.GetParam(3));
		params.push_back(parser.GetParam(4));
		RunProcess(parser.GetParam(2),  params, false, true);
		params.clear();
		RunProcess(parser.GetParam(1), params, false);
		return false;
	}
	#endif
	return false;
}


bool UpdaterApp::StartUpdate( const wxString& source, const wxString& destination )
{
    wxString sep = wxFileName::GetPathSeparator();
    if ( !wxFileName::IsDirWritable( destination ) ) {
        wxLogError( _T("dir not writable: ") + destination );
        wxMessageBox(_("Unable to write to the lobby installation directory.\nPlease update manually or enable write permissions for the current user."), _("Error"));
        return false;
    }
    bool success = CopyDirWithFilebackupRename( source, destination);
    if ( !success ) {
        wxLogError( _T("Full dir copy failed") );
		return false;
    }
	return true;
}
