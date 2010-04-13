#ifndef SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H

#include "../customvirtlistctrl.h"

struct ChannelInfo
{
    ChannelInfo()
        : name(_T("")), usercount(0),topic(_T("")) {}
    ChannelInfo( const wxString& name_, int usercount_, const wxString& topic_ = wxEmptyString )
        : name(name_), usercount(usercount_),topic(topic_) {}

    wxString name;
    int usercount;
    wxString topic;
};

class ChannelListctrl : public CustomVirtListCtrl< ChannelInfo, ChannelListctrl >
{
    public:
        ChannelListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("ChannelListCtrl"),
            long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize);
        virtual ~ChannelListctrl();

        void AddChannel( const wxString& channel, unsigned int num_users, const wxString& topic);
        void ClearChannels();
        wxString GetInfo();
        void FilterChannel( const wxString& partial );

        //these are overloaded to use list in virtual style
        wxString GetItemText(long item, long column) const;
        int GetItemImage(long item) const;
        int GetItemColumnImage(long item, long column) const;
        wxListItemAttr* GetItemAttr(long /*unused*/) const {return 0;}

    protected:
        void Sort();
        void SetTipWindowText( const long item_hit, const wxPoint& position);

        void OnActivateItem( wxListEvent& event );

        void HighlightItem( long item );

        enum {
          CHANNELLIST = wxID_HIGHEST

        };

        int GetIndexFromData( const DataType& data ) const;

        //! passed as callback to generic ItemComparator, returns -1,0,1 as per defined ordering
		int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;

        wxString m_last_filter_value;

        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNELLISTCTRL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

