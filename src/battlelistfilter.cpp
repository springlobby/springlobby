/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleOptionsTab
//

#include <wx/tglbtn.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/event.h>
#include <string.h>

#include "battlelistfilter.h"
#include "battlelistctrl.h"
#include "battle.h"
#include "utils.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(BattleListFilter, wxDialog)

  EVT_BUTTON              ( BATTLE_FILTER_PLAYER_BUTTON   , BattleListFilter::OnPlayerButton    )
  EVT_BUTTON              ( BATTLE_FILTER_MAXPLAYER_BUTTON, BattleListFilter::OnMaxPlayerButton )
  EVT_BUTTON              ( BATTLE_FILTER_SPECTATOR_BUTTON, BattleListFilter::OnSpectatorButton )
  EVT_CHECKBOX            ( BATTLE_FILTER_ACTIV           , BattleListFilter::OnActivate        )
  EVT_TOGGLEBUTTON        ( BATTLE_FILTER_ACTIV_TB        , BattleListFilter::OnActivate        )
  EVT_CLOSE               ( BattleListFilter::OnClose                                           )
  EVT_CHOICE              ( BATTLE_FILTER_STATUS_CHOICE   , BattleListFilter::OnStatusChange    )
  EVT_CHOICE              ( BATTLE_FILTER_SPECTATOR_CHOICE, BattleListFilter::OnSpectatorChange )
  EVT_CHOICE              ( BATTLE_FILTER_MAXPLAYER_CHOICE, BattleListFilter::OnMaxPlayerChange )
  EVT_CHOICE              ( BATTLE_FILTER_PLAYER_CHOICE   , BattleListFilter::OnPlayerChange    )
  EVT_CHOICE              ( BATTLE_FILTER_RANK_CHOICE     , BattleListFilter::OnRankChange      )

  EVT_TEXT                ( BATTLE_FILTER_HOST_EDIT       , BattleListFilter::OnChange          )
  EVT_TEXT                ( BATTLE_FILTER_DESCRIPTION_EDIT, BattleListFilter::OnChange          )
  EVT_TEXT                ( BATTLE_FILTER_MAP_EDIT        , BattleListFilter::OnChange          )
  EVT_TEXT                ( BATTLE_FILTER_MOD_EDIT        , BattleListFilter::OnChange          )
  EVT_CHECKBOX            ( BATTLE_FILTER_MAP_SHOW        , BattleListFilter::OnChange          )
  EVT_CHECKBOX            ( BATTLE_FILTER_MOD_SHOW        , BattleListFilter::OnChange          )

END_EVENT_TABLE()

//BattleListFilter::BattleListFilter( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
BattleListFilter::BattleListFilter( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style , BattleListTab* parentBattleListTab) :
   wxDialog( parent, id, title, pos, size, style ),
   m_parent_battlelisttab( parentBattleListTab )

{
	wxBoxSizer* m_filter_sizer;
	m_filter_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* m_filter_headline_sizer;
	m_filter_headline_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_headline_sizer->SetMinSize( wxSize( -1,20 ) );
	m_filter_show = new wxToggleButton( this, BATTLE_FILTER_ACTIV_TB, wxT("^"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_filter_headline_sizer->Add( m_filter_show, 0, wxALL, 5 );

	m_filter_text = new wxStaticText( this, wxID_ANY, wxT("Filter:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_filter_text->Wrap( -1 );
	m_filter_text->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );

	m_filter_headline_sizer->Add( m_filter_text, 0, wxALL, 5 );


	m_filter_headline_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_filter_activ = new wxCheckBox( this, BATTLE_FILTER_ACTIV, wxT("Aktiv"), wxDefaultPosition, wxDefaultSize, 0 );

	m_filter_headline_sizer->Add( m_filter_activ, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_sizer->Add( m_filter_headline_sizer, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* m_filter_body_sizer;
	m_filter_body_sizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxVERTICAL );

	m_filter_body_sizer->SetMinSize( wxSize( -1,200 ) );
	wxBoxSizer* m_filter_body_row1_sizer;
	m_filter_body_row1_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_host_text = new wxStaticText( this, wxID_ANY, wxT("Host:"), wxDefaultPosition, wxSize( 58,-1 ), 0 );
	m_filter_host_text->Wrap( -1 );
	m_filter_host_text->SetMinSize( wxSize( 58,-1 ) );

	m_filter_body_row1_sizer->Add( m_filter_host_text, 0, wxALL, 5 );

	m_filter_host_edit = new wxTextCtrl( this, BATTLE_FILTER_HOST_EDIT, wxEmptyString, wxDefaultPosition, wxSize( 220,-1 ), 0|wxSIMPLE_BORDER );
	m_filter_host_edit->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );

	m_filter_body_row1_sizer->Add( m_filter_host_edit, 0, wxALL, 5 );


	m_filter_body_row1_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_filter_status_text = new wxStaticText( this, wxID_ANY, wxT("Status:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_filter_status_text->Wrap( -1 );
	m_filter_body_row1_sizer->Add( m_filter_status_text, 0, wxALIGN_RIGHT|wxALL, 5 );

  wxArrayString m_filter_status_choiceChoices;
	m_filter_status_choiceChoices.Add( _T("All") );
	m_filter_status_choiceChoices.Add( _T("Started") );
	m_filter_status_choiceChoices.Add( _T("Open") );
	m_filter_status_choiceChoices.Add( _T("Open/PW") );
	m_filter_status_choiceChoices.Add( _T("Closed") );
	m_filter_status_choiceChoices.Add( _T("Closed/PW") );
	m_filter_status_choiceChoices.Add( _T("Unknown") );

	m_filter_status_choice = new wxChoice( this, BATTLE_FILTER_STATUS_CHOICE , wxDefaultPosition, wxSize( 60,-1 ), m_filter_status_choiceChoices, wxSIMPLE_BORDER );
	m_filter_status_choice->SetMinSize( wxSize( 60,-1 ) );
	m_filter_status_choice->SetMaxSize( wxSize( 60,-1 ) );
	m_filter_status_choice->Select(0);

	m_filter_body_row1_sizer->Add( m_filter_status_choice, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_rank_text = new wxStaticText( this, wxID_ANY, wxT("Rank:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_filter_rank_text->Wrap( -1 );
	m_filter_body_row1_sizer->Add( m_filter_rank_text, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxArrayString m_filter_rank_choiceChoices;
	m_filter_rank_choiceChoices.Add( _T("All") );
	m_filter_rank_choiceChoices.Add( _T("1") );
	m_filter_rank_choiceChoices.Add( _T("2") );
	m_filter_rank_choiceChoices.Add( _T("3") );
	m_filter_rank_choiceChoices.Add( _T("4") );
	m_filter_rank_choiceChoices.Add( _T("5") );

	m_filter_rank_choice = new wxChoice( this, BATTLE_FILTER_RANK_CHOICE , wxDefaultPosition, wxSize( 40,-1 ), m_filter_rank_choiceChoices, wxSIMPLE_BORDER );
	m_filter_rank_choice->SetMinSize( wxSize( 40,-1 ) );
	m_filter_rank_choice->SetMaxSize( wxSize( 60,-1 ) );
	m_filter_rank_choice->Select(0);

	m_filter_body_row1_sizer->Add( m_filter_rank_choice, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_body_sizer->Add( m_filter_body_row1_sizer, 1, wxEXPAND, 5 );

	wxBoxSizer* m_filter_body_row2_sizer;
	m_filter_body_row2_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_description_text = new wxStaticText( this, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxSize( 58,-1 ), 0 );
	m_filter_description_text->Wrap( -1 );
	m_filter_description_text->SetMinSize( wxSize( 58,-1 ) );

	m_filter_body_row2_sizer->Add( m_filter_description_text, 0, wxALL, 5 );

	m_filter_description_edit = new wxTextCtrl( this, BATTLE_FILTER_DESCRIPTION_EDIT, wxEmptyString, wxDefaultPosition, wxSize( 220,-1 ), 0|wxSIMPLE_BORDER );
	m_filter_body_row2_sizer->Add( m_filter_description_edit, 0, wxALL, 5 );


	m_filter_body_row2_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_filter_player_text = new wxStaticText( this, wxID_ANY, wxT("Player:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_filter_player_text->Wrap( -1 );
	m_filter_body_row2_sizer->Add( m_filter_player_text, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_player_button = new wxButton( this, BATTLE_FILTER_PLAYER_BUTTON, wxT("="), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_filter_body_row2_sizer->Add( m_filter_player_button, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxArrayString m_filter_player_choiceChoices;
	m_filter_player_choiceChoices.Add( _T("All") );
	for (wxLongLong i = 0;i <= 32;i++) {
	  m_filter_player_choiceChoices.Add( i.ToString() );
	}
	m_filter_player_choice = new wxChoice( this, BATTLE_FILTER_PLAYER_CHOICE , wxDefaultPosition, wxSize( 40,-1 ), m_filter_player_choiceChoices, 0 );
  m_filter_player_choice->Select(0);
	m_filter_body_row2_sizer->Add( m_filter_player_choice, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_body_sizer->Add( m_filter_body_row2_sizer, 1, wxEXPAND, 5 );

	wxBoxSizer* m_filter_body_row3_sizer;
	m_filter_body_row3_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_map_text = new wxStaticText( this, wxID_ANY, wxT("Map:"), wxDefaultPosition, wxSize( 58,-1 ), 0 );
	m_filter_map_text->Wrap( -1 );
	m_filter_map_text->SetMinSize( wxSize( 58,-1 ) );

	m_filter_body_row3_sizer->Add( m_filter_map_text, 0, wxALL, 5 );

	m_filter_map_edit = new wxTextCtrl( this, BATTLE_FILTER_MAP_EDIT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0|wxSIMPLE_BORDER );
	m_filter_map_edit->SetMinSize( wxSize( 140,-1 ) );

	m_filter_body_row3_sizer->Add( m_filter_map_edit, 0, wxALL, 5 );

	m_filter_map_show = new wxCheckBox( this, BATTLE_FILTER_MAP_SHOW, wxT("Only maps i have"), wxDefaultPosition, wxDefaultSize, 0 );

	m_filter_map_show->SetMinSize( wxSize( 100,-1 ) );

	m_filter_body_row3_sizer->Add( m_filter_map_show, 0, wxALL, 5 );


	m_filter_body_row3_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_filter_maxplayer_text = new wxStaticText( this, wxID_ANY, wxT("Max.Player.:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_filter_maxplayer_text->Wrap( -1 );
	m_filter_body_row3_sizer->Add( m_filter_maxplayer_text, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_maxplayer_button = new wxButton( this, BATTLE_FILTER_MAXPLAYER_BUTTON, wxT("="), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_filter_body_row3_sizer->Add( m_filter_maxplayer_button, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxArrayString m_filter_maxplayer_choiceChoices;
	m_filter_maxplayer_choiceChoices.Add( _T("All") );
	for (wxLongLong i = 0;i <= 32;i++) {
	  m_filter_maxplayer_choiceChoices.Add( i.ToString() );
	}
	m_filter_maxplayer_choice = new wxChoice( this, BATTLE_FILTER_MAXPLAYER_CHOICE, wxDefaultPosition, wxSize( 40,-1 ), m_filter_maxplayer_choiceChoices, 0 );
	m_filter_maxplayer_choice->Select(0);
	m_filter_body_row3_sizer->Add( m_filter_maxplayer_choice, 0, wxALL, 5 );

	m_filter_body_sizer->Add( m_filter_body_row3_sizer, 1, wxEXPAND, 5 );

	wxBoxSizer* m_filter_body_row4_sizer;
	m_filter_body_row4_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_mod_text = new wxStaticText( this, wxID_ANY, wxT("Mod:"), wxDefaultPosition, wxSize( 58,-1 ), 0 );
	m_filter_mod_text->Wrap( -1 );
	m_filter_mod_text->SetMinSize( wxSize( 58,-1 ) );

	m_filter_body_row4_sizer->Add( m_filter_mod_text, 0, wxALL, 5 );

	m_filter_mod_edit = new wxTextCtrl( this, BATTLE_FILTER_MOD_EDIT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0|wxSIMPLE_BORDER );
	m_filter_mod_edit->SetMinSize( wxSize( 140,-1 ) );

	m_filter_body_row4_sizer->Add( m_filter_mod_edit, 0, wxALL, 5 );

	m_filter_mod_show = new wxCheckBox( this, BATTLE_FILTER_MOD_SHOW, wxT("Only mods i have"), wxDefaultPosition, wxDefaultSize, 0 );

	m_filter_mod_show->SetMinSize( wxSize( 100,-1 ) );

	m_filter_body_row4_sizer->Add( m_filter_mod_show, 0, wxALL, 5 );


	m_filter_body_row4_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_filter_spectator_text = new wxStaticText( this, wxID_ANY, wxT("Spectator:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_filter_spectator_text->Wrap( -1 );
	m_filter_body_row4_sizer->Add( m_filter_spectator_text, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_spectator_button = new wxButton( this, BATTLE_FILTER_SPECTATOR_BUTTON, wxT("="), wxDefaultPosition, wxSize( 16,16 ), 0 );
	m_filter_body_row4_sizer->Add( m_filter_spectator_button, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxArrayString m_filter_spectator_choiceChoices;
	m_filter_spectator_choiceChoices.Add( _T("All") );
	for (wxLongLong i = 0;i <= 32;i++) {
	  m_filter_spectator_choiceChoices.Add( i.ToString() );
	}
	m_filter_spectator_choice = new wxChoice( this, BATTLE_FILTER_SPECTATOR_CHOICE , wxDefaultPosition, wxSize( 40,-1 ), m_filter_spectator_choiceChoices, 0 );
	m_filter_spectator_choice->Select(0);
	m_filter_body_row4_sizer->Add( m_filter_spectator_choice, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_filter_body_sizer->Add( m_filter_body_row4_sizer, 1, wxEXPAND, 5 );

	m_filter_sizer->Add( m_filter_body_sizer, 1, wxEXPAND, 5 );

	m_filter_player_mode = m_equal;
	m_filter_maxplayer_mode = m_equal;
	m_filter_spectator_mode = m_equal;

  m_activ = false;

  m_filter_status_choice_value = m_all;
  m_filter_rank_choice_value = -1;
  m_filter_player_choice_value = -1;
  m_filter_maxplayer_choice_value = -1;
  m_filter_spectator_choice_value = -1;

	this->SetSizer( m_filter_sizer );
	this->Layout();
}


char BattleListFilter::_GetButtonSign(m_button_mode value)
{
  switch (value) {
    case m_equal   : return '=';
    case m_smaller : return '<';
    default        : return '>';
  }
}


BattleListFilter::m_button_mode BattleListFilter::_GetNextMode(m_button_mode value)
{
  switch (value) {
    case m_equal   : return m_smaller;
    case m_smaller : return m_bigger;
    default        : return m_equal;
  }
}

void BattleListFilter::OnPlayerButton   ( wxCommandEvent& event )
{
  m_filter_player_mode = _GetNextMode(m_filter_player_mode);
  const char player_mode = (const char)_GetButtonSign(m_filter_player_mode);
  m_filter_player_button->SetLabel( wxString( &player_mode, wxConvUTF8 ) );
  OnChange(event);
}


void BattleListFilter::OnMaxPlayerButton( wxCommandEvent& event )
{
  m_filter_maxplayer_mode = _GetNextMode(m_filter_maxplayer_mode);
  const char maxplayer_mode = (const char)_GetButtonSign(m_filter_maxplayer_mode);
  m_filter_maxplayer_button->SetLabel( wxString( &maxplayer_mode, wxConvUTF8 ) );
  OnChange(event);
}


void BattleListFilter::OnSpectatorButton( wxCommandEvent& event )
{
  m_filter_spectator_mode = _GetNextMode(m_filter_spectator_mode);
  const char spectator_mode = (const char)_GetButtonSign(m_filter_spectator_mode);
  m_filter_spectator_button->SetLabel( wxString( &spectator_mode, wxConvUTF8 ) );
  OnChange(event);
}

void BattleListFilter::OnActivate( wxCommandEvent& event )
{
  SetActiv();
}


void BattleListFilter::SetActiv()
{
  m_activ = !m_activ;
  m_filter_activ->SetValue(m_activ);
  m_filter_show->SetValue(m_activ);
  if (m_parent_battlelisttab) {
    m_parent_battlelisttab->m_filter_activ->SetValue(m_activ);
    m_parent_battlelisttab->UpdateList();
  }
}


void BattleListFilter::OnClose( wxCloseEvent& event )
{
  if (m_parent_battlelisttab) m_parent_battlelisttab->m_filter_show->SetValue(false);
  this->Hide();
}


bool BattleListFilter::_IntCompare(int a,int b,m_button_mode mode)
{
  switch (mode) {
    case m_equal   : return (a == b);
    case m_smaller : return (a <  b);
    case m_bigger  : return (a >  b);
    default        : return false;
  }
}

bool BattleListFilter::FilterBattle(Battle& battle)
{

  if (!m_activ) return true;

  //Battle Status Check
  switch (m_filter_status_choice_value) {
    case m_started  : if ( !battle.GetInGame() ) return false;
                      break;
    case m_open     : if ( battle.GetInGame() or battle.IsLocked() or battle.IsPassworded() ) return false;
                      break;
    case m_openpw   : if ( battle.GetInGame() or battle.IsLocked() or !battle.IsPassworded() ) return false;
                      break;
    case m_closed   : if ( battle.GetInGame() or !battle.IsLocked() or battle.IsPassworded() ) return false;
                      break;
    case m_closedpw : if ( battle.GetInGame() or !battle.IsLocked() or !battle.IsPassworded() ) return false;
                      break;
    case m_unknown  : return false; //UHMMMM could never happen... dont know what to ask...
    default :         ;
  }

  //Rank Check
  if ( (m_filter_rank_choice_value != -1) and (m_filter_rank_choice_value+1)*100 != battle.GetRankNeeded()) return false;

  //Player Check
  if ( (m_filter_player_choice_value != -1) and !_IntCompare( battle.GetNumUsers() - battle.GetSpectators() , m_filter_player_choice_value , m_filter_player_mode ) ) return false;

  //MaxPlayer Check
  if ( (m_filter_maxplayer_choice_value != -1) and !_IntCompare( battle.GetMaxPlayers() , m_filter_maxplayer_choice_value , m_filter_maxplayer_mode ) ) return false;

  //Spectator Check
  if ( (m_filter_spectator_choice_value != -1) and !_IntCompare( battle.GetSpectators() , m_filter_spectator_choice_value , m_filter_spectator_mode ) ) return false;

  //Only Maps i have Check
  if (m_filter_map_show->GetValue() and !battle.MapExists()) return false;

  //Only Mods i have Check
  if (m_filter_mod_show->GetValue() and !battle.ModExists()) return false;

  //Description:
  if ( !WX_STRING( battle.GetDescription() ).Upper().Contains( m_filter_description_edit->GetValue().Upper() ) ) return false;

  //Host:
  if ( !WX_STRING( battle.GetFounder().GetNick() ).Upper().Contains( m_filter_host_edit->GetValue().Upper() ) ) return false;

  //Map:
  if ( !WX_STRING( battle.GetMapName() ).Upper().Contains( m_filter_map_edit->GetValue().Upper() ) ) return false;

  //Mod:
  if ( !WX_STRING( battle.GetModName() ).Upper().Contains( m_filter_mod_edit->GetValue().Upper() ) ) return false;

  return true;
}

void BattleListFilter::OnChange   ( wxCommandEvent& event )
{
  if (!m_activ) return;
  m_parent_battlelisttab->UpdateList();
}


void BattleListFilter::OnStatusChange( wxCommandEvent& event )
{
  m_filter_status_choice_value = (m_status) (m_filter_status_choice->GetSelection()-1);
  OnChange(event);
}



void BattleListFilter::OnRankChange( wxCommandEvent& event )
{
  m_filter_rank_choice_value = m_filter_rank_choice->GetSelection()-1;
  OnChange(event);
}


void BattleListFilter::OnPlayerChange( wxCommandEvent& event )
{
  m_filter_player_choice_value = m_filter_player_choice->GetSelection()-1;
  OnChange(event);
}


void BattleListFilter::OnMaxPlayerChange( wxCommandEvent& event )
{
  m_filter_maxplayer_choice_value = m_filter_maxplayer_choice->GetSelection()-1;
  OnChange(event);
}


void BattleListFilter::OnSpectatorChange( wxCommandEvent& event )
{
  m_filter_spectator_choice_value = m_filter_spectator_choice->GetSelection()-1;
  OnChange(event);
}


bool BattleListFilter::GetActiv() const
{
  return m_activ;
}
