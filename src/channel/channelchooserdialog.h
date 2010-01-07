#ifndef SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED
#define SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED

#include <wx/dialog.h>
#include "../gui/windowattributespickle.h"

class ChannelChooserPanel;
class wxBoxSizer;

class ChannelChooserDialog : public wxDialog, public WindowAttributesPickle
{
    public:
        ChannelChooserDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~ChannelChooserDialog();

        void AddChannel( const wxString& name, int usercount, const wxString& topic = wxEmptyString );
        void ClearChannels();

    protected:
        ChannelChooserPanel* m_chooser_panel;
        wxBoxSizer* m_main_sizer;

};

#endif // SPRINGLOBBY_CHANNELCHOOSERDIALOG_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

