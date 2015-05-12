/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H

#include <wx/scrolwin.h>

class Ui;
class ChatPanel;
class IServer;
class Channel;
class User;
class wxAuiNotebookEvent;
class SLChatNotebook;
class wxBoxSizer;
class wxImageList;
class wxString;

//! @brief The main chat tab.
class MainChatTab : public wxPanel
{
public:
	MainChatTab(wxWindow* parent);
	~MainChatTab();

	ChatPanel* GetActiveChatPanel();
	ChatPanel* GetChannelChatPanel(const wxString& channel);
	ChatPanel* GetUserChatPanel(const wxString& user);

	ChatPanel& ServerChat();

	ChatPanel* AddChatPanel(Channel& channel, bool doFocus);
	ChatPanel* AddChatPanel(IServer& server, const wxString& name);
	ChatPanel* AddChatPanel(const User& user);
	ChatPanel* AddChatPanel();
	/** \brief this is only used if channel is left via raw command in server tab */
	bool RemoveChatPanel(ChatPanel* panel);


	void OnLoggedIn();
	void OnDisconnected();

	void BroadcastMessage(const wxString& message);

	void OnUserConnected(User& user);
	void OnUserDisconnected(User& user);


	void UpdateNicklistHighlights();

	void AdvanceSelection(bool forward);

private:
	ChatPanel* GetCurrentPanel();

	void OnTabsChanged(wxAuiNotebookEvent& event);
	void OnTabClose(wxAuiNotebookEvent& event);

	void LoadPerspective(const wxString& perspective_name = wxEmptyString);
	void SavePerspective(const wxString& perspective_name = wxEmptyString);

	wxImage ReplaceChannelStatusColour(wxBitmap img, const wxColour& colour) const;


	//	wxWindow* m_close_window;
	SLChatNotebook* m_chat_tabs;
	wxBoxSizer* m_main_sizer;
	wxImageList* m_imagelist;
	ChatPanel* m_server_chat;
	//	ChatPanel* m_main_chat;
	int m_newtab_sel;

	enum {
		CHAT_TABS = wxID_HIGHEST
	};

	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_MAINCHATTAB_H
