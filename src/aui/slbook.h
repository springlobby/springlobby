#ifndef SPRINGLOBBY_HEADERGUARD_SLBOOK_H
#define SPRINGLOBBY_HEADERGUARD_SLBOOK_H

#include <wx/aui/aui.h>
#include "../utils/mixins.hh"

class ChatPanelMenu;

/** \brief wxAuiNotebook derived class that allows child page layouts to be saved/loaded at runtime
    Originally this was just meant as means of setting the aui manager flags to circumvent
    the crash with compositing enabled: http://trac.wxwidgets.org/ticket/4841 \n
    The functionality for perspective loading/saving originates from the patch at:
    http://trac.wxwidgets.org/ticket/10466

**/
class SLNotebook : public wxAuiNotebook {

    public:
        SLNotebook (wxWindow* parent, const wxString& name, wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                        long style = wxAUI_NB_DEFAULT_STYLE);
//        wxAuiManager& GetAuiManager() { return m_mgr; }

        wxString SavePerspective();
        bool LoadPerspective(const wxString& layout);

		void AdvanceSelection( bool forward );

        /** \brief call fitinside for each child page
            useful after loading perspectives, since that does not generate OnSize events
            \note koshi: turns out it isn't strictly necessary on at least wxGTK
        **/
        void FitChildPages();
};

class ChatPanel;

/** \brief SLNotebook derived class that only accepts ChatPanel pages
    Provides a common context menu for all tab headers that has a the repective Chatpanel's context menu as a submenu
**/
class SLChatNotebook : public SLNotebook, public SL::NonCopyable {

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
        bool AddPage(wxWindow* , const wxString& , bool , const wxBitmap& );
		bool DeleteChatPage( size_t i );

        ChatPanelMenu* m_ch_menu;
        ChatPanel* m_cur_page;

        DECLARE_EVENT_TABLE();
};

//! Utility function that handles surrounding Layout updates in addition to loading a notebook perspective
void LoadNotebookPerspective( SLNotebook* notebook, const wxString& perspective_name );
void SaveNotebookPerspective( SLNotebook* notebook, const wxString& perspective_name );

#endif // SPRINGLOBBY_HEADERGUARD_SLBOOK_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

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
