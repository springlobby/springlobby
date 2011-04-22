/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/
#include "main.h"
#include "frame.h"

#include "../crashreport.h"
#include "../utils/conversion.h"
#include "../utils/platform.h"
#include "../settings.h"
#include "se_utils.h"
#include "../defines.h"

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/cmdline.h>
#include <wx/frame.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "../utils/customdialogs.h"
#include "../globalsmanager.h"
#include "../springunitsynclib.h"
#include "../customizations.h"
#include "../Helper/wxTranslationHelper.h"

IMPLEMENT_APP(Springsettings)

Springsettings::Springsettings()
	:  m_translationhelper(NULL),
	m_log_verbosity( 3 ),
    m_log_console( true ),
	m_log_file( false ),
    m_log_window_show( false ),
	m_crash_handle_disable( false ),
	m_appname( _T("SpringSettings") ),
	m_engine_config_filepath(wxEmptyString)
{}

Springsettings::~Springsettings()
{}


#if defined(__WXMSW__) && defined(ENABLE_DEBUG_REPORT)
LONG __stdcall filter(EXCEPTION_POINTERS* p){
	#if wxUSE_STACKWALKER
		CrashReport::instance().GenerateReport();
	#else
		CrashReport::instance().GenerateReport(p);
	#endif
	return 0; //must return 0 here or we'll end in an inf loop of dbg reports
}
#endif

void Springsettings::OnFatalException()
{
#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
	#if wxUSE_STACKWALKER
		CrashReport::instance().GenerateReport();
	#else
		EXCEPTION_POINTERS* p = new EXCEPTION_POINTERS; //lets hope this'll never get called
		CrashReport::instance().GenerateReport(p);
	#endif
#else
	wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
#endif
}

bool Springsettings::OnInit()
{
	wxSetEnv( _T("UBUNTU_MENUPROXY"), _T("0") );
    //this triggers the Cli Parser amongst other stuff
    if (!wxApp::OnInit())
        return false;

	SetAppName( m_appname );

	if ( !wxDirExists( GetConfigfileDir() ) )
		wxMkdir( GetConfigfileDir() );

	if (!m_crash_handle_disable) {
	#if wxUSE_ON_FATAL_EXCEPTION
		wxHandleFatalExceptions( true );
	#endif
	#if defined(__WXMSW__) && defined(ENABLE_DEBUG_REPORT)
		//this undocumented function acts as a workaround for the dysfunctional
		// wxUSE_ON_FATAL_EXCEPTION on msw when mingw is used (or any other non SEH-capable compiler )
		SetUnhandledExceptionFilter(filter);
	#endif
	}

    //initialize all loggers
	//TODO non-constant parameters
	wxLogChain* logchain  = 0;
	wxLogWindow* loggerwin = InitializeLoggingTargets( 0, m_log_console, m_log_file_path, m_log_window_show, !m_crash_handle_disable, m_log_verbosity, logchain );
	//this needs to called _before_ mainwindow instance is created
	wxInitAllImageHandlers();

#ifdef __WXMSW__
	wxString path = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
#else
	#if defined(LOCALE_INSTALL_DIR)
		wxString path ( _T(LOCALE_INSTALL_DIR) );
	#else
		// use a dummy name here, we're only interested in the base path
		wxString path = wxStandardPaths::Get().GetLocalizedResourcesDir(_T("noneWH"),wxStandardPaths::ResourceCat_Messages);
		path = path.Left( path.First(_T("noneWH") ) );
	#endif
#endif
	m_translationhelper = new wxTranslationHelper( *( (wxApp*)this ), path );
	m_translationhelper->Load();

    SetSettingsStandAlone( true );

	if ( !m_customizer_archive_name.IsEmpty() )
	{//this needsto happen before usync load
		sett().SetForcedSpringConfigFilePath( GetCustomizedEngineConfigFilePath() );
	}
	else if ( !m_engine_config_filepath.IsEmpty() )
	{

		if ( !wxFileName( m_engine_config_filepath ).IsAbsolute() )
		{
			customMessageBox( SS_MAIN_ICON, wxString::Format( _T(" custom engine config filename is not an sbolute path"), m_engine_config_filepath.c_str() ) );
			return false;
		}
		if ( !wxFile::Exists( m_engine_config_filepath ) )
		{
			if ( !wxFile(m_engine_config_filepath,wxFile::write).Write(wxString()) )
			{
				customMessageBox( SS_MAIN_ICON, wxString::Format( _T("cannot open custom engine config filename for wrtiting"), m_engine_config_filepath.c_str() ) );
				return false;
			}
		}
		if( !wxFileName::IsFileWritable( m_engine_config_filepath ) )
		{
			customMessageBox( SS_MAIN_ICON, wxString::Format( _T("given custom engine config filename is not writeable"), m_engine_config_filepath.c_str() ) );
			return false;
		}
		sett().SetForcedSpringConfigFilePath( m_engine_config_filepath );
	}
	//unitsync first load, NEEDS to be blocking
	usync().ReloadUnitSyncLib();
	if ( !m_customizer_archive_name.IsEmpty() ) {
		if ( !SLcustomizations().Init( m_customizer_archive_name ) ) {
			customMessageBox( SL_MAIN_ICON, _("Couldn't load customizations for ") + m_customizer_archive_name + _("\nPlease check that that is the correct name, passed in qoutation"), _("Fatal error"), wxOK );
//            wxLogError( _("Couldn't load customizations for ") + m_customizer_archive_name + _("\nPlease check that that is the correct name, passed in qoutation"), _("Fatal error") );
			exit( OnExit() );//for some twisted reason returning false here does not terminate the app
		}
	}

	settings_frame* frame = new settings_frame(NULL,GetAppName());
    SetTopWindow(frame);
    frame->Show();

    if ( loggerwin ) { // we got a logwindow, lets set proper parent win
        loggerwin->GetFrame()->SetParent( frame );
    }

    return true;
}

int Springsettings::OnExit()
{
	if(m_translationhelper)
	{
		wxDELETE(m_translationhelper);
	}

	sett().SaveSettings(); // to make sure that cache path gets saved before destroying unitsync

	SetEvtHandlerEnabled(false);
	DestroyGlobals();
	return 0;
}

void Springsettings::OnInitCmdLine(wxCmdLineParser& parser)
{
	#ifndef HAVE_WX29
		#define STR _T
	#else
		#define STR
	#endif

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, STR("h"), STR("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_SWITCH, STR("nc"), STR("no-crash-handler"), _("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_SWITCH, STR("cl"), STR("console-logging"),  _("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("fl"), STR("file-logging"),  _("dumps application log to a file ( enter path )"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_SWITCH, STR("gl"), STR("gui-logging"),  _("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("f"), STR("config-file"),  _("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
		{ wxCMD_LINE_OPTION, STR("e"), STR("engine-config"),  _("override default choice for engine config-file, needs absolute path"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
		{ wxCMD_LINE_OPTION, STR("c"), STR("customize"),  _("load lobby customizations from given archive. Expects the archive filename."), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("n"), STR("name"),  _("overrides default application name"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("l"), STR("log-verbosity"),  _("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL } //this is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html
	};

    parser.SetDesc( cmdLineDesc );
	parser.SetSwitchChars (_T("-"));

	#undef STR
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool Springsettings::OnCmdLineParsed(wxCmdLineParser& parser)
{
  #if wxUSE_CMDLINE_PARSER
    if ( !parser.Parse(true) )
    {
#if wxUSE_STD_IOSTREAM
		m_log_console = parser.Found(_T("console-logging"));
#endif
		m_log_file = parser.Found(_T("file-logging"), &m_log_file_path);
        m_log_window_show = parser.Found(_T("gui-logging"));
        m_crash_handle_disable = parser.Found(_T("no-crash-handler"));

		Settings::m_user_defined_config = parser.Found( _T("config-file"), &Settings::m_user_defined_config_path );
		if ( Settings::m_user_defined_config ) {
			 wxFileName fn ( Settings::m_user_defined_config_path );
			 if ( ! fn.IsAbsolute() ) {
				 wxLogError ( _T("path for parameter \"config-file\" must be absolute") );
				 return false;
			 }
			 if ( ! fn.IsFileWritable() ) {
				 wxLogError ( _T("path for parameter \"config-file\" must be writeable") );
				 return false;
			 }
		}

		if ( !parser.Found(_T("log-verbosity"), &m_log_verbosity ) )
			m_log_verbosity = m_log_window_show ? 3 : 5;
		const bool do_custom = parser.Found(_T("customize"), &m_customizer_archive_name );
		if ( !do_custom )
			m_customizer_archive_name = _T("");
		const bool do_appname = parser.Found(_T("name"), &m_appname );
		if ( !do_appname )
			m_appname = _T("SpringLobby");

		if ( parser.Found(_T("engine-config"), &m_engine_config_filepath)
				&& ( do_appname || do_custom ) )
		{
				wxLogError( _T("cannot mix -n/-c and -e parameters") );
				return false;
		}
        return true;
    }
    else
    {
        return false;
    }
  #else // wxUSE_CMDLINE_PARSER
  return true;
  #endif
}
