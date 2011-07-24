#ifndef CHANNELCHOOSERPANEL_H
#define CHANNELCHOOSERPANEL_H

#include <wx/scrolwin.h>
#include "../utils/mixins.hh"

class ChannelListctrl;
class wxBoxSizer;
class wxButton;
class wxTextCtrl;
class wxStaticText;

class ChannelChooserPanel : public wxScrolledWindow, public SL::NonCopyable
{
    public:
        ChannelChooserPanel(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL ,
            const wxString& name = _T("dialogBox") );
        virtual ~ChannelChooserPanel();

        void AddChannel( const wxString& name, int usercount, const wxString& topic = wxEmptyString );
        void ClearChannels();
        void OnSearch( wxCommandEvent& event );

    protected:
        wxButton* m_join_channels;
        wxButton* m_mark_autojoin;
        wxBoxSizer* m_main_sizer;
        ChannelListctrl* m_channellist;
        wxTextCtrl* m_search_text;
        wxStaticText* m_info_label;

        enum {
            ID_SEARCH_TEXT
        };

        DECLARE_EVENT_TABLE()
};

#endif // CHANNELCHOOSER_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

