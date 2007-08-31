#ifndef SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H

#include <wx/listctrl.h>
#include <wx/event.h>

class User;
class UserList;
class Ui;

class NickListCtrl : public wxListCtrl
{
  public:
    NickListCtrl( wxWindow* parent, Ui& ui, bool show_header = true );
    ~NickListCtrl();

    void AddUser( User& user );
    void RemoveUser( const User& user );

    void UserUpdated( User& user );
    void UserUpdated( const int& index );

    int GetUserIndex( User& user );

    void ClearUsers();

    void OnActivateItem( wxListEvent& event );

  protected:

    UserList* m_users;
    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

enum
{
  NICK_LIST = wxID_HIGHEST
};

#endif // SPRINGLOBBY_HEADERGUARD_NICKLISTCTRL_H
