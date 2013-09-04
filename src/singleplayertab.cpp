/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/colordlg.h>
#include <wx/listctrl.h>
#include <wx/settings.h>

#include "singleplayertab.h"
#include "mapctrl.h"
#include "mapselectdialog.h"
#include "utils/controls.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "uiutils.h"
#include "ui.h"
#include "hosting/addbotdialog.h"
#include "server.h"
#include "settings.h"
#include "helper/colorbutton.h"
#include "aui/auimanager.h"
#include "utils/customdialogs.h"

#include <lslutils/conversion.h>

BEGIN_EVENT_TABLE(SinglePlayerTab, wxPanel)

    EVT_CHOICE( SP_MAP_PICK, SinglePlayerTab::OnMapSelect )
    EVT_CHOICE( SP_MOD_PICK, SinglePlayerTab::OnModSelect )
    EVT_BUTTON( SP_BROWSE_MAP, SinglePlayerTab::OnMapBrowse )
    EVT_BUTTON( SP_ADD_BOT, SinglePlayerTab::OnAddBot )
    EVT_BUTTON( SP_RESET, SinglePlayerTab::OnReset )
    EVT_BUTTON( SP_START, SinglePlayerTab::OnStart )
    EVT_CHECKBOX( SP_RANDOM, SinglePlayerTab::OnRandomCheck )
    EVT_CHECKBOX( SP_SPECTATE, SinglePlayerTab::OnSpectatorCheck )
    EVT_BUTTON( SP_COLOUR, SinglePlayerTab::OnColorButton )
    EVT_MOUSEWHEEL( SinglePlayerTab::OnMouseWheel )

END_EVENT_TABLE()


SinglePlayerTab::SinglePlayerTab(wxWindow* parent, MainSinglePlayerTab& msptab):
        wxScrolledWindow( parent, -1 ),
        m_battle( msptab )
{
    GetAui().manager->AddPane( this, wxLEFT, _T("singleplayertab") );

    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* m_mapabour_sizer = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* m_map_sizer = new wxBoxSizer( wxHORIZONTAL );
   // m_map_sizer->SetMinSize( wxSize( 352, -1 ) );

    m_minimap = new MapCtrl( this, 100, &m_battle, false, false, true, true );
    m_minimap->SetToolTip( TE(_("You can drag the sun/bot icon around to define start position.\n "
                                "Hover over the icon for a popup that lets you change side, ally and bonus." )) );
    m_map_sizer->Add( m_minimap, 1, wxALL | wxEXPAND, 2 );
    m_mapabour_sizer->Add(m_map_sizer, 1, wxEXPAND, 2 );

    //map description and parametrs like in battletab
    wxBoxSizer* m_opts_sizer = new wxBoxSizer( wxVERTICAL );

	m_map_opts_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 150, 160 ), wxLC_NO_HEADER | wxLC_REPORT );
	m_map_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_map_opts_list->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );

	wxListItem col;

	col.SetText( _( "Option" ) );
	m_map_opts_list->InsertColumn( 0, col );
	col.SetText( _( "Value" ) );
	m_map_opts_list->InsertColumn( 1, col );
	m_map_opts_list->SetColumnWidth( 0, 90 );
	m_map_opts_list->SetColumnWidth( 1, 50 );

	m_map_opts_list->InsertItem( 0, _( "Size" ) );
	m_map_opts_list->InsertItem( 1, _( "Windspeed" ) );
	m_map_opts_list->InsertItem( 2, _( "Tidal strength" ) );
	m_map_opts_list->InsertItem( 3, _( "Gravity" ) );
	m_map_opts_list->InsertItem( 4, _( "Extractor radius" ) );
	m_map_opts_list->InsertItem( 5, _( "Max metal" ) );

	m_opts_sizer->Add( m_map_opts_list, 0, wxALL, 2 );

    m_map_desc = new wxStaticText(this,-1,_T(""));
    m_map_desc->Wrap(160);

    m_opts_sizer->Add( m_map_desc, 0, wxALL, 2 );
    m_mapabour_sizer->Add( m_opts_sizer, 0, wxALL | wxEXPAND, 2 );
    m_main_sizer->Add( m_mapabour_sizer, 1, wxEXPAND, 5 );

    wxBoxSizer* m_ctrl_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_map_lbl = new wxStaticText( this, -1, _("Map:") );
    m_ctrl_sizer->Add( m_map_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    m_map_pick = new wxChoice( this, SP_MAP_PICK );
    m_ctrl_sizer->Add( m_map_pick, 1, wxALL, 5 );

    m_select_btn = new wxButton( this, SP_BROWSE_MAP, _T("..."), wxDefaultPosition, wxSize(CONTROL_HEIGHT, CONTROL_HEIGHT), wxBU_EXACTFIT );
    m_ctrl_sizer->Add( m_select_btn, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );

	m_mod_lbl = new wxStaticText( this, -1, _("Game:") );
    m_ctrl_sizer->Add( m_mod_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    m_mod_pick = new wxChoice( this, SP_MOD_PICK );
    m_ctrl_sizer->Add( m_mod_pick, 1, wxALL, 5 );


//  m_ctrl_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

    m_addbot_btn = new wxButton( this, SP_ADD_BOT, _("Add bot..."), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
    m_ctrl_sizer->Add( m_addbot_btn, 0, wxALL, 5 );

    m_main_sizer->Add( m_ctrl_sizer, 0, wxEXPAND, 5 );

    m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    m_main_sizer->Add( m_buttons_sep, 0, wxLEFT|wxRIGHT|wxEXPAND, 5 );

    wxBoxSizer* m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

// see http://projects.springlobby.info/issues/show/649
//  m_reset_btn = new wxButton( this, SP_RESET, _("Reset"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
//  m_buttons_sizer->Add( m_reset_btn, 0, wxALL, 5 );

    m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

    m_color_btn = new  ColorButton( this, SP_COLOUR, sett().GetBattleLastColour(), wxDefaultPosition, wxSize(30, CONTROL_HEIGHT) );
	m_buttons_sizer->Add( m_color_btn, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0 );

    m_spectator_check = new wxCheckBox( this, SP_SPECTATE, _("Spectate only") );
	m_buttons_sizer->Add( m_spectator_check, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_random_check = new wxCheckBox( this, SP_RANDOM, _("Random start positions") );
	m_buttons_sizer->Add( m_random_check, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_start_btn = new wxButton( this, SP_START, _("Start"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
	m_buttons_sizer->Add( m_start_btn, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

    SetScrollRate( SCROLL_RATE, SCROLL_RATE );
    this->SetSizer( m_main_sizer );
    this->Layout();

    ReloadMaplist();
    ReloadModlist();

}


SinglePlayerTab::~SinglePlayerTab()
{

}


void SinglePlayerTab::UpdateMinimap()
{
    m_minimap->UpdateMinimap();
}


void SinglePlayerTab::ReloadMaplist()
{
	m_map_pick->Clear();
    m_map_pick->Append(LSL::Util::vectorToArrayString(LSL::usync().GetMapList()));
    m_map_pick->Insert( _("-- Select one --"), m_map_pick->GetCount() );

    if ( m_battle.GetHostMapName() != wxEmptyString )
    {
		m_map_pick->SetStringSelection( m_battle.GetHostMapName());
        if ( m_map_pick->GetStringSelection() == wxEmptyString )
            SetMap( m_mod_pick->GetCount()-1 );
    }
    else
    {
        m_map_pick->SetSelection( m_map_pick->GetCount()-1 );
        m_addbot_btn->Enable(false);
    }
}


void SinglePlayerTab::ReloadModlist()
{
    m_mod_pick->Clear();

    const auto modlist= LSL::Util::vectorToArrayString(LSL::usync().GetModList());
    //modlist.Sort(CompareStringIgnoreCase);

    size_t nummods = modlist.Count();
    for ( size_t i = 0; i < nummods; i++ )
        m_mod_pick->Insert( modlist[i], i );

    m_mod_pick->Insert( _("-- Select one --"), m_mod_pick->GetCount() );
    if ( !m_battle.GetHostModName().IsEmpty() )
    {
        m_mod_pick->SetStringSelection( m_battle.GetHostModName() );
        if ( m_mod_pick->GetStringSelection() == wxEmptyString ) SetMod( m_mod_pick->GetCount()-1 );
    }
    else
    {
        m_mod_pick->SetSelection( m_mod_pick->GetCount()-1 );
    }
}


void SinglePlayerTab::SetMap( unsigned int index )
{
	//ui().ReloadUnitSync();
  m_addbot_btn->Enable( false );
  	if ( index >= m_map_pick->GetCount()-1 ) {
    	m_battle.SetHostMap( wxEmptyString, wxEmptyString );
 		int count=m_map_opts_list->GetItemCount();
        for(int i=0;i<count;i++)
            m_map_opts_list->SetItem( i, 1, _T(""));
        m_map_desc->SetLabel(_T(""));
	}
	else {
    try {
      	LSL::UnitsyncMap map = LSL::usync().GetMapEx( index );
      	m_battle.SetHostMap(TowxString(map.name), TowxString(map.hash));
     	m_addbot_btn->Enable( true );
		m_map_opts_list->SetItem( 0, 1, wxFormat( _T( "%dx%d" ) ) % (map.info.width / 512) % (map.info.height / 512) );
        m_map_opts_list->SetItem( 1, 1, wxFormat( _T( "%d-%d" ) ) % map.info.minWind % map.info.maxWind );
		m_map_opts_list->SetItem( 2, 1, wxFormat( _T( "%d" ) ) % map.info.tidalStrength );
		m_map_opts_list->SetItem( 3, 1, wxFormat( _T( "%d" ) ) % map.info.gravity );
		m_map_opts_list->SetItem( 4, 1, wxFormat( _T( "%d" ) ) % map.info.extractorRadius );
		m_map_opts_list->SetItem( 5, 1, wxFormat( _T( "%.3f" ) ) % map.info.maxMetal );
		m_map_desc->SetLabel(TowxString(map.info.description));
		m_map_desc->Wrap(160);
    } catch (...) {}
  }
  m_minimap->UpdateMinimap();
  m_battle.SendHostInfo( IBattle::HI_Map_Changed ); // reload map options
  m_map_pick->SetSelection( index );
}

void SinglePlayerTab::ResetUsername()
{
    m_battle.GetMe().SetNick(TowxString(LSL::usync().GetDefaultNick()));
}

void SinglePlayerTab::SetMod( unsigned int index )
{
    //ui().ReloadUnitSync();
    if ( index >= m_mod_pick->GetCount()-1 )
    {
        m_battle.SetHostMod( wxEmptyString, wxEmptyString );
    }
    else
    {
        try
        {
            LSL::UnitsyncMod mod = LSL::usync().GetMod( index );
            m_battle.SetLocalMod( mod );
            m_battle.SetHostMod(TowxString(mod.name), TowxString(mod.hash));
        }
        catch (...) {}
    }
    m_minimap->UpdateMinimap();
    m_battle.SendHostInfo( IBattle::HI_Restrictions ); // Update restrictions in options.
    m_battle.SendHostInfo( IBattle::HI_Mod_Changed ); // reload mod options
    m_mod_pick->SetSelection( index );
}


bool SinglePlayerTab::ValidSetup() const
{
    if ( (unsigned int)m_mod_pick->GetSelection() >= m_mod_pick->GetCount()-1 )
    {
		wxLogWarning( _T("no game selected") );
		customMessageBox(SL_MAIN_ICON, _("You have to select a game first."), _("Game setup error") );
        return false;
    }

    if ( (unsigned int)m_map_pick->GetSelection() >= m_map_pick->GetCount()-1 )
    {
        wxLogWarning( _T("no map selected") );
		customMessageBox(SL_MAIN_ICON, _("You have to select a map first."), _("Game setup error") );
        return false;
    }

    if ( m_battle.GetNumUsers() == 1 )
    {
        wxLogWarning(_T("trying to start sp game without bot"));
        if ( customMessageBox(SL_MAIN_ICON, _("Continue without adding a bot first?.\n The game will be over pretty fast.\n "),
                              _("No Bot added"), wxYES_NO) == wxNO )
            return false;
    }
    return true;
}


void SinglePlayerTab::OnMapSelect( wxCommandEvent& /*unused*/ )
{
    unsigned int index = (unsigned int)m_map_pick->GetCurrentSelection();
    SetMap( index );
}


void SinglePlayerTab::OnModSelect( wxCommandEvent& /*unused*/ )
{
    unsigned int index = (unsigned int)m_mod_pick->GetCurrentSelection();
    size_t num_bots = m_battle.GetNumBots();
    SetMod( index );
    if( num_bots != m_battle.GetNumBots() )
        customMessageBoxNoModal( SL_MAIN_ICON, _("Incompatible bots have been removed after game selection changed."), _("Bots removed") );
}


void SinglePlayerTab::OnMapBrowse( wxCommandEvent& /*unused*/ )
{
	wxLogDebugFunc( _T("") );
	const wxString mapname = mapSelectDialog();
	if ( !mapname.empty()) {
	        const int idx = m_map_pick->FindString( mapname, true /*case sensitive*/ );
		if ( idx != wxNOT_FOUND ) {
			SetMap( idx );
		}
	}
}


void SinglePlayerTab::OnAddBot( wxCommandEvent& /*unused*/ )
{
    AddBotDialog dlg( this, m_battle, true );
    if ( dlg.ShowModal() == wxID_OK )
    {
        UserBattleStatus bs;
        bs.owner = m_battle.GetMe().GetNick();
        bs.aishortname = dlg.GetAIShortName();
        bs.airawname = dlg.GetAiRawName();
        bs.aiversion = dlg.GetAIVersion();
        bs.aitype = dlg.GetAIType();
        bs.team = m_battle.GetFreeTeam();
        bs.ally = m_battle.GetFreeAlly();
        bs.colour = m_battle.GetNewColour();
		User& bot = m_battle.OnBotAdded( dlg.GetNick(), bs  );
        ASSERT_LOGIC( &bot != 0, _T("bot == 0") );
        m_minimap->UpdateMinimap();
    }
}

void SinglePlayerTab::OnUnitsyncReloaded( wxCommandEvent& /*data*/ )
{
    try {
        ReloadMaplist();
        ReloadModlist();
        UpdateMinimap();
    }
    catch ( ... )
    {
        wxLogDebugFunc( _T("") );
        wxLogError( _T("unitsync reload sink failed") );
    }
}


void SinglePlayerTab::OnStart( wxCommandEvent& /*unused*/ )
{
    wxLogDebugFunc( _T("SP: ") );

    if ( ui().IsSpringRunning() )
    {
        wxLogWarning(_T("trying to start spring while another instance is running") );
        customMessageBoxNoModal(SL_MAIN_ICON, _("You cannot start a spring instance while another is already running"), _("Spring error"), wxICON_EXCLAMATION );
        return;
    }

    if ( ValidSetup() ) m_battle.StartSpring();
}


void SinglePlayerTab::OnRandomCheck( wxCommandEvent& /*unused*/ )
{
    if ( m_random_check->IsChecked() )
        m_battle.CustomBattleOptions().setSingleOption("startpostype",
                                                       LSL::Util::ToString(IBattle::ST_Random), LSL::OptionsWrapper::EngineOption );
    else
        m_battle.CustomBattleOptions().setSingleOption("startpostype",
                                                       LSL::Util::ToString(IBattle::ST_Pick), LSL::OptionsWrapper::EngineOption );
    m_battle.SendHostInfo( IBattle::HI_StartType );
}

void SinglePlayerTab::OnSpectatorCheck( wxCommandEvent& /*unused*/ )
{
    m_battle.GetMe().BattleStatus().spectator = m_spectator_check->IsChecked();
    UpdateMinimap();
}

void SinglePlayerTab::OnColorButton( wxCommandEvent& /*unused*/ )
{
    User& u = m_battle.GetMe();
    wxColour CurrentColour = u.BattleStatus().colour;
    CurrentColour = GetColourFromUser(this, CurrentColour);
    if ( !CurrentColour.IsOk() ) return;
    sett().SetBattleLastColour( CurrentColour );
    m_battle.ForceColour( u, CurrentColour );
    UpdateMinimap();
}

void SinglePlayerTab::UpdateTag( const wxString& Tag )
{
    long type;
    Tag.BeforeFirst( '_' ).ToLong( &type );
    const wxString key = Tag.AfterFirst( '_' );
    if ( type == LSL::OptionsWrapper::PrivateOptions ) {
        if ( key == _T("mapname") ) {
            m_addbot_btn->Enable( false );
            try
            {
                m_map_pick->SetSelection(LSL::usync().GetMapIndex(STD_STRING(m_battle.GetHostMapName())));
                UpdateMinimap();
                m_addbot_btn->Enable( true );
            }
            catch (...) {}
        }
        else if ( key == _T("modname") ) {
            try
            {
//                int pln = m_battle.GetNumUsers();
//                int botn = m_battle.GetNumBots();
                UpdateMinimap();
//                pln -= m_battle.GetNumUsers();
//                botn -= m_battle.GetNumBots();
//                assert( pln == 0 );
//                assert( botn == 0 );

            }
            catch (...) {}
        }
    }
}

void SinglePlayerTab::UpdatePresetList()
{
}

void SinglePlayerTab::OnReset( wxCommandEvent& /*unused*/ )
{

}

void SinglePlayerTab::OnMouseWheel( wxMouseEvent& event )
{
    if ( m_minimap )
    {
        wxRect map_rect = m_minimap->GetRect();
        if ( map_rect.Contains( event.GetPosition() ) )
        {
            m_minimap->OnMouseWheel( event );
            return;
        }
    }
    event.Skip();
}
