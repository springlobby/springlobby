/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANEL_H

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <vector>
#include <set>

#include "chatlog.h"
#include "utils/TextCompletionDatabase.h"
#include "utils/mixins.h"
#include "utils/globalevents.h"

class wxCommandEvent;
class wxSizeEvent;
class wxBoxSizer;
class wxSplitterWindow;
class wxTextCtrl;
class wxTextCtrlHist;
class wxTextUrlEvent;
class wxComboBox;
class wxButton;
class wxBitmapButton;
class NickListCtrl;
class Channel;
class User;
class ChatLog;
class IServer;
class IBattle;
class Ui;
class wxStaticText;

class wxMouseEvent;
class SLNotebook;
class wxNotebook;
class wxImageList;
class ChatPanelMenu;
class VotePanel;

enum ChatPanelType {
	CPT_Channel,
	CPT_Server,
	CPT_User,
	CPT_Battle,
	CPT_Debug
};

struct ChatLine
{
	wxString chat;
	wxString time;
	wxTextAttr timestyle;
	wxTextAttr chatstyle;
};

/*! @brief wxPanel that contains a chat.
 *
 * This panel contains a chat with a multiline TextCtrl for the messages, a
 * signle line TextCtrl for messages to send, a send button and a nick list.
 *
 * The nick list is optional and can be removed by setting show_nick_list in the
 * constructor to false.
 */
class ChatPanel : public wxPanel, public GlobalEvent, public SL::NonCopyable
{
public:
	ChatPanel(wxWindow* parent, Channel& chan, wxImageList* imaglist);
	ChatPanel(wxWindow* parent, const User& user, wxImageList* imaglist);
	ChatPanel(wxWindow* parent, IServer& serv, wxImageList* imaglist);
	ChatPanel(wxWindow* parent, IBattle* battle);
	ChatPanel(wxWindow* parent); //debug chatpanel
	~ChatPanel();

	void Said(const wxString& who, const wxString& message);
	void DidAction(const wxString& who, const wxString& action);
	void Motd(const wxString& message);
	void StatusMessage(const wxString& message);
	void ClientMessage(const wxString& message);

	void UnknownCommand(const wxString& command, const wxString& params);

	void Joined(User& who);
	void Parted(User& who, const wxString& message);
	void SetTopic(const wxString& who, const wxString& message);
	void UserStatusUpdated(User& who);
	void OnChannelJoin(User& who);

	const Channel* GetChannel() const;
	void SetChannel(Channel* chan);

	const IServer* GetServer() const;
	void SetServer(IServer* serv);

	const User* GetUser() const;
	void SetUser(const User* usr);

	ChatPanelType GetPanelType() const;

	void SetBattle(IBattle* battle);

	void Part();
	void FocusInputBox();


	size_t GetIconIndex() const
	{
		return m_icon_index;
	}
	void SetIconIndex(size_t index)
	{
		m_icon_index = index;
	}

	void OnUserDisconnected();
	void OnUserConnected();

	void UpdateNicklistHighlights();

	void SetVotePanel(VotePanel* votePanel);

	bool Say(const wxString& message);

private:
	void Init(const wxString& panelname);
	//! @returns true on success ( blank line ), false otherwise

	wxString GetChatTypeStr() const;

	const User& GetMe() const;
	const User* GetSelectedUser() const;

	void SortNickList();
	void OnFilterUsers(wxCommandEvent&);

	void OnChanOpts(wxCommandEvent& event);
	void OnSay(wxCommandEvent& event);
	void OnPaste(wxClipboardTextEvent& event);

	void OnLinkEvent(wxTextUrlEvent& event);
	void OnMouseDown(wxMouseEvent& event);

	void OnKeyPressed(wxKeyEvent& keyevent);
	void OnKeyReleased(wxKeyEvent& keyevent);

	bool IsOk() const;
	void OnMenuItem(wxCommandEvent& event);

	void OnLogin(wxCommandEvent& data);

	void OutputLine(const wxString& message, const wxColour& col, bool showtime = true);

	void OutputLine(const ChatLine& line);
	void SetLogFile(const wxString& name);

	enum HighlightType {
		highlight_say,
		highlight_join_leave,
		highlight_important
	};

	void SetIconHighlight(HighlightType highlight);
	wxString FindUrl(const long pos) const;

	bool ContainsWordToHighlight(const wxString& message) const;
	bool m_show_nick_list; //!< If the nicklist should be shown or not.

	VotePanel* m_votePanel;

	wxBoxSizer* m_main_sizer; //!< Main sizer containing all other sizers.
	wxBoxSizer* m_chat_sizer; //!< Sizer containing the chat messages, and send input and button.
	wxBoxSizer* m_say_sizer;  //!< Sizer containing send input and button.
	wxBoxSizer* m_nick_sizer; //!< Sizer containing the nicklist.

	wxSplitterWindow* m_splitter; //!< The splitter.
	wxPanel* m_chat_panel;	//!< Panel containing the chat. Only used when nicklist is visible.
	wxPanel* m_nick_panel;	//!< Panel containing the nicklist.
	wxTextCtrl* m_nick_filter;    //!< Textcontrol for filtering nicklist

	wxTextCtrl* m_chatlog_text;	 //!< The chat log textcontrol.
	wxTextCtrlHist* m_say_text;	 //!< The say textcontrol.
	wxBitmapButton* m_chan_opts_button; //!< The channel options button.

	NickListCtrl* m_nicklist; //!< The nicklist.
	//wxComboBox* m_nick_filter;  //!< The filter combo.

	wxButton* m_say_button; //!< The say button.
	SLNotebook* m_chat_tabs;

	Channel* m_channel; //!< Channel object.
	IServer* m_server;  //!< Server object.
	const User* m_user; //!< User object.
	IBattle* m_battle;  //!< User object.

	wxStaticText* m_usercount_label;

	const ChatPanelType m_type; //!< Channel object.

	wxString m_chan_pass;

	ChatPanelMenu* m_popup_menu;

	ChatLog m_chat_log;

	void LogTime();
	void CreateControls();
	void CreatePopup();
	size_t m_icon_index;

	wxImageList* m_imagelist;

	static const int m_groupMenu_baseID = 6798;
	TextCompletionDatabase textcompletiondatabase;

	std::vector<ChatLine> m_buffer;
	std::set<wxString> m_active_users; //users who spoke
	bool m_disable_append;		   //disable text appending
	bool m_display_joinitem;	   //show users joing/leaving

	wxString m_chatpanelname;
	wxString m_url_at_pos; //! the mouse event sink sets this

	friend class ChatPanelMenu; //menu needs access to members

	//used to avoid marking channel as changed when it's just been created.
	bool m_topic_set;

	DECLARE_EVENT_TABLE()
};

enum {
	CHAT_SEND = wxID_HIGHEST,
	CHAT_TEXT,
	CHAT_LOG,
	CHAT_CHAN_OPTS,
	FILTER_USERS
};
#endif // SPRINGLOBBY_HEADERGUARD_CHATPANEL_H
