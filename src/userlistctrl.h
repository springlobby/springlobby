#ifndef SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H

#include "customlistctrl.h"


class UserListctrl : public customListCtrl
{
    public:
        UserListctrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style, wxString name);
        virtual ~UserListctrl();
    protected:


        enum {
            USERLIST = wxID_HIGHEST
        };

};

#endif // SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H
