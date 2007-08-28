

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/filename.h>
#include <wx/dir.h>

#include "addbotdialog.h"
#include "settings.h"
#include "utils.h"
#include "battle.h"


BEGIN_EVENT_TABLE( AddBotDialog, wxDialog )
    EVT_BUTTON( ADDBOT_CANCEL, AddBotDialog::OnClose )
    EVT_BUTTON( ADDBOT_ADD, AddBotDialog::OnAddBot )
END_EVENT_TABLE()


AddBotDialog::AddBotDialog( wxWindow* parent, IBattle& battle , bool singleplayer):
  wxDialog( parent, wxID_ANY, _("Add bot"), wxDefaultPosition, wxSize( 360,155 ) ),
  m_battle( battle ),
  m_sp(singleplayer)
{
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

  m_ai = new wxChoice( this, wxID_ANY );
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
  return m_ai->GetStringSelection();
}


void AddBotDialog::ReloadAIList()
{
  wxDir dir( WX_STRING( sett().GetSpringDir() ) + wxFileName::GetPathSeparator() + _T("AI") + wxFileName::GetPathSeparator() + _T("Bot-libs") );
  wxString filename;

  m_ai->Clear();
  if ( !dir.IsOpened() )
  {
    return;
  }

  bool cont = dir.GetFirst( &filename, wxEmptyString, wxDIR_FILES );
  while ( cont )
  {
    filename = filename.BeforeLast( '.' );
    if ( m_ai->FindString( filename ) == wxNOT_FOUND ) m_ai->Append( filename );
    cont = dir.GetNext(&filename);
  }

  if ( m_ai->GetCount() > 0 ) {
    wxString ai = WX_STRING(sett().GetLastAI());
    if ( ai == wxEmptyString ) ai = m_ai->GetString( 0 );
    m_ai->SetStringSelection( ai );
  }
}


void AddBotDialog::OnClose( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}


void AddBotDialog::OnAddBot( wxCommandEvent& event )
{
  sett().SetLastAI( STD_STRING( GetAI() ) );
  EndModal( wxID_OK );
}

