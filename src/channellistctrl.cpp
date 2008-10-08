#include "channellistctrl.h"

ChannelListctrl::ChannelListctrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style, wxString name)
    :CustomListCtrl(parent, CHANNELLIST, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("ChannelListCtrl"))
{
    //ctor
}

ChannelListctrl::~ChannelListctrl()
{
    //dtor
}
