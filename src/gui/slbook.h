/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SLBOOK_H
#define SPRINGLOBBY_HEADERGUARD_SLBOOK_H

#include <wx/aui/aui.h>
#include "utils/mixins.h"
class ChatPanelMenu;

/** \brief wxAuiNotebook derived class that allows child page layouts to be saved/loaded at runtime
    Originally this was just meant as means of setting the aui manager flags to circumvent
    the crash with compositing enabled: http://trac.wxwidgets.org/ticket/4841 \n
    The functionality for perspective loading/saving originates from the patch at:
    http://trac.wxwidgets.org/ticket/10466

**/
class SLNotebook : public wxAuiNotebook
{

public:
	SLNotebook(wxWindow* parent, const wxString& name, wxWindowID id = wxID_ANY,
		   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		   long style = wxAUI_NB_DEFAULT_STYLE);
	//        wxAuiManager& GetAuiManager() { return m_mgr; }

	wxString SavePerspective();
	bool LoadPerspective(const wxString& layout);
	void AdvanceSelection(bool forward);

private:
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
class SLChatNotebook : public SLNotebook, public SL::NonCopyable
{

public:
	SLChatNotebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE);

private:
	void OnHeaderRightClick(wxAuiNotebookEvent& event);
	void OnMenuItem(wxCommandEvent& event);
	void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);

	bool DeleteChatPage(size_t i);

	ChatPanelMenu* m_ch_menu;
	ChatPanel* m_cur_page;

	DECLARE_EVENT_TABLE()
};

//! Utility function that handles surrounding Layout updates in addition to loading a notebook perspective
void LoadNotebookPerspective(SLNotebook* notebook, const wxString& perspective_name);
void SaveNotebookPerspective(SLNotebook* notebook, const wxString& perspective_name);

#endif // SPRINGLOBBY_HEADERGUARD_SLBOOK_H
