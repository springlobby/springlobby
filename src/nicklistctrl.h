#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

#include <wx/listctrl.h>
#include <wx/event.h>

class User;
class UserList;
class Ui;
class wxMenu;

class NickListCtrl : public wxListCtrl
{
  public:
    NickListCtrl( wxWindow* parent, Ui& ui, bool show_header = true, wxMenu* popup = 0 );
    ~NickListCtrl();

    void AddUser( User& user );
    void RemoveUser( const User& user );

    void UserUpdated( User& user );
    void UserUpdated( const int& index );

    int GetUserIndex( User& user );

    void ClearUsers();

    void OnActivateItem( wxListEvent& event );
    void OnShowMenu( wxContextMenuEvent& event );
  protected:

    UserList* m_users;
    Ui& m_ui;

    wxMenu* m_menu;

    DECLARE_EVENT_TABLE()
};

enum
{
  NICK_LIST = wxID_HIGHEST
};

#endif // SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
