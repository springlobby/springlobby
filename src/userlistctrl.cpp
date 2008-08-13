#include "userlistctrl.h"
#include "ui.h"
#include "tasserver.h"
#include "user.h"

UserListctrl::UserListctrl(wxWindow* parent, bool show_header, UserMenu* popup, bool singleSelectList,
                            const wxString& name, bool highlight)
  : NickListCtrl( parent, show_header , popup, singleSelectList, name, highlight ),
  m_dummy_server((ui()))
{
    //ctor
}

UserListctrl::~UserListctrl()
{
    //dtor
}

void UserListctrl::AddUser( const User& user )
{
    m_usernames.Add( user.GetNick() );
    NickListCtrl::AddUser( user );
}


void UserListctrl::AddUserByName( const wxString& name )
{
    UserStatus st;
    st.offline = true;
    User user (m_dummy_server);
    user.SetNick( name );
    user.SetStatus( st );
    AddUser( user );
}

void UserListctrl::Sort(){}

void UserListctrl::SetTipWindowText( const long item_hit, const wxPoint position){}

wxArrayString UserListctrl::GetUserNicks( )
{
    return m_usernames;
}
