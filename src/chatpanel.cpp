//
// Class: ChatPanel
//

#include <stdexcept>
#include <wx/intl.h>
#include <wx/datetime.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/combobox.h>
#include <wx/button.h>

#include "channel.h"
#include "chatpanel.h"
#include "utils.h"
#include "ui.h"
#include "server.h"
#include "user.h"
#include "battle.h"
#include "nicklistctrl.h"


BEGIN_EVENT_TABLE(ChatPanel, wxPanel)

  EVT_TEXT_ENTER ( CHAT_TEXT, ChatPanel::OnSay   )
  EVT_BUTTON     ( CHAT_SEND, ChatPanel::OnSay   )
  EVT_SIZE       (            ChatPanel::OnResize)

END_EVENT_TABLE()

/*
//! @brief ChatPanel constructor.
//!
//! @param parent the parent wxWindow.
//! @param show_nick_list set to true if the nick list should be created.
//! @todo Fix nick list
//! @todo Enlarge the say input.
ChatPanel::ChatPanel( wxWindow* parent, bool show_nick_list ) : wxPanel( parent, -1 )
{
  // Setting default values
  m_show_nick_list = show_nick_list;
  m_channel = 0;

  _CreateControls();

}
*/

ChatPanel::ChatPanel( wxWindow* parent, Ui& ui,  Channel& chan )
: wxPanel( parent, -1),m_show_nick_list(true),m_ui(ui),m_channel(&chan),m_server(0),m_user(0),m_battle(0),m_type(CPT_Channel)
{
  debug_func( "wxWindow* parent, Channel& chan" );
  _CreateControls( );
  _SetChannel( &chan );
}

ChatPanel::ChatPanel( wxWindow* parent, Ui& ui,  User& user )
: wxPanel( parent, -1),m_show_nick_list(false),m_ui(ui),m_channel(0),m_server(0),m_user(&user),m_battle(0),m_type(CPT_User)
{
  _CreateControls( );
  user.uidata.panel = this;
}

ChatPanel::ChatPanel( wxWindow* parent, Ui& ui,  Server& serv )
: wxPanel( parent, -1),m_show_nick_list(false),m_ui(ui),m_channel(0),m_server(&serv),m_user(0),m_battle(0),m_type(CPT_Server)
{
  debug_func( "wxWindow* parent, Server& serv" );
  _CreateControls( );
  serv.uidata.panel = this;
}

ChatPanel::ChatPanel( wxWindow* parent, Ui& ui,  Battle& battle )
: wxPanel( parent, -1),m_show_nick_list(false),m_ui(ui),m_channel(0),m_server(0),m_user(0),m_battle(&battle),m_type(CPT_Battle)
{
  debug_func( "wxWindow* parent, Battle& battle" );
  _CreateControls( );
  //serv.uidata.panel = this;
};

void ChatPanel::_CreateControls( )
{
  debug_func( "" );

  // Creating sizers
  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_chat_sizer = new wxBoxSizer( wxVERTICAL );
  m_say_sizer = new wxBoxSizer( wxHORIZONTAL );

  if ( m_show_nick_list ) {

    m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_nick_panel = new wxPanel( m_splitter, -1 );
    m_chat_panel = new wxPanel( m_splitter, -1 );

    m_nick_sizer = new wxBoxSizer( wxVERTICAL );

    m_nicklist = new NickListCtrl( m_nick_panel );

    m_nick_filter = new wxComboBox( m_nick_panel, -1, _T("Show all"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT), 0, 0, wxCB_READONLY );

    m_nick_sizer->Add( m_nicklist, 1, wxEXPAND );
    m_nick_sizer->Add( m_nick_filter, 0, wxEXPAND | wxTOP, 2 );

    m_nick_panel->SetSizer( m_nick_sizer );

  } else {

    m_chat_panel = this;
    m_nick_sizer = 0;
    m_nicklist = 0;
    m_nick_filter = 0;
    m_splitter = 0;

  }

  // Creating ui elements
  m_chatlog_text = new wxTextCtrl( m_chat_panel, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                             wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );

  m_say_text = new wxTextCtrl( m_chat_panel, CHAT_TEXT, _T(""), wxDefaultPosition, wxSize(100,CONTROL_HEIGHT), wxTE_PROCESS_ENTER );
  m_say_button = new wxButton( m_chat_panel, CHAT_SEND, _("Send"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT) );

  // Adding elements to sizers
  m_say_sizer->Add( m_say_text, 1, wxEXPAND );
  m_say_sizer->Add( m_say_button );

  m_chat_sizer->Add( m_chatlog_text, 1, wxEXPAND );
  m_chat_sizer->Add( m_say_sizer, 0, wxEXPAND | wxTOP, 2 );

  if ( m_show_nick_list ) {
    m_chat_panel->SetSizer( m_chat_sizer );

    m_splitter->SplitVertically( m_chat_panel, m_nick_panel, 100 );

    m_main_sizer->Add( m_splitter, 1, wxEXPAND | wxALL, 2 );

  } else {
    m_main_sizer->Add( m_chat_sizer, 4, wxEXPAND | wxALL, 2 );
  }

  // Assign sizer to panel
  SetSizer( m_main_sizer );


  if ( m_show_nick_list ) {
    wxSize s = m_splitter->GetSize();
    m_splitter->SetSashPosition( s.GetWidth() - 238, true );
  }
}


//! @brief ChatPanel destructor.
ChatPanel::~ChatPanel()
{
  if ( m_server != 0 ) {
    if ( m_server->uidata.panel == this ) m_server->uidata.panel = 0;
  }
  if ( m_user != 0 ) {
    if ( m_user->uidata.panel == this ) m_user->uidata.panel = 0;
  }
  if ( m_channel != 0 ) {
    if ( m_channel->uidata.panel == this ) m_channel->uidata.panel = 0;
  }
}


User& ChatPanel::GetMe()
{
  if ( m_type == CPT_Channel ) return m_channel->GetMe();
  else if ( m_type == CPT_Battle ) return m_battle->GetMe();
  else if ( m_type == CPT_Server ) return m_server->GetMe();
  else if ( m_type == CPT_User ) return *m_user;

  ASSERT_LOGIC( false, "GetMe() m_type invalid." );
}


void ChatPanel::_OutputLine( const wxString& message, const wxColour& col )
{
  LogTime();
  m_chatlog_text->SetDefaultStyle(wxTextAttr(col));
  m_chatlog_text->Freeze(); 
  m_chatlog_text->AppendText( message + _T("\n") );
#ifdef __WXMSW__
  m_chatlog_text->ScrollLines( 10 );
  m_chatlog_text->ShowPosition( m_chatlog_text->GetLastPosition() );
#endif
  m_chatlog_text->Thaw();
}


void ChatPanel::OnResize( wxSizeEvent& event )
{
  SetSize( event.GetSize() );
  Layout();
  if ( m_show_nick_list ) {
    wxSize s = m_splitter->GetSize();
    m_splitter->SetSashPosition( s.GetWidth() - 238, true );
  }
}


void ChatPanel::OnSay( wxCommandEvent& event )
{
  Say( m_say_text->GetValue() );
  m_say_text->SetValue( _T("") );
}

//! @brief Output a message said in the channel.
//!
//! @param who nick of the person who said something.
//! @param message the message to be outputted.
void ChatPanel::Said( const wxString& who, const wxString& message )
{
  wxString me = WX_STRING(GetMe().GetNick());
  wxColour col;
  if ( who.Upper() == me.Upper() ) col.Set( 100,100,140 );
  else if ( message.Upper().Contains( me.Upper() ) ) col.Set( 255,40,40 );
  else col.Set( 0,0,0 );

  _OutputLine( _T(" <") + who + _T("> ")+ message, col );
}


void ChatPanel::DidAction( const wxString& who, const wxString& action )
{
  _OutputLine( _T(" * ") + who + _T(" ") + action, wxColour( 230, 0, 255 ) );
}


//! @brief Output motd sent by server
//!
//! @param message The MOTD message to output
void ChatPanel::Motd( const wxString& message )
{
  _OutputLine( _T(" ** motd ** ") + message, wxColour(0, 80, 128) );
}


void ChatPanel::StatusMessage( const wxString& message )
{
  _OutputLine( _T(" ** Server ** ")+ message, wxColour(255, 150, 80) );
}


void ChatPanel::UnknownCommand( const wxString& command, const wxString& params )
{
  _OutputLine( _(" !! Command: \"") + command + _("\" params: \"") + params + _T("\"."), wxColour(128, 0, 0) );
}


wxString ChatPanel::GetChatTypeStr()
{
  if ( m_type == CPT_Channel ) return _("channel");
  else if ( m_type == CPT_Battle ) return _("battle");
  else if ( m_type == CPT_Server ) return _("server");

  return _T("ROOMTYPE FIXME");
}


void ChatPanel::Joined( User& who )
{
  _OutputLine( _T(" ** ") + WX_STRING(who.GetNick()) + _(" joined the ") + GetChatTypeStr() + _T("."), wxColour(0, 80, 0) );
  if ( m_show_nick_list ) m_nicklist->AddUser( who );
}


void ChatPanel::Parted( User& who, const wxString& message )
{
  _OutputLine( _T(" ** ")+ WX_STRING(who.GetNick()) + _(" left the channel ( ") + message + _T(" )."), wxColour(0, 80, 0) );
  if ( m_show_nick_list ) m_nicklist->RemoveUser( who );
}


void ChatPanel::SetTopic( const wxString& who, const wxString& message )
{
  _OutputLine( _T(" ** Channel topic: ")+ message + _("\n * Set by ") + who, wxColour(0, 0, 80) );
}


void ChatPanel::UserStatusUpdated( User& who )
{
  if ( m_show_nick_list ) {
    assert( m_nicklist != 0 );
    m_nicklist->UserUpdated( who );
  }
}


Channel& ChatPanel::GetChannel()
{
  return *m_channel;
}


Server* ChatPanel::GetServer()
{
  return m_server;
}

void ChatPanel::SetServer( Server* serv )
{
  ASSERT_LOGIC(m_type == CPT_Server, "Not of type server" );
  m_server = serv;
}


bool ChatPanel::IsServerPanel()
{
  return (m_type == CPT_Server);
}


//! @brief Set the Channel object
//!
//! @param channel the Channel object.
void ChatPanel::_SetChannel( Channel* channel )
{
  if ( m_channel != 0 ) {
    m_channel->uidata.panel = 0;
  }

  m_channel = channel;

  if ( m_channel != 0 ) {
    m_channel->uidata.panel = this;
  }

}


void ChatPanel::Say( const wxString& message )
{
  debug_func( "" );
  if ( message.Find('/') == 0 ) {
    if ( m_ui.ExecuteSayCommand( message ) ) return;
  }

  if ( message == _T("/ver") ) {
    // TODO fix real version numbers
    _OutputLine( wxString::Format( _(" You have SpringLobby version %.2f r%d"), 0.01, 0 ), *wxBLACK );
    return;
  }

  if ( m_type == CPT_Channel ) {

    assert( m_channel != 0 );
    if ( message.StartsWith( _T("/") ) ) {
      if ( m_channel->ExecuteSayCommand( STD_STRING(message) ) ) return;
    }
    m_channel->Say( STD_STRING(message) );

  } else if ( m_type == CPT_Battle ) {

    assert( m_battle != 0 );
    if ( message.StartsWith(_T("/")) ) {
      if ( m_battle->ExecuteSayCommand( STD_STRING(message) ) ) return;
    }
    m_battle->Say( STD_STRING(message) );

  } else if ( m_type == CPT_User ) {

    assert( m_user != 0 );
    if ( message.StartsWith(_T("/")) ) {
      if ( m_user->ExecuteSayCommand( STD_STRING(message) ) ) return;
    }
    m_user->Say( STD_STRING(message) );

  }
}


void ChatPanel::Part()
{
  debug_func( "" );
  if ( m_type == CPT_Channel ) {
    assert( m_channel != 0 );
    m_channel->Leave();
    m_channel->uidata.panel = 0;
  }
}


void ChatPanel::LogTime()
{
  wxDateTime now = wxDateTime::Now();
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxBLACK));
  m_chatlog_text->AppendText( _T("[") + now.Format( _T("%H:%M") ) + _T("]") );
}


