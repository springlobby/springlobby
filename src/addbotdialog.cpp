#include "addbotdialog.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/listctrl.h>

#include "settings.h"
#include "utils.h"
#include "battle.h"
#include "iunitsync.h"

#include "settings++/custom_dialogs.h"

BEGIN_EVENT_TABLE( AddBotDialog, wxDialog )
    EVT_BUTTON( ADDBOT_CANCEL, AddBotDialog::OnClose )
    EVT_BUTTON( ADDBOT_ADD, AddBotDialog::OnAddBot )
    EVT_CHOICE( ADDBOT_AI, AddBotDialog::OnSelectBot )
END_EVENT_TABLE()


AddBotDialog::AddBotDialog( wxWindow* parent, IBattle& battle , bool singleplayer):
  wxDialog( parent, wxID_ANY, _("Add bot"), wxDefaultPosition, wxDefaultSize ),
  m_battle( battle ),
  m_sp(singleplayer)
{

  this->SetSizeHints( wxDefaultSize, wxDefaultSize );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  if ( !m_sp )
  {
    wxBoxSizer* m_nick_sizer;
    m_nick_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_nick_lbl = new wxStaticText( this, wxID_ANY, _("Nickname:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_nick_sizer->Add( m_nick_lbl, 1, wxALL, 5 );

    int bot = m_battle.GetNumBots()+1;

    m_nick = new wxTextCtrl( this, wxID_ANY, wxString::Format( _T("Bot%d"), bot ), wxDefaultPosition, wxDefaultSize, 0 );
    m_nick_sizer->Add( m_nick, 2, wxALL, 5 );

    m_main_sizer->Add( m_nick_sizer, 0, wxEXPAND, 5 );
  }

  wxBoxSizer* m_ai_sizer;
  m_ai_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_ai_lbl = new wxStaticText( this, wxID_ANY, _("AI:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_ai_sizer->Add( m_ai_lbl, 1, wxALL, 5 );

  m_ai = new wxChoice( this, ADDBOT_AI );
  m_ai->SetToolTip( TE(_("Choose the AI library to use with this bot.") ) );

  m_ai_sizer->Add( m_ai, 2, wxALL, 5 );

  m_main_sizer->Add( m_ai_sizer, 0, wxEXPAND, 5 );

	if ( usync().VersionSupports( IUnitSync::USYNC_GetSkirmishAI ) )
	{
		m_ai_infos_lst = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER );
		wxListItem col;
		col.SetText( _("property") );
		col.SetImage( -1 );
		m_ai_infos_lst->InsertColumn( 0, col );
		wxListItem col2;
		col2.SetText( _("value") );
		col2.SetImage( -1 );
		m_ai_infos_lst->InsertColumn( 1, col2 );

		m_ai_infos_lst->DeleteAllItems();
		wxArrayString info = usync().GetAIInfos( m_ai->GetSelection() );
		int count = info.GetCount();
		for ( int i = 0; i < count; i = i + 3 )
		{
			long index = m_ai_infos_lst->InsertItem( i, info[i] );
			m_ai_infos_lst->SetItem( index, 0,  info[i] );
			m_ai_infos_lst->SetItem( index, 1,  info[i+1] );
		}
		m_ai_infos_lst->SetColumnWidth( 0, wxLIST_AUTOSIZE );
		m_ai_infos_lst->SetColumnWidth( 1, wxLIST_AUTOSIZE );

		m_info_sizer = new wxBoxSizer(wxVERTICAL);
		m_info_sizer->Add( m_ai_infos_lst, 1, wxALL|wxEXPAND );
		m_main_sizer->Add( m_info_sizer, 1, wxALL|wxEXPAND );
	}
	else
	{
		 this->SetSize( wxSize(-1, 155) );
		 m_main_sizer->AddStretchSpacer();
	}


  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND );

  wxBoxSizer* m_buttons_sizer;
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_cancel_btn = new wxButton( this, ADDBOT_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT), 0 );
  m_buttons_sizer->Add( m_cancel_btn, 0, wxALL );

  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND );

  m_add_btn = new wxButton( this, ADDBOT_ADD, _("Add Bot"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT), 0 );
  m_buttons_sizer->Add( m_add_btn, 0, wxALL );

  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );

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


wxString AddBotDialog::GetAIShortName()
{
	wxArrayString infos = usync().GetAIInfos( m_ai->GetSelection() );
	int namepos = infos.Index( _T("shortName") );
	if ( namepos == wxNOT_FOUND ) return m_ais[ m_ai->GetSelection() ];
	return infos[namepos +1];
}

wxString AddBotDialog::GetAIVersion()
{
	wxArrayString infos = usync().GetAIInfos( m_ai->GetSelection() );
	int namepos = infos.Index( _T("version") );
	if ( namepos == wxNOT_FOUND ) return _T("");
	return infos[namepos +1];
}

int AddBotDialog::GetAIType()
{
	return m_ai->GetSelection();
}

wxString AddBotDialog::RefineAIName( const wxString& name )
{
  wxString ret = name;
  if ( !usync().VersionSupports( IUnitSync::USYNC_GetSkirmishAI ) )
  {
		if ( ret.Contains(_T('.')) ) ret = ret.BeforeLast(_T('.'));
		if ( ret.Contains(_T('/')) ) ret = ret.AfterLast(_T('/'));
		if ( ret.Contains(_T('\\')) ) ret = ret.AfterLast(_T('\\'));
		if ( ret.Contains(_T("LuaAI:")) ) ret = ret.AfterFirst(_T(':'));
  }
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
  try
  {
    m_ais = usync().GetAIList( m_battle.GetHostModName() );
  } catch (...) {}

  m_ai->Clear();
  for ( unsigned int i = 0; i < m_ais.GetCount(); i++ ) m_ai->Append( RefineAIName(m_ais[i]) );
  if ( m_ais.GetCount() > 0 )
  {
    m_ai->SetStringSelection( sett().GetLastAI() );
    if ( m_ai->GetStringSelection() == wxEmptyString ) m_ai->SetSelection( 0 );
  }
  else
  {
    customMessageBox(SL_MAIN_ICON, _("No AI bots found in your Spring installation."), _("No bot-libs found"), wxOK );
  }
  m_add_btn->Enable( m_ai->GetStringSelection() != wxEmptyString );
  ShowAIInfo();
}


void AddBotDialog::OnClose( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}


void AddBotDialog::OnAddBot( wxCommandEvent& event )
{
  sett().SetLastAI(  m_ai->GetStringSelection() );
  EndModal( wxID_OK );
}


void AddBotDialog::OnSelectBot( wxCommandEvent& event )
{
	ShowAIInfo();
}

void AddBotDialog::ShowAIInfo()
{
  m_add_btn->Enable( m_ai->GetStringSelection() != wxEmptyString );
  if ( !usync().VersionSupports( IUnitSync::USYNC_GetSkirmishAI ) ) return;
  m_ai_infos_lst->DeleteAllItems();
  wxArrayString info = usync().GetAIInfos( m_ai->GetSelection() );
  int count = info.GetCount();
	for ( int i = 0; i < count; i = i + 3 )
	{
		long index = m_ai_infos_lst->InsertItem( i, info[i] );
		m_ai_infos_lst->SetItem( index, 0,  info[i] );
		m_ai_infos_lst->SetItem( index, 1,  info[i+1] );
	}
	m_ai_infos_lst->SetColumnWidth( 0, wxLIST_AUTOSIZE );
	m_ai_infos_lst->SetColumnWidth( 1, wxLIST_AUTOSIZE );
	Layout();
	SetSize( wxDefaultSize );
}
