/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: SpringLobbyApp
//

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/image.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/tooltip.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
#include <wx/utils.h>
#include <wx/wfstream.h>

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "crashreport.h"
#include "utils.h"
#include "ui.h"
#include "iunitsync.h"
#include "channel/channel.h"
#include "settings++/custom_dialogs.h"
#include "settings++/se_utils.h"
#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif
#include "updater/updater.h"
#include "globalsmanager.h"
#include "Helper/wxTranslationHelper.h"
#include "Helper/tasclientimport.h"

const unsigned int TIMER_ID         = 101;
const unsigned int TIMER_INTERVAL   = 100;


#if 0
/// testing TDF parser
#include "tdfcontainer.h"
#include <iostream>
#include <fstream>
void TestTDFParser(){
  PDataList parsetree(new DataList);
  Tokenizer tokenizer;
  std::ifstream f("/home/dmytry/.spring/script.txt");
  tokenizer.EnterStream(f);
  parsetree->Load(tokenizer);
  wxString result;
  TDFWriter writer(result);
  parsetree->Save(writer);
  wxLogMessage(_T("Testing tdf parser: result %s "), result.c_str());
}
#endif

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)

    EVT_TIMER(TIMER_ID, SpringLobbyApp::OnTimer)

END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp()
    :m_translationhelper( NULL )
{
    m_timer = new wxTimer(this, TIMER_ID);
    SetAppName( _T("springlobby") );
}

SpringLobbyApp::~SpringLobbyApp()
{
    delete m_timer;
}

//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{


#if wxUSE_ON_FATAL_EXCEPTION && wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
    wxHandleFatalExceptions( true );
#endif

    //initialize all loggers
    InitializeLoggingTargets();
    wxSocketBase::Initialize();

    wxLogDebugFunc( _T("") );
    wxInitAllImageHandlers();

     //TODO needed?
    wxImage::AddHandler(new wxPNGHandler);
    wxFileSystem::AddHandler(new wxZipFSHandler);


#ifdef __WXMSW__
    wxString path = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
#else
    wxString path = wxStandardPaths::Get().GetLocalizedResourcesDir(_T("noneWH"),wxStandardPaths::ResourceCat_Messages);
    path = path.Left( path.First(_T("noneWH") ) );
#endif


    m_translationhelper = new wxTranslationHelper( *( (wxApp*)this ), path );
    m_translationhelper->Load();


		if( sett().IsFirstRun() )
		{
			wxString defaultconfigpath = GetExecutableFolder() + wxFileName::GetPathSeparator() + _T("springlobby.global.conf");
			if (  wxFileName::FileExists( defaultconfigpath ) )
			{
				wxFileInputStream instream( defaultconfigpath );

				if ( instream.IsOk() )
				{
					#ifdef __WXMSW__
					SL_WinConf defaultconf( instream );
					#else
					wxConfig defaultconf( instream );
					#endif
					sett().SetDefaultConfigs( defaultconf );
				}
			}
		}

    SetSettingsStandAlone( false );

    if ( sett().IsFirstRun() && !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) ) wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

    if ( (sett().GetCacheVersion() < CACHE_VERSION) && !sett().IsFirstRun() )
    {
        sett().SetMapCachingThreadProgress( 0 ); // reset map cache thread
        sett().SetModCachingThreadProgress( 0 ); // reset mod cache thread
        if ( wxDirExists( sett().GetCachePath() )  )
        {
            wxLogWarning( _T("erasing old cache ver %d (app cache ver %d)"), sett().GetCacheVersion(), CACHE_VERSION );
            wxString file = wxFindFirstFile( sett().GetCachePath() + wxFILE_SEP_PATH + _T("*") );
            while ( !file.empty() )
            {
                wxRemoveFile( file );
                file = wxFindNextFile();
            }
        }
    }

    if ( !sett().IsFirstRun() )
    {
    	if ( sett().GetSettingsVersion() < 3 ) sett().ConvertOldSpringDirsOptions();
			if ( sett().GetSettingsVersion() < 4 )
			{
				if ( sett().GetTorrentPort() == DEFSETT_SPRING_PORT ) sett().SetTorrentPort( DEFSETT_SPRING_PORT + 1 );
			}
			if ( sett().GetSettingsVersion() < 5 )
			{
				wxArrayString list = sett().GetServers();
				int count = list.GetCount();
				wxString wordlist = sett().GetHighlightedWords();
				for ( int i= 0; i < count; i++ )
				{
					wxString nick = sett().GetServerAccountNick( list[i] );
					if ( !wordlist.Contains( nick ) )
					{
						 if ( !wordlist.IsEmpty() && !wordlist.EndsWith( _T(";") ) ) wordlist += _T(";");
					}  wordlist += nick;
				}
				sett().SetHighlightedWords( wordlist );
			}
			if ( sett().GetSettingsVersion() < 6 )
			{
				sett().ConvertOldServerSettings();
			}
			if ( sett().GetSettingsVersion() < 7 )
			{
				sett().AddChannelJoin( _T("springlobby"), _T("") );
			}
			if ( sett().GetSettingsVersion() < 8 )
			{
				 sett().DeleteServer( _T("Backup server") );
				 sett().SetServer( _T("Backup server 1"), _T("springbackup1.servegame.com"), 8200 );
				 sett().SetServer( _T("Backup server 2"), _T("springbackup2.servegame.org"), 8200 );
				 sett().SetServer( _T("Test server"), _T("taspringmaster.servegame.com"), 8300 );
			}
			if ( sett().GetSettingsVersion() < 10 )
			{
				sett().ConvertOldColorSettings();
			}
			if ( sett().GetSettingsVersion() < 11 )
			{
		  #ifdef __WXMSW__
				wxRegKey UACpath( _T("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System") ); // check if UAC is on, skip dialog if not
				if( UACpath.Exists() )
				{
					long value;
					if( UACpath.QueryValue( _T("EnableLUA"), &value ) ) // reg key not present -> not vista
					{
						if( value != 0 ) usync().SetSpringDataPath(_T("")); // UAC is on, fix the spring data path
					}
				}
			#endif
			}
			if ( sett().GetSettingsVersion() < 12 )
			{
				sett().ConvertOldChannelSettings();
			}
    }

    ui().ReloadUnitSync(); // first time load of unitsync
    ui().ShowMainWindow();

    if ( sett().IsFirstRun() )
    {
#ifdef __WXMSW__
        sett().SetOldSpringLaunchMethod( true );
#endif

        wxLogMessage( _T("first time startup"));
        wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nIt looks like this is your first time using SpringLobby. I have guessed a configuration that I think will work for you but you should review it, especially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
                     wxOK | wxICON_INFORMATION, &ui().mw() );
        SetupUserFolders();

				if ( sett().ShouldAddDefaultServerSettings() ) sett().SetDefaultServerSettings();
				if ( sett().ShouldAddDefaultChannelSettings() )
				{
					sett().AddChannelJoin( _T("main"), _T("") );
					sett().AddChannelJoin( _T("newbies"), _T("") );
				}
				if ( sett().ShouldAddDefaultGroupSettings() ) sett().AddGroup( _("Default") );

        if ( !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) ) wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

        customMessageBoxNoModal(SL_MAIN_ICON, _("By default SpringLobby reports some statistics.\nYou can disable that on options tab --> General."),_("Notice"),wxOK );


				// copy uikeys.txt
				wxPathList pl;
				pl.AddEnvList( _T("%ProgramFiles%") );
				pl.AddEnvList( _T("XDG_DATA_DIRS") );
				pl = sett().GetAdditionalSearchPaths( pl );
				wxString uikeyslocation = pl.FindValidPath( _T("uikeys.txt") );
				if ( !uikeyslocation.IsEmpty() )
				{
					wxCopyFile( uikeyslocation, sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("uikeys.txt"), false );
				}

    #ifdef __WXMSW__
        if ( TASClientPresent() &&
                customMessageBox(SL_MAIN_ICON, _("Should I try to import (some) TASClient settings?\n" ),_("Import settings?"), wxYES_NO ) == wxYES )
        {
            ImportTASClientSettings();
        }
    #endif

        ui().mw().ShowConfigure();
    }
    else
    {
        ui().Connect();
    }

  #ifndef NO_TORRENT_SYSTEM
  if( sett().GetTorrentSystemAutoStartMode() == 1 ) torrent().ConnectToP2PSystem();
  #endif

  m_timer->Start( TIMER_INTERVAL );

//  #ifdef __WXMSW__
//  if ( sett().GetAutoUpdate() )Updater().CheckForUpdates();
//  #endif

    return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
    wxLogDebugFunc( _T("") );

    if(m_translationhelper)
    {
        wxDELETE(m_translationhelper);
    }


  #ifndef NO_TORRENT_SYSTEM
  //if( sett().GetTorrentSystemAutoStartMode() == 1 )
  torrent().DisconnectFromP2PSystem();// Cant hurt to disconnect unconditionally.
  #endif

  m_timer->Stop();

  sett().SaveSettings(); // to make sure that cache path gets saved before destroying unitsync

	if ( usync().IsLoaded() )
	{
		usync().FreeUnitSyncLib();
	}

  DestroyGlobals();

  return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
#if wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
    crashreport().GenerateReport(wxDebugReport::Context_Exception);
#else
    wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
#endif
}


//! @brief Is called every 1/10 seconds to update statuses
void SpringLobbyApp::OnTimer( wxTimerEvent& event )
{
    ui().OnUpdate( event.GetInterval() );
}


/** Try to create the named directory, if it doesn't exist.
 *
 * @param name Path to directory that should exist or be created.
 *
 * @param perm Value of @p perm parameter for wxFileName::Mkdir.
 *
 * @param flags Value of @p flags parameter for wxFileName::Mkdir.
 *
 * @return @c true if the directory already exists, or the return
 * value of wxFileName::Mkdir if it does not.
 */
inline bool
tryCreateDirectory(const wxString& name, int perm = 0775, int flags = 0)
{
    if ( wxFileName::DirExists(name) )
	return true;
    else
	return wxFileName::Mkdir(name, perm, flags);
}

void SpringLobbyApp::SetupUserFolders()
{
      wxString sep = wxFileName::GetPathSeparator();
      wxString defaultdir = wxFileName::GetHomeDir() + sep +_T("spring");
      wxArrayString choices;

      int donothing = choices.Add( _("Do nothing") );
      int createcustompath = choices.Add( _("Create a folder in a custom path (you'll get prompted for the path)") );
      int choseexisting = choices.Add( _("I have already a SpringData folder, i want to browse manually for it") );

      int result = wxGetSingleChoiceIndex(
                       _("Looks like you don't have yet a user SpringData folder structure\nWhat would you like to do? (leave default choice if you don't know what this is for)"),
                       _("First time wizard"),
                       choices );

      wxString dir;
      bool createdirs = true;
      if ( result == choseexisting ) createdirs = false;
      else if ( result == donothing ) return;

      if ( result == createcustompath || result == choseexisting ) dir = wxDirSelector( _("Choose a folder"), defaultdir );

      if ( createdirs )
      {
				if ( dir.IsEmpty() ||
	       ( !tryCreateDirectory( dir, 0775 ) ||
				 ( !tryCreateDirectory( dir + sep + _T("mods"), 0775 ) ||
		       !tryCreateDirectory( dir + sep + _T("maps"), 0775 ) ||
		       !tryCreateDirectory( dir + sep + _T("base"), 0775 ) ||
		       !tryCreateDirectory( dir + sep + _T("demos"), 0775 ) ||
					 !tryCreateDirectory( dir + sep + _T("screenshots"), 0775  ) )
				 )
	       )
				{
					if ( dir.IsEmpty() ) dir = defaultdir;
					wxMessageBox( _("Something went wrong when creating the directories\nPlease create manually the following folders:") + wxString(_T("\n")) + dir +  _T("\n") + dir + sep + _T("mods\n") + dir + sep + _T("maps\n") + dir + sep + _T("base\n") );
				return;
				}
      }
      if ( usync().IsLoaded() )
      {
				usync().SetSpringDataPath(dir);
      }
}

bool SpringLobbyApp::SelectLanguage()
{
    wxArrayString names;
    wxArrayLong identifiers;
    int current_selection_index;
    m_translationhelper->GetInstalledLanguages( names, identifiers, current_selection_index );
    bool ret = m_translationhelper->AskUserForLanguage( names, identifiers, current_selection_index );
    if ( ret ) m_translationhelper->Save();
    return ret;
}
