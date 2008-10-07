/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: MainOptionsTab
//

#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/button.h>
#include <wx/sizer.h>

#ifndef HAVE_WX26
#include "auimanager.h"
#else
#include <wx/listbook.h>
#endif


#include "mainoptionstab.h"
#include "ui.h"
#include "springoptionstab.h"
#include "chatoptionstab.h"
#include "settings.h"
#include "uiutils.h"
#include "groupoptionspanel.h"

#ifndef NO_TORRENT_SYSTEM
#include "torrentoptionspanel.h"
#endif

#include "lobbyoptionstab.h"
#include "images/torrentoptionspanel_icon.png.h"
#include "images/spring.xpm"
#include "images/userchat.xpm"
#include "images/springlobby.xpm"


BEGIN_EVENT_TABLE(MainOptionsTab, wxPanel)

    EVT_BUTTON ( wxID_APPLY, MainOptionsTab::OnApply )
    EVT_BUTTON ( wxID_REVERT, MainOptionsTab::OnRestore )

END_EVENT_TABLE()

/** \brief A container for the various option panels
 * Contains a notebook holding the real option panels as pages. Handles "apply" and "restore" events for those pages,
 * rather then those having to implement (and duplicate) this functionality. \n
 * See SpringOptionsTab, TorrentOptionsPanel, ChatOptionsTab
 */
MainOptionsTab::MainOptionsTab( wxWindow* parent, Ui& ui ) : wxScrolledWindow( parent, -1 ),m_ui(ui)
{
    #ifdef HAVE_WX26
    m_tabs = new wxNotebook( this, OPTIONS_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );
    #else
    GetAui().manager->AddPane( this, wxLEFT, _T("mainoptionstab") );
    m_tabs = new wxAuiNotebook( this, OPTIONS_TABS, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE );
    #endif
    m_imagelist = new wxImageList( 12, 12 );
    m_imagelist->Add( wxIcon(spring_xpm) );
    m_imagelist->Add( *charArr2wxBitmap( torrentoptionspanel_icon_png, sizeof(torrentoptionspanel_icon_png) )  );
    m_imagelist->Add( wxIcon(userchat_xpm) );
    m_imagelist->Add( wxIcon(userchat_xpm) );
    m_imagelist->Add( wxIcon(springlobby_xpm) );

    #ifdef HAVE_WX26
    m_tabs->AssignImageList( m_imagelist );
    #endif

    m_spring_opts = new SpringOptionsTab( m_tabs, m_ui );
    m_tabs->AddPage( m_spring_opts, _("Spring"), true, 0 );

#ifndef NO_TORRENT_SYSTEM
    m_torrent_opts = new TorrentOptionsPanel( m_tabs, m_ui );
    #ifdef HAVE_WX26
    m_tabs->AddPage( m_torrent_opts, _("P2P"), true, 1 );
    #else
    m_tabs->AddPage( m_torrent_opts, _("P2P"), true, *charArr2wxBitmap( torrentoptionspanel_icon_png, sizeof(torrentoptionspanel_icon_png) ) );
    #endif
#endif

    m_chat_opts = new ChatOptionsTab( m_tabs, m_ui );
    #ifdef HAVE_WX26
    m_tabs->AddPage( m_chat_opts, _("Chat"), true, 2 );
    #else
    m_tabs->AddPage( m_chat_opts, _("Chat"), true, wxIcon(userchat_xpm) );
    #endif

    m_groups_opts = new GroupOptionsPanel( m_tabs );
    #ifdef HAVE_WX26
    m_tabs->AddPage( m_groups_opts , _("Groups"), true, 2 );
    #else
    m_tabs->AddPage( m_groups_opts , _("Groups"), true, wxIcon(userchat_xpm) );
    #endif

    m_lobby_opts = new LobbyOptionsTab( m_tabs );

   #ifdef HAVE_WX26
     m_tabs->AddPage ( m_lobby_opts, _("General"), true, 4 );
    #else

    #endif

    m_groups_opts = new GroupOptionsPanel( m_tabs );

    #ifdef HAVE_WX26
    m_tabs->AddPage( m_groups_opts, _("Groups"), true, 2 );
    #else
    m_tabs->AddPage ( m_lobby_opts, _("Groups"), true, wxIcon(userchat_xpm) );
    #endif


    m_restore_btn = new wxButton( this, wxID_REVERT, _("Restore") );
    m_apply_btn = new wxButton( this, wxID_APPLY, _("Apply") );

    m_button_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_button_sizer->Add( m_restore_btn, 0, wxALL, 2 );
    m_button_sizer->AddStretchSpacer();
    m_button_sizer->Add( m_apply_btn, 0, wxALL, 2 );

    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_main_sizer->Add( m_tabs, 1, wxEXPAND );
    m_main_sizer->Add( m_button_sizer, 0, wxEXPAND );

    SetSizer( m_main_sizer );
    SetScrollRate( 3, 3 );
    Layout();
}


MainOptionsTab::~MainOptionsTab()
{

}


GroupOptionsPanel& MainOptionsTab::GetGroupOptionsPanel()
{
  ASSERT_EXCEPTION(m_groups_opts != 0, _T("m_groups_opts == 0"));
  return *m_groups_opts;
}


void MainOptionsTab::OnApply( wxCommandEvent& event )
{
    m_spring_opts->OnApply( event );
    m_chat_opts->OnApply( event );
#ifndef NO_TORRENT_SYSTEM
    m_torrent_opts->OnApply( event );
#endif
    m_lobby_opts->OnApply( event );

    sett().SaveSettings();
}


void MainOptionsTab::OnRestore( wxCommandEvent& event )
{
    m_spring_opts->OnRestore( event );
    m_chat_opts->OnRestore( event );
#ifndef NO_TORRENT_SYSTEM
    m_torrent_opts->OnRestore( event );
#endif

    m_lobby_opts->OnRestore ( event );
}

void MainOptionsTab::OnOpenGroupsTab()
{
    //m_groups_opts->ReloadGroupSizer();
}

void MainOptionsTab::SetSelection( const unsigned int page )
{
    if ( page < m_tabs->GetPageCount() ){
        m_tabs->SetSelection( page );
        //m_groups_opts->ReloadGroupSizer();
    }
    else
        m_tabs->SetSelection( 0 );
}


void MainOptionsTab::ReloadSpringPathFromConfig()
{
  m_spring_opts->ReloadSpringPathFromConfig();
}
