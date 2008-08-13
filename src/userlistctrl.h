#ifndef SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H

#include "nicklistctrl.h"
#include "tasserver.h"


//! a rather sneaky approach to resue NickList for both off/online users
/**
    Don't ever use this for anything that needs constant updating, context menus sorting and the like.\n
    Basically don't use it outside the groupuserdialog without extreme caution.
**/
class UserListctrl : public NickListCtrl
{
    public:
        UserListctrl( wxWindow* parent, bool show_header = true, UserMenu* popup = 0,
            bool singleSelectList = true, const wxString& name = _T("NickListCtrl"), bool highlight = true  );
        virtual ~UserListctrl();

        virtual void AddUser( const User& user );
        void AddUserByName( const wxString& name );
        wxArrayString GetUserNicks( );
    protected:
        wxSortedArrayString m_usernames;
        //! no-op atm, so i don't get segfault because of missing data
        virtual void Sort();
        virtual void SetTipWindowText( const long item_hit, const wxPoint position);
        //! i fake a server so can use the User class interface of nicklist to add fake User
        TASServer m_dummy_server;
        enum {
            USERLIST = wxID_HIGHEST
        };

};

#endif // SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H
