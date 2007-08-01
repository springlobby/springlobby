//
// Class: BattleListTab
//

#include "battlelisttab.h"
#include "battlelistctrl.h"
#include "battle.h"
#include "ui.h"
#include "chatpanel.h"
#include "utils.h"

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>

BEGIN_EVENT_TABLE(BattleListTab, wxPanel)

  EVT_BUTTON              ( CHAT_SEND, BattleListTab::OnJoin     )
  EVT_LIST_ITEM_ACTIVATED ( CHAT_SEND, BattleListTab::OnListJoin )

END_EVENT_TABLE()


BattleListTab::BattleListTab( wxWindow* parent, Ui& ui ) : wxPanel( parent, -1 ),m_ui(ui)
{
  m_battle_list = new BattleListCtrl( this );
  m_filter_text = new wxStaticText( this, -1, _("Filter ") );
  m_filter_combo = new wxComboBox( this, -1, _("Show all") );
  m_join_button = new wxButton( this, BATTLE_JOIN, _("Join"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT) );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_tools_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_tools_sizer->Add( m_filter_text, 0, wxEXPAND | wxTOP, 4 );
  m_tools_sizer->Add( m_filter_combo, 0, wxEXPAND );
  m_tools_sizer->AddStretchSpacer();
  m_tools_sizer->Add( m_join_button, 0, wxEXPAND );

  m_main_sizer->Add( m_battle_list, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 2 );
  m_main_sizer->Add( m_tools_sizer, 0, wxEXPAND | wxALL, 2 );

  SetSizer( m_main_sizer );
  Layout();
}


BattleListTab::~BattleListTab()
{

}


void BattleListTab::AddBattle( Battle& battle )
{
  m_battle_list->AddBattle( battle );
}


void BattleListTab::RemoveBattle( Battle& battle )
{
  m_battle_list->RemoveBattle( battle );
}


void BattleListTab::UpdateBattle( Battle& battle )
{
  m_battle_list->UpdateBattle( battle );
}

void BattleListTab::UpdateList()
{
  m_battle_list->UpdateList();
}

void BattleListTab::OnJoin( wxCommandEvent& event )
{
  assert( m_battle_list != 0 );
  if ( m_battle_list->GetSelectedIndex() < 0 ) return;

  Battle& battle = *((Battle*)m_battle_list->GetItemData( m_battle_list->GetSelectedIndex() ));

  if ( battle.opts().ispassworded ) {
    wxPasswordEntryDialog pw( this, _("Battle password"), _("Enter password") );
    if ( pw.ShowModal() == wxID_OK ) battle.Join( STD_STRING(pw.GetValue()) );
  } else {
    battle.Join();
  }
}


void BattleListTab::OnListJoin( wxListEvent& event )
{
  assert( m_battle_list != 0 );
  if ( event.GetIndex() < 0 ) return;

  Battle& battle = *((Battle*)m_battle_list->GetItemData( event.GetIndex() ));

  if ( !battle.HasMod() ) {
    if (wxMessageBox( _("You need to download the mod before you can join this game. Do you want me to take you to the download page?"), _("Mod not awailable"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString mod = WX_STRING(battle.opts().modname);
      mod.Replace(_T(" "), _T("%20") );
      wxString url = _T("http://spring.unknown-files.net/page/search/1/14/") + mod + _T("/");
      wxLaunchDefaultBrowser( url );
    }
    return;
  }

  if ( battle.opts().ispassworded ) {
    wxPasswordEntryDialog pw( this, _("Battle password"), _("Enter password") );
    if ( pw.ShowModal() == wxID_OK ) battle.Join( STD_STRING(pw.GetValue()) );
  } else {
    battle.Join();
  }
}
