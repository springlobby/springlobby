#ifndef SPRINGLOBBY_HEADERGUARD_SLBOOK_H
#define SPRINGLOBBY_HEADERGUARD_SLBOOK_H

#include <wx/aui/aui.h>

class ChatPanelMenu;

class SLNotebook : public wxAuiNotebook {

    public:
        SLNotebook (wxWindow* parent, const wxString& name, wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                        long style = wxAUI_NB_DEFAULT_STYLE);
//        wxAuiManager& GetAuiManager() { return m_mgr; }

        wxString SavePerspective();
        bool LoadPerspective(const wxString& layout);
        wxString GetName() { return m_name; }

    protected:
        wxString m_name;
        bool m_autosave_prespective;

};

class ChatPanel;

class SLChatNotebook : public SLNotebook {

    public:
        SLChatNotebook (wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE);
        bool AddPage(ChatPanel* page, const wxString& caption, bool select = false, const wxBitmap& bitmap = wxNullBitmap);

        void OnHeaderRightClick(wxAuiNotebookEvent &event);
        void OnMenuItem( wxCommandEvent& event );
        void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);

    protected:
        typedef SLNotebook
            ParentType;

        //prohibit adding other panels
        bool AddPage(wxWindow* page, const wxString& caption, bool select = false, const wxBitmap& bitmap = wxNullBitmap) {return false;}

        ChatPanelMenu* m_ch_menu;

        DECLARE_EVENT_TABLE();
};

void LoadNotebookPerspective( SLNotebook* notebook, const wxString& perspective_name );
void SaveNotebookPerspective( SLNotebook* notebook, const wxString& perspective_name );

#endif // SPRINGLOBBY_HEADERGUARD_SLBOOK_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

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
