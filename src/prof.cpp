/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: SpringLobbyApp
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/app.h>

class SpringLobbyApp : public wxApp
{
  public:
    SpringLobbyApp();
    ~SpringLobbyApp();

    virtual bool OnInit();
    virtual int OnExit();
    void SelectLanguage();

  protected:

    DECLARE_EVENT_TABLE()
};

DECLARE_APP(SpringLobbyApp)



#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/image.h>
#include <wx/cmdline.h>
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
#include <boost/timer/timer.hpp>


#include "useractions.h"
#include "mainwindow.h"
#include "settings.h"
#include "crashreport.h"
#include "utils/controls.h"
#include "utils/platform.h"
#include "ui.h"
#include "springunitsync.h"
#include "channel/channel.h"
#include "utils/customdialogs.h"
#include "utils/downloader.h"
#include "settings++/se_utils.h"
#include "torrentwrapper.h"
#include "updater/updater.h"
#include "globalsmanager.h"
#include "gui/notificationmanager.h"
#include "Helper/wxTranslationHelper.h"
#include "playback/playbacktraits.h"
#include "playback/playbacktab.h"
#include "defines.h"
#include "customizations.h"
#include "alsound.h"
#include "mapselectdialog.h"

#include <wx/debugrpt.h>
#include "utils/misc.h"

const unsigned int TIMER_ID         = 101;
const unsigned int TIMER_INTERVAL   = 100;

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)
END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp()
{
}

SpringLobbyApp::~SpringLobbyApp()
{
}

void SpringLobbyApp::SelectLanguage(){}

//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{
     wxLogNull logNo;
    //this needs to called _before_ mainwindow instance is created
    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxSocketBase::Initialize();

	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

//    bool usync_loaded = false;
	//unitsync first load, NEEDS to be blocking
    {
        boost::timer::auto_cpu_timer t;
        /*usync_loaded =*/ usync().ReloadUnitSyncLib();
    }
    DestroyGlobals();

    return false;
}

//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
    return 0;
}

