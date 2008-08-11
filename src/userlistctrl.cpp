#include "userlistctrl.h"

UserListctrl::UserListctrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style, wxString name)
    :customListCtrl(parent, USERLIST, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("UserListCtrl"))
{
    //ctor
}

UserListctrl::~UserListctrl()
{
    //dtor
}
