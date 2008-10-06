#ifndef SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H

#include "customlistctrl.h"


class ChannelListctrl : public CustomListCtrl
{
    public:
        ChannelListctrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style, wxString name);
        virtual ~ChannelListctrl();
    protected:


        enum {
          CHANNELLIST = wxID_HIGHEST

        };
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
