

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <stdexcept>
#include <wx/msgdlg.h>

#include "addbotdialog.h"
#include "settings.h"
#include "utils.h"
#include "battle.h"
#include "iunitsync.h"

#include "settings++/custom_msgbox.h"
#include "images/springlobby.xpm"
#include <wx/icon.h>

BEGIN_EVENT_TABLE( AddBotDialog, wxDialog )
    EVT_BUTTON( ADDBOT_CANCEL, AddBotDialog::OnClose )
    EVT_BUTTON( ADDBOT_ADD, AddBotDialog::OnAddBot )
    EVT_CHOICE( ADDBOT_AI, AddBotDialog::OnSelectBot )
END_EVENT_TABLE()


AddBotDialog::AddBotDialog( wxWindow* parent, IBattle& battle , bool singleplayer):
  wxDialog( parent, wxID_ANY, _("Add bot"), wxDefaultPosition, wxSize( 360,155 ) ),
  m_battle( battle ),
  m_sp(singleplayer)
{
  m_sl_icon = new wxIcon(springlobby_xpm);
	
  this->SetSizeHints( wxDefaultSize, wxDefaultSize );

  wxBoxSizer* m_main_sizer;
  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  if ( !m_sp ) {
    wxBoxSizer* m_nick_sizer;
    m_nick_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_nick_lbl = new wxStaticText( this, wxID_ANY, _("Nickname:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_nick_sizer->Add( m_nick_lbl, 1, wxALL, 5 );

    int bot = 1;
    while ( m_battle.GetBot( "Bot" + i2s(bot) ) != 0 ) {
      bot++;
    }

    m_nick = new wxTextCtrl( this, wxID_ANY, wxString::Format( _("Bot%d"), bot ), wxDefaultPosition, wxDefaultSize, 0 );
    m_nick_sizer->Add( m_nick, 2, wxALL, 5 );

    m_main_sizer->Add( m_nick_sizer, 0, wxEXPAND, 5 );
  }

  wxBoxSizer* m_ai_sizer;
  m_ai_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_ai_lbl = new wxStaticText( this, wxID_ANY, _("AI:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_ai_sizer->Add( m_ai_lbl, 1, wxALL, 5 );

  m_ai = new wxChoice( this, ADDBOT_AI );
  m_ai->SetToolTip( _("Choose the AI library to use with this bot.") );

  m_ai_sizer->Add( m_ai, 2, wxALL, 5 );

  m_main_sizer->Add( m_ai_sizer, 0, wxEXPAND, 5 );

  m_main_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_buttons_sizer;
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_cancel_btn = new wxButton( this, ADDBOT_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT), 0 );
  m_buttons_sizer->Add( m_cancel_btn, 0, wxALL, 5 );

  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_add_btn = new wxButton( this, ADDBOT_ADD, _("Add Bot"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT), 0 );
  m_buttons_sizer->Add( m_add_btn, 0, wxALL, 5 );

  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

  this->SetSizer( m_main_sizer );
  this->Layout();

  ReloadAIList();
}


wxString AddBotDialog::GetNick()
{
  wxString s = m_nick->GetValue();
  s.Replace( _T(" "), _T("_") );
  return s;
}


wxString AddBotDialog::GetAI()
{
  return m_ais[ m_ai->GetSelection() ];
}


wxString AddBotDialog::_RefineAIName( const wxString& name )
{
  wxString ret = name.BeforeLast('.').AfterLast('/');
  if ( m_ai->FindString( ret ) == wxNOT_FOUND ) return ret;
  wxString ret2;
  int i = 2;
  do {
    ret2 = ret + wxString::Format( _T(" (%d)"), i );
    i++;
  } while ( m_ai->FindString( ret2 ) != wxNOT_FOUND );
  return ret2;
}


void AddBotDialog::ReloadAIList()
{
  try {
    m_ais = usync()->GetAIList();
  } catch (...) {}

  m_ai->Clear();
  for ( unsigned int i = 0; i < m_ais.GetCount(); i++ ) m_ai->Append( _RefineAIName(m_ais[i]) );

  if ( m_ais.GetCount() > 0 ) {
    m_ai->SetStringSelection( WX_STRING(sett().GetLastAI()) );
    if ( m_ai->GetStringSelection() == wxEmptyString ) m_ai->SetSelection( 0 );
  } else {
    customMessageBox(SL_MAIN_WINDOW_PTR, m_sl_icon, _("No AI bots found in your Spring installation."), _("No bot-libs found"), wxOK);
  }
  m_add_btn->Enable( m_ai->GetStringSelection() != wxEmptyString );
}


void AddBotDialog::OnClose( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}


void AddBotDialog::OnAddBot( wxCommandEvent& event )
{
  sett().SetLastAI( STD_STRING( m_ai->GetStringSelection() ) );
  EndModal( wxID_OK );
}


void AddBotDialog::OnSelectBot( wxCommandEvent& event )
{
  m_add_btn->Enable( m_ai->GetStringSelection() != wxEmptyString );
}
