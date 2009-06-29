/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/log.h>
#include <stdexcept>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif


#include "aui/auimanager.h"
#include "battlelisttab.h"
#include "battlelistctrl.h"
#include "battle.h"
#include "ui.h"
#include "chatpanel.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "uiutils.h"
#include "hostbattledialog.h"
#include "server.h"
#include "settings.h"
#include "iunitsync.h"
#include "mapctrl.h"
#include "nicklistctrl.h"
#include "mainwindow.h"
#include "mainjoinbattletab.h"
#include "battlelistfilter.h"
#include "iconimagelist.h"
#include "useractions.h"
#include "settings++/custom_dialogs.h"

const unsigned int BATTLELIST_COLUMNCOUNT = 10;

BEGIN_EVENT_TABLE(BattleListTab, wxPanel)

  EVT_BUTTON              ( BattleListTab::BATTLE_JOIN , BattleListTab::OnJoin        )
  EVT_BUTTON              ( BattleListTab::BATTLE_HOST , BattleListTab::OnHost        )
  EVT_LIST_ITEM_ACTIVATED ( BattleListTab::BATTLE_JOIN , BattleListTab::OnListJoin    )
  EVT_LIST_ITEM_SELECTED  ( BattleListCtrl::BLIST_LIST  , BattleListTab::OnSelect      )
  EVT_CHECKBOX            ( BattleListTab::BATTLE_LIST_FILTER_ACTIV, BattleListTab::OnFilterActiv )
#if  wxUSE_TOGGLEBTN
  EVT_TOGGLEBUTTON        ( BattleListTab::BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter  )
  EVT_TOGGLEBUTTON        ( BattleListTab::BATTLE_LIST_INFO_BUTTON, BattleListTab::OnInfoShow  )
#else
  EVT_CHECKBOX            ( BattleListTab::BATTLE_LIST_FILTER_BUTTON , BattleListTab::OnFilter )
  EVT_CHECKBOX            ( BattleListTab::BATTLE_LIST_INFO_BUTTON, BattleListTab::OnOnInfoShow )
#endif
  EVT_SIZE( BattleListTab::OnResize )


END_EVENT_TABLE()


BattleListTab::BattleListTab( wxWindow* parent, Ui& ui ) : wxScrolledWindow( parent, -1 ),
  m_ui(ui),
  m_sel_battle(0)
{
  GetAui().manager->AddPane( this, wxLEFT, _T("battlelisttab") );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* m_filter_sizer;
  m_filter_sizer = new wxBoxSizer( wxVERTICAL );


  m_battlelist_sizer = new wxBoxSizer( wxVERTICAL );

  m_battle_list = new BattleListCtrl( this, m_ui );
  m_battle_list->SetHighLightAction ( UserActions::ActHighlight );
  m_battlelist_sizer->Add( m_battle_list, 1, wxALL|wxEXPAND, 5 );

  m_main_sizer->Add( m_battlelist_sizer, 1, wxEXPAND, 5 );;

  m_info_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_minimap = new MapCtrl( this, 100, 0, m_ui, true, true, false, false );
  m_info_sizer->Add( m_minimap, 0, wxALL, 5 );

  m_data_sizer = new wxFlexGridSizer( 4, 2, 0, 0 );

  m_map_lbl = new wxStaticText( this, wxID_ANY, _("Map:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_map_lbl, 0, wxALL, 5 );

  m_map_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_map_text, 0, wxALL, 5 );

  m_mod_lbl = new wxStaticText( this, wxID_ANY, _("Mod:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_mod_lbl, 0, wxALL, 5 );

  m_mod_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_mod_text, 0, wxALL, 5 );

  m_players_lbl = new wxStaticText( this, wxID_ANY, _("Players:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_players_lbl, 0, wxALL, 5 );

  m_players_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_players_text, 0, wxALL, 5 );

  m_spec_lbl = new wxStaticText( this, wxID_ANY, _("Spectators:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_spec_lbl, 0, wxALL, 5 );

  m_spec_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_spec_text, 0, wxALL, 5 );

  m_info_sizer->Add( m_data_sizer, 1, wxEXPAND, 5 );

  m_players = new NickListCtrl( this, false );
  m_info_sizer->Add( m_players, 1, wxALL|wxEXPAND, 5 );

  m_main_sizer->Add( m_info_sizer, 0, wxEXPAND, 5 );


  m_filter = new BattleListFilter( this , wxID_ANY, this ,wxDefaultPosition, wxSize( -1,-1 ), wxEXPAND );
  m_filter_sizer->Add( m_filter, 0, wxEXPAND, 5);

  m_main_sizer->Add( m_filter_sizer, 0, wxEXPAND, 5);

  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_buttons_sizer;
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

#if  wxUSE_TOGGLEBTN
	m_filter_show = new wxToggleButton( this, BATTLE_LIST_FILTER_BUTTON , _(" Filter "), wxDefaultPosition , wxSize( -1,28 ), 0 );
#else
	m_filter_show = new wxCheckBox( this, BATTLE_LIST_FILTER_BUTTON , _(" Filter "), wxDefaultPosition , wxSize( -1,28 ), 0 );
#endif
  m_buttons_sizer->Add( m_filter_show, 0, 0, 5 );

  #if  wxUSE_TOGGLEBTN
	m_info_show = new wxToggleButton( this, BATTLE_LIST_INFO_BUTTON , _(" Battle infos "), wxDefaultPosition , wxSize( -1,28 ), 0 );
#else
	m_info_show = new wxCheckBox( this, BATTLE_LIST_INFO_BUTTON, _(" Battle infos "), wxDefaultPosition , wxSize( -1,28 ), 0 );
#endif
  m_buttons_sizer->Add( m_info_show , 0, 0, 5 );

  m_filter_activ = new wxCheckBox( this, BATTLE_LIST_FILTER_ACTIV , _("Activated"), wxDefaultPosition, wxDefaultSize, 0 );
  m_buttons_sizer->Add( m_filter_activ, 0, wxALL, 5 );


  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
  m_battle_num = new wxStaticText( this, wxID_ANY, _("0 battles displayed"), wxDefaultPosition, wxDefaultSize, 0 );
  m_buttons_sizer->Add( m_battle_num, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 4 );
  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_host_btn = new wxButton( this, BATTLE_HOST, _("Host new..."), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_host_btn, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

  m_join_btn = new wxButton( this, BATTLE_JOIN, _("Join"), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_join_btn, 0, wxBOTTOM|wxRIGHT, 5 );

  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

  m_filter->Hide();

  this->SetSizer( m_main_sizer );
  this->Layout();

  SelectBattle(0);
}


BattleListTab::~BattleListTab()
{
    if (m_filter != 0)
        m_filter->SaveFilterValues();
}

void BattleListTab::SetNumDisplayed()
{
    int num = m_battle_list->GetItemCount();
    m_battle_num->SetLabel( wxString::Format( _("%d battles displayed"), num ) );
}

void BattleListTab::SelectBattle( IBattle* battle )
{
  m_sel_battle = battle;
  m_minimap->SetBattle( m_sel_battle );
  m_players->ClearUsers();
  if ( m_sel_battle != 0 )
  {
    m_map_text->SetLabel( RefineMapname( m_sel_battle->GetHostMapName() ) );
    m_mod_text->SetLabel( m_sel_battle->GetHostModName() );
    m_players_text->SetLabel( wxString::Format( _T("%d / %d"), int(m_sel_battle->GetNumUsers()) - int(m_sel_battle->GetSpectators()), int(m_sel_battle->GetMaxPlayers() )) );
    m_spec_text->SetLabel( wxString::Format( _T("%d"), m_sel_battle->GetSpectators() ) );
    for ( unsigned int i = 0; i < m_sel_battle->GetNumUsers(); i++ )
    {
    	User& usr = m_sel_battle->GetUser( i );
    	if ( usr.BattleStatus().IsBot() ) continue;
      m_players->AddUser( usr );
    }
  } else
  {
    m_map_text->SetLabel( wxEmptyString );
    m_mod_text->SetLabel( wxEmptyString );
    m_players_text->SetLabel(  _T("0 / 0") );
    m_spec_text->SetLabel( _T("0") );
  }
}

void BattleListTab::AddBattle( IBattle& battle ) {
    if ( battle.GetGUIListActiv() || ( m_filter->GetActiv() && !m_filter->FilterBattle( battle ) ) ) {
        return;
    }

    m_battle_list->AddBattle( battle );
    battle.SetGUIListActiv( true );
    m_battle_list->MarkDirtySort();
    SetNumDisplayed();
}


void BattleListTab::RemoveBattle( IBattle& battle )
{

    if ( &battle == m_sel_battle ) {
        m_battle_list->ResetSelection();
        SelectBattle( 0 );
    }

    m_battle_list->RemoveBattle( battle );

    battle.SetGUIListActiv( false );
    SetNumDisplayed();
}


void BattleListTab::UserUpdate( User& user )
{
  if (m_sel_battle && user.GetBattle() == m_sel_battle) { m_players->UserUpdated( user ); }
}


void BattleListTab::UpdateBattle( IBattle& battle )
{
    if ( !battle.GetGUIListActiv() ) {
        AddBattle( battle );
        return;
    }

    if ( m_filter->GetActiv() && !m_filter->FilterBattle( battle ) ) {
        RemoveBattle( battle );
        return;
    }

    m_battle_list->UpdateBattle( battle );

    if ( &battle == m_sel_battle )
        SelectBattle( m_sel_battle );

}


void BattleListTab::RemoveAllBattles()
{
  SelectBattle( 0 );
  m_ui.GetServer().battles_iter->IteratorBegin();
  while (! m_ui.GetServer().battles_iter->EOL() )
  {
    Battle* temp_battle = m_ui.GetServer().battles_iter->GetBattle();
    if (temp_battle != 0)
        temp_battle->SetGUIListActiv( false );
  }
  m_battle_list->Clear();
  SetNumDisplayed();
}


void BattleListTab::UpdateList() {
    m_ui.GetServer().battles_iter->IteratorBegin();
    while (! m_ui.GetServer().battles_iter->EOL() ) {
        Battle* b = m_ui.GetServer().battles_iter->GetBattle();
        if (b!=0)
            UpdateBattle(*b);
    }
    m_battle_list->RefreshVisibleItems();
}


void BattleListTab::SetFilterActiv( bool activ )
{
  m_filter->SetActiv( activ );
  m_filter_activ->SetValue( activ );
  sett().SetBattleFilterActivState( activ );
  m_battle_list->MarkDirtySort();
}


void BattleListTab::OnHost( wxCommandEvent& event )
{
  if ( !m_ui.IsConnected() )
  {
    wxLogWarning( _T("Trying to host while offline") );
    customMessageBoxNoModal(SL_MAIN_ICON, _("You cannot host a game while being offline. Please connect to a lobby server."), _("Not Online."), wxOK );
    m_ui.ShowConnectWindow();
    return;
  }
  if ( !m_ui.IsSpringCompatible() )
  {
    wxLogWarning(_T("Hosting is disabled due to the incompatible version ") );
    customMessageBoxNoModal(SL_MAIN_ICON,_("Hosting is disabled due to the incompatible version you're using"), _("Spring error"), wxICON_EXCLAMATION|wxOK);
    return;
  }
  if ( m_ui.IsSpringRunning() )
  {
    wxLogWarning(_T("trying to host while spring is running") );
    customMessageBoxNoModal(SL_MAIN_ICON,_("You already are running a Spring instance, close it first in order to be able to host a new game"), _("Spring error"), wxICON_EXCLAMATION|wxOK );
    return;
  }
  Battle* battle = m_ui.mw().GetJoinTab().GetCurrentBattle();
  if ( battle != 0 )
   {
    if ( m_ui.Ask( _("Already in a battle"), _("You are already in a battle.\n\nDo you want to leave current battle to start a new?") ) ) {
      battle->Leave();
      m_ui.mw().GetJoinTab().LeaveCurrentBattle();
    }
    else
    {
      return;
    }
  }

  HostBattleDialog dlg( this );
  if ( dlg.ShowModal() == wxID_OK )
  {
    BattleOptions bo;
    bo.description = sett().GetLastHostDescription();
    bo.port = sett().GetLastHostPort();
    bo.nattype = NatType(sett().GetLastHostNATSetting());

    if ( bo.nattype == NAT_None && sett().GetTestHostPort() )
    {
        switch ( m_ui.TestHostPort( bo.port ) )
        {
            case Server::porttest_pass : break; // success
            case Server::porttest_pass_WX26 :
                wxLogWarning(_T("hosting port %d: test aborted (wx26)"),bo.port  );
                customMessageBoxNoModal( SL_MAIN_ICON, wxString::Format( _("Your using wxWidgets prior to version 2.8,\n "
                    "port testing is not supported.\n Hosting may or may not work."), bo.port ) );
                sett().SetTestHostPort(false); // no need to have it checked anymore
                break;

            case Server::porttest_unreachable :
                wxLogWarning(_T("hosting port %d: test undetermined"),bo.port  );
                customMessageBoxNoModal( SL_MAIN_ICON, wxString::Format( _("The server used for testing your port %d "
                    "is unreachable. \nHosting may or may not work with this setting."), bo.port ) );
                break; //inconclusive test shouldn't hinder hosting imo (koshi)

            case Server::porttest_timeout :
            case Server::porttest_socketNotOk :
            case Server::porttest_socketError :
                wxLogWarning(_T("hosting port %d: test unsuccessful, closing battle"),bo.port  );
                customMessageBoxNoModal( SL_MAIN_ICON, wxString::Format( _("Battle not started because the port you selected (%d) "
                    "is unable to recieve incoming packets\n checks your router & firewall configuration again or change port "
                    "in the dialog.\n\nIf everything else fails, enable the Hole Punching NAT Traversal\n "
                    "option in the hosting settings."), bo.port ) );
                return;

        }
      if ( !m_ui.TestHostPort( bo.port ) )
      {
        wxLogWarning(_T("hosting port %d: test unsuccessful, closing battle"),bo.port  );
        customMessageBoxNoModal( SL_MAIN_ICON, wxString::Format( _("Battle not started because the port you selected (%d) is unable to recieve incoming packets\n checks your router & firewall configuration again or change port in the dialog.\n\nIf everything else fails, enable the Hole Punching NAT Traversal\n option in the hosting settings."), bo.port ) );
        return;
      }
    }

    // Get selected mod from unitsync.
    UnitSyncMod mod;
    try
    {
      mod = usync().GetMod( sett().GetLastHostMod() );
      bo.modhash = mod.hash;
      bo.modname = mod.name;
    }
    catch ( ... )
    {
      wxLogWarning( _T("can't host: mod not found") );
      customMessageBoxNoModal( SL_MAIN_ICON,_("Battle not started beacuse the mod you selected could not be found. "), _("Error starting battle."), wxOK );
      return;
    }

    UnitSyncMap map;
    wxString mname = sett().GetLastHostMap();
    try {
      if ( usync().MapExists(mname) )
    	  map = usync().GetMap( mname );
      else if ( usync().GetNumMaps() <= 0 )
      {
        wxLogWarning( _T("no maps found") );
        customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't find any maps in your spring installation. This could happen when you set the Spring settings incorrectly."), _("No maps found"), wxOK );
        return;
      }
      else
      {
        map = usync().GetMap( 0 );
      }
    }
    catch ( ... )
    {
      wxLogWarning( _T("no maps found") );
      customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't find any maps in your spring installation. This could happen when you set the Spring settings incorrectly."), _("No maps found"), wxOK );
      return;
    }
    bo.maphash = map.hash;
    bo.mapname = map.name;

    bo.rankneeded = sett().GetLastRankLimit();

    bo.maxplayers = sett().GetLastHostPlayerNum();

    bo.isproxy = sett().GetLastHostRelayedMode();

    m_ui.GetServer().HostBattle( bo, sett().GetLastHostPassword() );
  }
}


void BattleListTab::OnFilter( wxCommandEvent& event )
{
  if (m_filter_show->GetValue())
	{
    m_filter->Show(  );
    this->Layout();
  }
  else
  {
    m_filter->Hide(  );
    this->Layout();
  }
}


void BattleListTab::OnFilterActiv( wxCommandEvent& event )
{
  bool active = m_filter_activ->GetValue();
  if ( !m_ui.IsConnected() )
  {
    m_filter_activ->SetValue( !active );
    return;
  }
  m_filter->SetActiv( active );
  sett().SetBattleFilterActivState( active );
  SetNumDisplayed();
}


void BattleListTab::OnJoin( wxCommandEvent& event )
{
    try
    {
        ASSERT_LOGIC( m_battle_list != 0, _T("m_battle_list = 0") );
    } catch (...) { return; }

    if ( m_battle_list->GetSelectedIndex() < 0 ) return;

    int id = m_battle_list->GetSelectedData()->GetBattleId();
    DoJoin( m_ui.GetServer().battles_iter->GetBattle( id ) );
}


void BattleListTab::OnListJoin( wxListEvent& event )
{
    try
    {
        ASSERT_LOGIC( m_battle_list != 0, _T("m_battle_list = 0") );
    } catch (...) { return; }
    if ( event.GetIndex() < 0 ) return;

    int id = m_battle_list->GetSelectedData()->GetBattleId();
    DoJoin( m_ui.GetServer().battles_iter->GetBattle( id ) );
}


void BattleListTab::DoJoin( Battle& battle )
{
  if ( !m_ui.IsSpringCompatible() )
  {
    wxLogWarning(_T("trying to join battles with imcompatible spring version") );
    customMessageBox(SL_MAIN_ICON,_("Joining battles is disabled due to the incompatible spring version you're using."), _("Spring error"), wxICON_EXCLAMATION|wxOK);
    return;
  }

  Battle* curbattle = m_ui.mw().GetJoinTab().GetCurrentBattle();
  if ( curbattle != 0 )
  {
    if ( m_ui.Ask( _("Already in a battle"), _("You are already in a battle.\n\nDo you want to leave current battle to and join this one?") ) ) {
      curbattle->Leave();
      m_ui.mw().GetJoinTab().LeaveCurrentBattle();
    }
    else
    {
      return;
    }
  }

  if ( m_ui.IsSpringRunning() )
  {
    wxLogWarning(_T("trying to join a battle while spring is running") );
    customMessageBox(SL_MAIN_ICON,_("You already are running a Spring instance, close it first in order to be able to join another battle."), _("Spring error"), wxICON_EXCLAMATION|wxOK );
    return;
  }

  #ifdef NO_TORRENT_SYSTEM
      wxString downloadProc = _("Do you want me to take you to the download page?");
  #else
      wxString downloadProc = _("Should i try to download it for you?\nYou can see the progress in the \"Download Manager\" tab.");
  #endif

  if ( !battle.ModExists() )
  {
    if (customMessageBox( SL_MAIN_ICON, _("You need to download the mod before you can join this game.\n\n") + downloadProc, _("Mod not available"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString modhash = battle.GetHostModHash();
      wxString modname = battle.GetHostModName();
      m_ui.DownloadMod ( modhash, modname );
    }
    return;
  }

  if ( !battle.MapExists() )
  {
    if (customMessageBox(SL_MAIN_ICON, _("You need to download the map to be able to play in this game.\n\n") + downloadProc, _("Map not available"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString maphash = battle.GetHostMapHash();
      wxString mapname = battle.GetHostMapName();
      m_ui.DownloadMap ( maphash, mapname );
    }
  }

  if ( battle.IsPassworded() )
  {
    wxPasswordEntryDialog pw( this, _("Battle password"), _("Enter password") );
    pw.SetFocus();
    if ( pw.ShowModal() == wxID_OK ) battle.Join( pw.GetValue() );
  }
  else
  {
    battle.Join();
  }
}


void BattleListTab::OnSelect( wxListEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( event.GetIndex() == -1 )
  {
    SelectBattle( 0 );
  } else {
      IBattle* b = ( m_battle_list->GetDataFromIndex( event.GetIndex() ) ) ;
    SelectBattle( b );//
  }
}


void BattleListTab::OnUnitSyncReloaded()
{
  if ( ! m_ui.GetServerStatus() ) { return; }

  m_ui.GetServer().battles_iter->IteratorBegin();
  while (! m_ui.GetServer().battles_iter->EOL() )
  {
    Battle* b = m_ui.GetServer().battles_iter->GetBattle();
    if (b!=0) b->OnUnitSyncReloaded();
  }
  UpdateList();
  m_minimap->UpdateMinimap();
}

void BattleListTab::UpdateHighlights()
{
    m_battle_list->RefreshVisibleItems();
}


void BattleListTab::SortBattleList()
{
  m_battle_list->SortList();
}

void BattleListTab::ShowExtendedInfos( bool show )
{
    const bool recursive = true;
    m_main_sizer->Show(m_info_sizer, show, recursive );
    m_main_sizer->Show(m_buttons_sep, show, recursive  );
    m_info_show->SetValue( show );
    Layout();
}

void BattleListTab::OnInfoShow( wxCommandEvent& event )
{
    ShowExtendedInfos( m_info_show->GetValue() );
}

void BattleListTab::OnResize( wxSizeEvent& event )
{
	SetSize( event.GetSize() );
	Layout();
	// window too small, hide additional infos
    ShowExtendedInfos( ( GetClientSize().GetHeight() > 400 ) );
}

