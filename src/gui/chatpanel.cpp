/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: ChatPanel
//

#include "utils/curlhelper.h" //has to be first include, as else it warns about winsock2.h should be included first
#include "utils/conversion.h"
#include "gui/customdialogs.h"
#include "uiutils.h"

#include "chatpanel.h"

#include <wx/intl.h>
#include <wx/splitter.h>
#include <wx/tokenzr.h>
#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>
#include <wx/wupdlock.h>

#include "aui/auimanager.h"
#include "gui/slbook.h"
#include "channel.h"
#include "utils/uievents.h"
#include "utils/version.h"
#include "iserver.h"
#include "serverselector.h"
#include "ibattle.h"
#include "nickdataviewctrl.h"
#include "mainwindow.h"
#include "chatpanelmenu.h"
#include "gui/pastedialog.h"
#include "gui/wxtextctrlhist.h"
#include "log.h"
#include "utils/slconfig.h"
#include "gui/hosting/votepanel.h"
#include "utils/globalevents.h"
#include "gui/iconscollection.h"
#include "gui/controls.h"
#include "gui/mainwindow.h"

BEGIN_EVENT_TABLE(ChatPanel, wxPanel)
EVT_TEXT_ENTER(CHAT_TEXT, ChatPanel::OnSay)
EVT_TEXT_PASTE(CHAT_TEXT, ChatPanel::OnPaste)
EVT_BUTTON(CHAT_CHAN_OPTS, ChatPanel::OnChanOpts)
EVT_BUTTON(CHAT_SEND, ChatPanel::OnSay)
EVT_TEXT_URL(CHAT_LOG, ChatPanel::OnLinkEvent)
EVT_MENU(wxID_ANY, ChatPanel::OnMenuItem)
EVT_TEXT(FILTER_USERS, ChatPanel::OnFilterUsers)
EVT_CHECKBOX(SHOW_PLAYERS_ONLY_CHECK, ChatPanel::OnShowPlayerOnlyCheck)
END_EVENT_TABLE()


SLCONFIG("/Channel/bridgebot", (const wxString&) _T("TIZBOT"), "Name of the Bridgebot (which forwards traffic between #sy and irc channels)");
SLCONFIG("/GUI/ShowPromotions", true, "Show promotion messages as popup");

/// table for irc colors
static wxColor m_irc_colors[16] = {
    wxColor(204, 204, 204),
    wxColor(0, 0, 0),
    wxColor(54, 54, 178),
    wxColor(42, 140, 42),
    wxColor(195, 59, 59),
    wxColor(199, 50, 50),
    wxColor(128, 38, 127),
    wxColor(102, 54, 31),
    wxColor(217, 166, 65),
    wxColor(61, 204, 61),
    wxColor(26, 85, 85),
    wxColor(47, 140, 116),
    wxColor(69, 69, 230),
    wxColor(176, 55, 176),
    wxColor(76, 76, 76),
    wxColor(149, 149, 149)};

void ChatPanel::Init(const wxString& panelname)
{
	m_chatpanelname = panelname;

	//Clear some controls pointer that can be used in ReadSettings
	m_say_text = nullptr;
	m_chatlog_text = nullptr;

	//Read settings
	ReadSettings();
	CreateControls();
	SetLogFile(panelname);

	GetAui().manager->AddPane(this, wxLEFT, _T("chatpanel-channel-") + panelname);
	m_chatlog_text->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(ChatPanel::OnMouseDown), 0, this);

	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnLogin, ChatPanel::OnLogin);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::ApplicationSettingsChangedEvent, ChatPanel::OnSettingsChanged);
}

ChatPanel::ChatPanel(wxWindow* parent, Channel& chan, wxImageList* imaglist)
    : wxPanel(parent, -1)
    , m_show_nick_list(true)
    , m_votePanel(0)
    , m_nicklist(0)
    , m_chat_tabs((SLNotebook*)parent)
    , m_channel(&chan)
    , m_server(0)
    , m_user(0)
    , m_battle(0)
    , m_type(CPT_Channel)
    , m_popup_menu(NULL)
    , m_icon_index(2)
    , m_imagelist(imaglist)
    , m_disable_append(false)
    , m_display_joinitem(false)
    , m_topic_set(false)
    , m_reactOnPromoteEvents(true)
{
	Init(TowxString(chan.GetName()));
	SetChannel(&chan);
}


ChatPanel::ChatPanel(wxWindow* parent, User& user, wxImageList* imaglist)
    : wxPanel(parent, -1)
    , m_show_nick_list(false)
    , m_votePanel(0)
    , m_nicklist(0)
    , m_chat_tabs((SLNotebook*)parent)
    , m_channel(0)
    , m_server(0)
    , m_user(&user)
    , m_battle(0)
    , m_type(CPT_User)
    , m_popup_menu(NULL)
    , m_icon_index(3)
    , m_imagelist(imaglist)
    , m_disable_append(false)
    , m_display_joinitem(true)
    , m_topic_set(false)
    , m_reactOnPromoteEvents(false)
{
	Init(_T("chatpanel-pm-") + TowxString(user.GetNick()));
	SetUser(&user);
}


ChatPanel::ChatPanel(wxWindow* parent, IServer& serv, wxImageList* imaglist)
    : wxPanel(parent, -1)
    , m_show_nick_list(true)
    , m_votePanel(0)
    , m_nicklist(0)
    , m_chat_tabs((SLNotebook*)parent)
    , m_channel(0)
    , m_server(&serv)
    , m_user(0)
    , m_battle(0)
    , m_type(CPT_Server)
    , m_popup_menu(NULL)
    , m_icon_index(1)
    , m_imagelist(imaglist)
    , m_disable_append(false)
    , m_display_joinitem(false)
    , m_topic_set(false)
    , m_reactOnPromoteEvents(false)
{
	Init(_T("chatpanel-server"));
	SetServer(&serv);
}


ChatPanel::ChatPanel(wxWindow* parent, IBattle* battle)
    : wxPanel(parent, -1)
    , m_show_nick_list(false)
    , m_votePanel(0)
    , m_nicklist(0)
    , m_chat_tabs(0)
    , m_channel(0)
    , m_server(0)
    , m_user(0)
    , m_battle(battle)
    , m_type(CPT_Battle)
    , m_popup_menu(NULL)
    , m_disable_append(false)
    , m_display_joinitem(true)
    , m_topic_set(false)
    , m_reactOnPromoteEvents(false)
{
	Init(_T("BATTLE"));
	SetBattle(battle);
}

ChatPanel::ChatPanel(wxWindow* parent)
    : wxPanel(parent, -1)
    , m_show_nick_list(false)
    , m_votePanel(0)
    , m_nicklist(0)
    , m_chat_tabs(0)
    , m_channel(0)
    , m_server(0)
    , m_user(0)
    , m_battle(0)
    , m_type(CPT_Debug)
    , m_popup_menu(NULL)
    , m_disable_append(false)
    , m_display_joinitem(true)
    , m_topic_set(false)
    , m_reactOnPromoteEvents(false)
{
	Init(_T("debug"));
}

//! @brief ChatPanel destructor.
ChatPanel::~ChatPanel()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);

	if (m_server != 0) {
		if (m_server->panel == this)
			m_server->panel = 0;
	}
	if (m_user != 0) {
		if (m_user->panel == this)
			m_user->panel = nullptr;
	}
	if (m_channel != 0) {
		if (m_channel->panel == this)
			m_channel->panel = nullptr;
	}
	cfg().Write(_T( "/Channels/DisplayJoinLeave/" ) + m_chatpanelname, m_display_joinitem);

	m_chatlog_text->Disconnect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(ChatPanel::OnMouseDown), 0, 0);
	wxDELETE(m_chatlog_text);
	wxDELETE(m_popup_menu);

	if (GetAui().manager) {
		GetAui().manager->DetachPane(this);
	}
}


void ChatPanel::CreateControls()
{
	// Creating sizers
	m_main_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_chat_sizer = new wxBoxSizer(wxVERTICAL);
	m_say_sizer = new wxBoxSizer(wxHORIZONTAL);

	if (m_show_nick_list) {

		m_splitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D);
		m_splitter->SetSashGravity(0.9);
		m_nick_panel = new wxPanel(m_splitter, -1);
		m_chat_panel = new wxPanel(m_splitter, -1);

		m_nick_sizer = new wxBoxSizer(wxVERTICAL);
		m_usercount_label = new wxStaticText(m_nick_panel, wxID_ANY, wxEmptyString);
		m_showPlayerOnlyCheck = new wxCheckBox(m_nick_panel, SHOW_PLAYERS_ONLY_CHECK, _("Hide bots"));

		wxBoxSizer* nickListHeaderSizer = new wxBoxSizer(wxVERTICAL);
		nickListHeaderSizer->Add(m_usercount_label, 0, wxEXPAND);
		nickListHeaderSizer->Add(m_showPlayerOnlyCheck, 0, wxEXPAND);

		UpdateUserCountLabel();
		CreatePopup(); //ensures m_popup_menu is constructed
		//SL_GENERIC::UserMenu<ChatPanelMenu>* usermenu  = m_popup_menu->GetUserMenu();
		m_nicklist = new NickDataViewCtrl(_T("NickDataViewCtrl_ChatPanel"), m_nick_panel, true, m_popup_menu);

		// m_nick_filter = new wxComboBox( m_nick_panel, -1, _("Show all"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT), 0, 0, wxCB_READONLY );
		// m_nick_filter->Disable();

		m_nick_filter = new wxTextCtrl(m_nick_panel, FILTER_USERS);
		m_nick_sizer->Add(m_nicklist, 1, wxEXPAND);
		m_nick_sizer->Add(nickListHeaderSizer, 0, wxEXPAND);
		m_nick_sizer->Add(m_nick_filter, 0, wxEXPAND);
		// m_nick_sizer->Add( m_nick_filter, 0, wxEXPAND | wxTOP, 2 );

		m_nick_panel->SetSizer(m_nick_sizer);
		m_nicklist->UserFilterShowPlayersOnly(m_ShowPlayersOnlyFlag);
		m_showPlayerOnlyCheck->SetValue(m_ShowPlayersOnlyFlag);
	} else {

		m_chat_panel = this;
		m_nick_sizer = 0;
		m_nicklist = 0;
		m_splitter = 0;
		m_nick_filter = 0;
	}

	// Creating ui elements

	m_chatlog_text = new wxTextCtrl(m_chat_panel, CHAT_LOG, wxEmptyString, wxDefaultPosition, wxDefaultSize,
					wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL);
	m_chan_opts_button = NULL;
	if (m_type == CPT_Channel) {
		m_chatlog_text->SetToolTip(_("right click for options (like autojoin)"));
	}

	wxBitmap ico;
	if (m_type == CPT_User && (m_user != NULL)) {
		ico = IconsCollection::Instance()->GetUserBattleStateBmp(m_user->GetStatus());
	} else {
		ico = IconsCollection::Instance()->BMP_CHANNEL_OPTIONS;
	}
	m_chan_opts_button = new wxBitmapButton(m_chat_panel, CHAT_CHAN_OPTS, ico, wxDefaultPosition, wxSize(CONTROL_HEIGHT, CONTROL_HEIGHT));

	m_say_text = new wxTextCtrlHist(
	    textcompletiondatabase, m_chat_panel, CHAT_TEXT,
	    wxEmptyString, wxDefaultPosition, wxSize(100, CONTROL_HEIGHT),
	    wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
	m_say_button = new wxButton(m_chat_panel, CHAT_SEND, _("Send"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT));

	// Adding elements to sizers
	if (m_chan_opts_button != NULL)
		m_say_sizer->Add(m_chan_opts_button);
	m_say_sizer->Add(m_say_text, 1, wxEXPAND);
	m_say_sizer->Add(m_say_button);
	m_chat_sizer->Add(m_chatlog_text, 1, wxEXPAND);
	m_chat_sizer->Add(m_say_sizer, 0, wxEXPAND | wxTOP, 2);
	if (m_show_nick_list) {
		m_chat_panel->SetSizer(m_chat_sizer);
		m_splitter->SplitVertically(m_chat_panel, m_nick_panel, 100);
		m_splitter->SetMinimumPaneSize(MINIMUM_PANE_SIZE);
		m_main_sizer->Add(m_splitter, 1, wxEXPAND | wxALL, 2);
	} else {
		m_main_sizer->Add(m_chat_sizer, 4, wxEXPAND | wxALL, 2);
	}

	// Assign sizer to panel
	SetSizer(m_main_sizer);

	if (m_show_nick_list) {
		wxSize s = m_splitter->GetSize();
		m_splitter->SetSashPosition(s.GetWidth() - MINIMUM_PANE_SIZE, true);
	}

	m_chatlog_text->ShowPosition(m_chatlog_text->GetLastPosition());
	m_chatlog_text->SetBackgroundColour(sett().GetChatColorBackground());
	m_say_text->SetBackgroundColour(sett().GetChatColorBackground());
	m_say_text->SetForegroundColour(sett().GetChatColorNormal());

	// Fill up TextCompletionDatabase
	textcompletiondatabase.Insert_Mapping(_T("DLDK"), _T("Der Lockruf des Kaos"));
	textcompletiondatabase.Insert_Mapping(_T("lol"), _T("Laughing out loud"));
	textcompletiondatabase.Insert_Mapping(_T("wb"), _T("Welcome back"));
	textcompletiondatabase.Insert_Mapping(_T("hf"), _T("Have Fun!"));
	textcompletiondatabase.Insert_Mapping(_T("glhf"), _T("Good luck, have Fun!"));
	textcompletiondatabase.Insert_Mapping(_T("kaot"), _T("Have Fun!"));
	textcompletiondatabase.Insert_Mapping(_T("kaot_H"), _T("Der Kaot aus der HĂślle."));
}

void ChatPanel::CreatePopup()
{
	if (m_popup_menu != NULL)
		return;
	slLogDebugFunc("");
	m_popup_menu = new ChatPanelMenu(this);
}

User* ChatPanel::GetSelectedUser() const
{
	if (m_type == CPT_User) {
		return m_user;
	}
	if (!m_show_nick_list || (m_nicklist == 0))
		return 0;

	return m_nicklist->GetSelectedItem();
}


const User& ChatPanel::GetMe() const
{
	return serverSelector().GetServer().GetMe();
}

void ChatPanel::OutputLine(const wxString& message, const wxColour& col, bool showtime)
{

	if (!m_chatlog_text)
		return;

	ChatLine newline;
	newline.chat = message;
	newline.chatstyle = wxTextAttr(col, sett().GetChatColorBackground());
	if (showtime) {
		wxDateTime now = wxDateTime::Now();
		newline.time = _T( "[" ) + now.Format(_T( "%H:%M:%S" )) + _T( "]" );
		newline.timestyle = wxTextAttr(sett().GetChatColorTime(), sett().GetChatColorBackground());
		m_chat_log.AddMessage(newline.time + _T(" ") + message);
	} else {
		newline.time.clear();
	}

	if (m_disable_append) {
		m_buffer.push_back(newline);
	} else {
		OutputLine(newline);
	}
}

static size_t FirstSpecialChar(const wxString& str)
{
	for (size_t pos = 0; pos < str.Len(); pos++) {
		if (!str.GetChar(pos).IsAscii()) {
			continue;
		}
		const unsigned char uc = str.GetChar(pos);
		if (uc == 0x1f || uc == 0x1d || uc == 0x03 || uc == 0x02 || uc == 0x016 || uc == 0x0F) { //get all text until first irc color is found
			return pos;
		}
	}
	return -1;
}

void ChatPanel::OutputLine(const ChatLine& line)
{
	const int numOfLines = m_chatlog_text->GetNumberOfLines();
	const int maxlength = sett().GetChatHistoryLenght();

	// crop lines from history that exceeds limit
	if ((maxlength > 0) && (numOfLines > maxlength)) {
		int end_line = 0;
		for (int i = 0; i < numOfLines - maxlength; i++) {
			end_line += m_chatlog_text->GetLineLength(i) + 1;
		}
		if (end_line > 0) {
			m_chatlog_text->Remove(0, end_line);
		}
	}

	if (!line.time.empty()) {
		m_chatlog_text->SetDefaultStyle(line.timestyle);
		m_chatlog_text->AppendText(line.time + _T(" "));
	} else {
		m_chatlog_text->SetDefaultStyle(line.chatstyle);
	}

#ifndef __WXOSX_COCOA__
	if (sett().GetUseIrcColors()) {
		wxString m1(line.chat);
		wxString m2;
		wxString m3;
		wxTextAttr at(line.chatstyle);
		int color = 0;
		bool bold = false;
		wxColor curcolor(line.chatstyle.GetTextColour());
		const wxFont oldfont = sett().GetChatFont();
		const wxColor oldcolor(line.chatstyle.GetTextColour());

		while (m1.Len() > 0) {
			const size_t firstspec = FirstSpecialChar(m1);
			if (firstspec > 0) {	   //unformated text found, add as whole
				wxFont font = oldfont; //isn't needed any more in wx3.0
				at = line.chatstyle;
				if (bold)
					font.SetWeight(wxFONTWEIGHT_BOLD);
				at.SetFont(font);
				at.SetTextColour(curcolor);
				m_chatlog_text->SetDefaultStyle(at);
				m_chatlog_text->AppendText(m1.Mid(0, firstspec));
				m1 = m1.Mid(firstspec);
				if (m1.Len() <= 0) { //no chars left, abort
					break;
				}
			}
			const unsigned char uc = m1.GetChar(0);
			switch (uc) {      //  http://en.wikichip.org/wiki/irc/colors
				case 0x1f: //underline
					break;
				case 0x1d: //italics
					break;
				case 0x03: {
					if (m1.Len() > 2 && (m1.GetChar(2) >= 48 && m1.GetChar(2) <= 58)) {
						color = (int(m1.GetChar(1)) - 48) * 10 + (int(m1.GetChar(2)) - 48);
						m1 = m1.Mid(2); //next chars
					} else {
						color = int(m1.GetChar(1)) - 48;
						m1 = m1.Mid(1); //next char
					}

					wxColor dummy(0, 0, 0);
					if ((color > -1) && (color < long((sizeof(m_irc_colors) / sizeof(dummy))))) {
						curcolor = m_irc_colors[color];
					}
					break;
				}
				case 0x02: //bold
					bold = !bold;
					break;
				case 0x16: //reverse;
					break;
				case 0x0F: //reset formating
					bold = false;
					curcolor = oldcolor;
					break;
			}
			m1 = m1.Mid(1); //next char!
		}
		m_chatlog_text->AppendText(_T("\n"));
	} else
#endif
	{
		m_chatlog_text->SetDefaultStyle(line.chatstyle);
		m_chatlog_text->AppendText(line.chat + _T( "\n" ));
	}


	m_chatlog_text->ScrollLines(1);
	m_chatlog_text->ShowPosition(m_chatlog_text->GetLastPosition());
}


void ChatPanel::OnLinkEvent(wxTextUrlEvent& event)
{
	if (!event.GetMouseEvent().LeftDown())
		return;

	wxString url = m_chatlog_text->GetRange(event.GetURLStart(), event.GetURLEnd());
	OpenWebBrowser(url);
}

void ChatPanel::OnChanOpts(wxCommandEvent& /*unused*/)
{
	CreatePopup();
	if ((m_chan_opts_button == NULL) || (m_popup_menu == 0))
		return;
	m_chan_opts_button->PopupMenu(m_popup_menu->GetMenu());
}


void ChatPanel::OnSay(wxCommandEvent& /*unused*/)
{
	if (Say(m_say_text->GetValue()))
		m_say_text->SetValue(wxEmptyString);
	/*
	Say("\x02Test\x02Test");
	Say("\x02\x02Test\x02Test");
	Say("Test\x02Test");
	Say("TestTes\x02t");
	Say("TestTest\x02");
*/
}

void ChatPanel::OnFilterUsers(wxCommandEvent& /*unused*/)
{
	m_nicklist->SetUsersFilterString(m_nick_filter->GetValue());
	UpdateUserCountLabel();
}

void ChatPanel::OnPaste(wxClipboardTextEvent& event)
{
	// Read some text
	if (wxTheClipboard->Open()) {
		wxTextDataObject data;
		if (wxTheClipboard->GetData(data)) {
			wxString converted = data.GetText();
			converted.Replace(_T("\r\n"), _T("\n"));
			converted.Replace(_T("\r"), _T("\n"));
			m_say_text->WriteText(converted);
		} else
			event.Skip();
	} else
		event.Skip();
	wxTheClipboard->Close();
}


//! @brief Output a message said in the channel.
//!
//! @param who nick of the person who said something.
//! @param message the message to be outputted.
void ChatPanel::Said(const wxString& who, const wxString& message)
{
	if (m_active_users.find(who) == m_active_users.end()) {
		m_active_users.insert(who);
	}

	wxString me = TowxString(GetMe().GetNick());
	wxColour col;
	bool req_user = false;
	if (who.Upper() == me.Upper()) {
		col = sett().GetChatColorMine();
	} else {
		// change the image of the tab to show new events
		SetIconHighlight(highlight_say);
		if (m_type == CPT_User)
			req_user = true;
		//process logic for custom word highlights
		if (ContainsWordToHighlight(message)) {
			req_user = sett().GetRequestAttOnHighlight();
			col = sett().GetChatColorHighlight();
		} else
			col = sett().GetChatColorNormal();
	}

	if ((who == cfg().ReadString(_T("/Channel/bridgebot"))) && (message.StartsWith(_T( "<" ))) && (message.Find(_T( ">" )) != wxNOT_FOUND)) {
		wxString who2;
		wxString message2;
		who2 = message.BeforeFirst('>').AfterFirst('<');
		message2 = message.AfterFirst('>');
		OutputLine(_T( "<" ) + who2 + _T( "> " ) + message2, col);
	} else {
		OutputLine(_T( "<" ) + who + _T( "> " ) + message, col);
	}


	if (req_user) {
		bool inactive = ui().GetActiveChatPanel() != this || !wxTheApp->IsActive();
		if (inactive) {
			ui().mw().RequestUserAttention();
			const wxString msg = wxString::Format(_T("%s:\n%s"), who.c_str(), message.Left(50).c_str());
			UiEvents::GetNotificationEventSender().SendEvent(
			    UiEvents::NotficationData(UiEvents::PrivateMessage, msg));
		}
	}
}

bool ChatPanel::ContainsWordToHighlight(const wxString& message) const
{
	//get list of words to highlight
	wxArrayString words = sett().GetHighlightedWords();
	for (unsigned int i = 0; i < words.GetCount(); i++) {
		if (message.Find(words[i]) != wxNOT_FOUND)
			return true;
	}
	return false;
}

/**
 *	Check if there is "promote" event and notify user if needed
 */
void ChatPanel::CheckForPromotion(const wxString& /*who*/, const wxString& action)
{
	if (!m_reactOnPromoteEvents) { //If this feature disabled in options do nothing
		return;
	}

	//TODO: Make promote templates be user-configurable and uses regexp
	const wxString promoteMessageTemplate = _T(" player(s) needed for battle \"");
	const wxString promoteMessageTemplate2 = _T("). Come play and enjoy !");

	//Detect event and notify user
	//TODO: Rework for using array of regexps, not shit crap
	if (action.Contains(promoteMessageTemplate) || action.Contains(promoteMessageTemplate2)) {
		UiEvents::GetNotificationEventSender().SendEvent(UiEvents::NotficationData(UiEvents::GamePromoted, action));
	}
}

/**
 * Process text sent by the server, not users
 * Check for vote events, promote events and something else
 */
void ChatPanel::DidAction(const wxString& who, const wxString& action)
{
	//Handle vote events in chat by VotePanel
	if (m_votePanel != NULL) {
		m_votePanel->OnChatAction(who, action);
	}

	//Look for "promote" notifications from server
	CheckForPromotion(who, action);

	// change the image of the tab to show new events
	SetIconHighlight(highlight_say);
	OutputLine(_T( "* " ) + who + _T( " " ) + action, sett().GetChatColorAction());
	if (m_type == CPT_User && (ui().GetActiveChatPanel() != this || !wxTheApp->IsActive())) {
		const wxString msg = wxString::Format(_T("%s \n%s"), who.c_str(), action.Left(50).c_str());
		UiEvents::GetNotificationEventSender().SendEvent(
		    UiEvents::NotficationData(UiEvents::PrivateMessage, msg));
	}
}

//! @brief Output motd sent by server
//!
//! @param message The MOTD message to output
void ChatPanel::Motd(const wxString& message)
{
	// change the image of the tab to show new events
	SetIconHighlight(highlight_say);
	OutputLine(_T( "** motd ** " ) + message, sett().GetChatColorServer());
}

void ChatPanel::StatusMessage(const wxString& message)
{
	if (m_chatlog_text == 0) {
		wxLogMessage(_T( "m_chatlog_text is NULL" ));
	} else {
		if (CPT_Server == m_type)
			SetIconHighlight(highlight_important);
		OutputLine(_T( "** Server ** " ) + message, sett().GetChatColorServer());
	}
}

void ChatPanel::ClientMessage(const wxString& message)
{
	OutputLine(_T( "** " ) + message, sett().GetChatColorClient());
}

void ChatPanel::UnknownCommand(const wxString& command, const wxString& params)
{
	// change the image of the tab to show new events
	SetIconHighlight(highlight_important);
	OutputLine(_("!! Command: \"") + command + _("\" params: \"") + params + _T( "\"." ), sett().GetChatColorError());
}

void ChatPanel::OutputError(const wxString& message)
{
	OutputLine(wxString::Format(_("Error: %s"), message.c_str()), sett().GetChatColorError());
}


wxString ChatPanel::GetChatTypeStr() const
{
	if (m_type == CPT_Channel)
		return _("the channel");
	else if (m_type == CPT_Battle)
		return _("the battle");
	else if (m_type == CPT_Server)
		return _("the server");

	return _T( "ROOMTYPE FIXME" );
}

void ChatPanel::Joined(User& who)
{
	if (m_display_joinitem) {
		// change the image of the tab to show new events
		if (m_type == CPT_Channel) {
			SetIconHighlight(highlight_join_leave);
		}
		OutputLine(_T( "** " ) + wxString::Format(_("%s joined %s."), TowxString(who.GetNick()).c_str(), GetChatTypeStr().c_str()), sett().GetChatColorJoinPart());
	}

	if (m_show_nick_list && (m_nicklist != nullptr)) {
		m_nicklist->AddUser(who);
		UpdateUserCountLabel();
	}
	// Also add the User to the TextCompletionDatabase
	textcompletiondatabase.Insert_Mapping(TowxString(who.GetNick()), TowxString(who.GetNick()));
}

void ChatPanel::OnChannelJoin(User& who)
{
	//    assert( m_type == CPT_Channel || m_type == CPT_Server || m_type == CPT_Battle || m_type == CPT_User );
	if (m_show_nick_list && (m_nicklist != nullptr)) {
		m_nicklist->AddUser(who);
		UpdateUserCountLabel();
	}
	if (m_display_joinitem) {
		OutputLine(_T( "** " ) + wxString::Format(_("%s joined %s."), TowxString(who.GetNick()).c_str(), GetChatTypeStr().c_str()), sett().GetChatColorJoinPart());
	}
	// Also add the User to the TextCompletionDatabase
	textcompletiondatabase.Insert_Mapping(TowxString(who.GetNick()), TowxString(who.GetNick()));
}

void ChatPanel::Parted(User& who, const wxString& message)
{
	//    assert( m_type == CPT_Channel || m_type == CPT_Server || m_type == CPT_Battle || m_type == CPT_User );
	const bool me_parted = m_channel && &who == &m_channel->GetMe();
	const wxString nick = TowxString(who.GetNick());
	const bool wasactive = m_active_users.erase(nick) > 0;
	if (m_display_joinitem || (wasactive && !who.IsBot())) {
		OutputLine(_T( "** " ) + wxString::Format(_("%s left %s (%s)."), nick.c_str(), GetChatTypeStr().c_str(), message.c_str()), sett().GetChatColorJoinPart());
	}
	if (m_type == CPT_Channel) {
		if (m_channel == 0)
			return;
		if (me_parted) {
			m_channel->panel = nullptr;
			SetChannel(0);
			return;
		}
		if (m_display_joinitem) {
			// change the image of the tab to show new events
			SetIconHighlight(highlight_join_leave);
		}
	} else if (m_type == CPT_Server && me_parted)
		return;
	if (m_show_nick_list && (m_nicklist != nullptr)) {
		m_nicklist->RemoveUser(who);
		UpdateUserCountLabel();
	}
	// Also remove the User from the TextCompletionDatabase
	textcompletiondatabase.Delete_Mapping(TowxString(who.GetNick()));
}

void ChatPanel::SetTopic(const wxString& who, const wxString& message)
{
	/*
	int pos = refined.Find( _T("\\n") ); // serch for the \n string
	while ( pos != -1 )
	{
	if ( refined.Mid( pos - 1, 3 ) == _T("\\\n") ) continue; // the string \\n means escaped \n
	refined = refined.Left ( pos -1 ) + _T("\n") + refined.Right( pos +1 ); // replace the /n string with the carriage return char
	pos = refined.Find( _T("\\n") );
	}
	*/
	const wxColor col = sett().GetChatColorServer();
	// change the image of the tab to show new events
	if (m_topic_set)
		SetIconHighlight(highlight_say);
	OutputLine(_("** Channel topic:"), col);
	wxStringTokenizer tkz(message, _T("\n"));
	while (tkz.HasMoreTokens()) {
		wxString msg = tkz.GetNextToken();
		OutputLine(_T(" ") + msg, col);
	}
	OutputLine(_("** Set by ") + who, col);
	m_topic_set = true;
}

void ChatPanel::UserStatusUpdated(User& who)
{
	if ((m_type == CPT_User) && (m_user == &who) && (m_chan_opts_button != NULL)) {
		m_chan_opts_button->SetBitmapLabel(IconsCollection::Instance()->GetUserListStateBmp(who.GetStatus(), false, who.GetBattle() != 0));
	}
	if (!m_show_nick_list || (m_nicklist == nullptr))
		return;
	try {
		m_nicklist->UserUpdated(who);
	} catch (...) {
		return;
	}
}

const Channel* ChatPanel::GetChannel() const
{
	return m_channel;
}

void ChatPanel::SetChannel(Channel* chan)
{
	ASSERT_LOGIC(this, "this==null");
	ASSERT_LOGIC(m_type == CPT_Channel, "Not of type channel");

	if ((chan == 0) && (m_channel != 0)) {
		m_channel->panel = nullptr;
	}
	if (m_nicklist != nullptr) {
		m_nicklist->ClearUsers();
		UpdateUserCountLabel();
	}

	if (chan != 0) {
		chan->panel = this;
		if (chan != m_channel) {
			SetLogFile(TowxString(chan->GetName()));
		}
	}
	m_channel = chan;

	//set back to false so when we rejoin this channel SetTopic doesn;t update the chan icon
	if (!m_channel)
		m_topic_set = false;
}

const IServer* ChatPanel::GetServer() const
{
	return m_server;
}

void ChatPanel::SetServer(IServer* serv)
{
	ASSERT_LOGIC(m_type == CPT_Server, "Not of type server");
	if (m_nicklist != nullptr) {
		m_nicklist->ClearUsers();
		UpdateUserCountLabel();
	}

	if (serv != nullptr) {
		SetLogFile(_T("server"));
		serv->panel = this;
	}
	if (m_server != nullptr) {
		m_server->panel = 0;
	}
	m_server = serv;
}

const User* ChatPanel::GetUser() const
{
	return m_user;
}

void ChatPanel::SetUser(User* usr)
{
	ASSERT_LOGIC(m_type == CPT_User, "Not of type user");

	if (usr == NULL) {
		if (m_user != NULL) {
			m_user->panel = nullptr;
			if (m_chan_opts_button != NULL) {
				m_chan_opts_button->SetBitmapLabel(IconsCollection::Instance()->BMP_EMPTY);
			}
		}
	} else {
		usr->panel = this;
		if (m_chan_opts_button != NULL) {
			const wxBitmap icon = IconsCollection::Instance()->GetUserListStateIcon(usr->GetStatus(), false, usr->GetBattle() != 0);
			m_chan_opts_button->SetBitmapLabel(icon);
		}
	}
	m_user = usr;
}

ChatPanelType ChatPanel::GetPanelType() const
{
	return m_type;
}


bool ChatPanel::Say(const wxString& message) //FIXME: remove all parsing / tokenizing / ... to dedicated file
{
	static const unsigned int flood_threshold = 5;
	slLogDebugFunc("");
	wxStringTokenizer lines(message, _T('\n'));
	if (lines.CountTokens() > flood_threshold) {
		PasteDialog dl(this, wxString::Format(_("Are you sure you want to paste %d lines?"), lines.CountTokens()));
		switch (dl.ShowModal()) {
			case wxID_NO:
				return true;
			case PasteDialog::pasteButtonReturnCode: {
				wxString url = Paste2Pastebin(message);
				if (url != wxEmptyString && wxStringTokenizer(url, _T('\n')).CountTokens() <= flood_threshold) {
					Say(url);
					return true;
				} else {
					customMessageBoxModal(SL_MAIN_ICON, _("Failed to post to pastebin.com."));
					return false;
				}
			}
			default:
				break;
		}
	}
	while (lines.HasMoreTokens()) {
		wxString line = lines.GetNextToken();
		wxLogDebug(_T( "line: %s" ), line.c_str());

		if (line == "/help") {
			ui().ConsoleHelp();
			return true;
		}

		if (line == "/channels") {
			ui().mw().ShowChannelChooser();
			return true;
		}

		if (line == _T( "/ver" )) {
			//!this instance is not replaced with GetAppname for sake of help/debug online
			OutputLine(_("You have SpringLobby v") + TowxString(getSpringlobbyVersion()), sett().GetChatColorNormal());
			return true;
		}

		if (line == _T( "/clear" )) {
			m_chatlog_text->SetValue(wxEmptyString);
			return true;
		}

		if (m_type == CPT_Channel) {

			if (m_channel == 0) {
				OutputError(_("You are not in channel or channel does not exist."));
				return true;
			}
			if (line.StartsWith(_T( "/" ))) {
				if (m_channel->ExecuteSayCommand(STD_STRING(line)))
					return true;
				if (m_channel->GetServer().ExecuteSayCommand(STD_STRING(line)))
					return true;
				OutputError(wxString::Format(_("Command (%s) does not exist, use /help for a list of available commands."), line.c_str()));
				return true;
			}
			m_channel->Say(STD_STRING(line));

		} else if (m_type == CPT_Battle) {

			if (m_battle == 0) {
				OutputError(_("You are not in battle or battle does not exist, use /help for a list of available commands."));
				return true;
			}
			if (line.StartsWith(_T( "/" ))) {
				if (m_battle->ExecuteSayCommand(STD_STRING(line)))
					return true;
				if (m_battle->GetServer().ExecuteSayCommand(STD_STRING(line)))
					return true;
				OutputError(wxString::Format(_("Command (%s) does not exist, use /help for a list of available commands."), line.c_str()));
				return true;
			}
			m_battle->Say(STD_STRING(line));

		} else if (m_type == CPT_User) {

			if (m_user == 0) {
				OutputError(_(" User is offline."));
				return true;
			}
			if (line.StartsWith(_T( "/" ))) {
				if (m_user->ExecuteSayCommand(STD_STRING(line)))
					return true;
				if (m_user->GetServer().ExecuteSayCommand(STD_STRING(line)))
					return true;
				OutputError(wxString::Format(_("Command (%s) does not exist, use /help for a list of available commands."), line.c_str()));
				return true;
			}
			m_user->Say(STD_STRING(line));

		} else if (m_type == CPT_Server) {
			if (m_server == 0) {
				OutputError(_("Not connected to server"));
				return true;
			}

			if (line.StartsWith(_T( "/" ))) {
				if (m_server->ExecuteSayCommand(STD_STRING(line)))
					return true;
				OutputError(wxString::Format(_("Command (%s) does not exist, use /help for a list of available commands."), line.c_str()));
				return true;
			}

			//we need to disable the channel tab if leaving manually
			if (line.Upper().StartsWith(_T( "LEAVE" ))) {
				wxString channame = line.AfterFirst(' ').BeforeFirst(' ');
				try {
					Channel& chan = m_server->GetChannel(STD_STRING(channame));
					chan.Leave();
					chan.panel = 0;
				} catch (assert_exception) {
				}
			}

			m_server->SendCmd(STD_STRING(line), "");
			OutputLine(_("Sent: \"") + line + _("\""), sett().GetChatColorNormal());
		}
	}
	return true;
}

void ChatPanel::Part()
{
	slLogDebugFunc("");
	if (m_type == CPT_Channel) {
		if (m_channel == 0)
			return;
		m_channel->Leave();
		m_channel->panel = 0;
	}
}

bool ChatPanel::IsOk() const
{
	if (m_type == CPT_Channel)
		return (m_channel != 0);
	if (m_type == CPT_Server)
		return (m_server != 0);
	if (m_type == CPT_User)
		return (m_user != 0);
	if (m_type == CPT_Battle)
		return (m_battle != 0);
	return false;
}

void ChatPanel::OnUserDisconnected()
{
	// change the image of the tab to show new events
	SetIconHighlight(highlight_join_leave);
	OutputLine(_("** User is now offline."), sett().GetChatColorJoinPart());
}

void ChatPanel::OnUserConnected()
{
	// change the image of the tab to show new events
	SetIconHighlight(highlight_join_leave);
	OutputLine(_("** User just got online."), sett().GetChatColorJoinPart());
}


//////////////////////////////////////////////////////////////////////////////////////
// Menu Events
//////////////////////////////////////////////////////////////////////////////////////

void ChatPanel::FocusInputBox()
{
	m_say_text->SetFocus();
}

wxString ChatPanel::FindUrl(const long pos) const
{
	if (pos < 0) {
		return wxEmptyString;
	}
	long last = m_chatlog_text->GetLastPosition();

	long start = pos;
	while (start > 0 && m_chatlog_text->GetRange(start - 1, start) != _T(" "))
		start--;

	long end = pos;
	//the last bit prevents topic links from being copied with next line's timestamp appended
	//checking for newline isn't possible, cause links may well span multiple lines
	while (end < last && m_chatlog_text->GetRange(end, end + 1) != _T(" ") && m_chatlog_text->GetRange(end, end + 1) != _T("["))
		end++;

	wxString ret = m_chatlog_text->GetRange(start, end);
	//! \todo there's prolly some smarter way to capture a more flexible range of url types
	if (ret.StartsWith(_T("http://")) || ret.StartsWith(_T("https://")) || ret.StartsWith(_T("ftp://")))
		return ret;
	else
		return wxEmptyString;
}

void ChatPanel::OnMouseDown(wxMouseEvent& event)
{
	slLogDebugFunc("");
	wxTextCoord row;
	wxTextCoord col;
	wxTextCtrlHitTestResult ht = m_chatlog_text->HitTest(event.GetPosition(), &col, &row);
	if (ht != wxTE_HT_UNKNOWN) {
		long pos = m_chatlog_text->XYToPosition(col, row);
		m_url_at_pos = FindUrl(pos);
	}
	CreatePopup();
	if (m_popup_menu != NULL) {
		PopupMenu(m_popup_menu->GetMenu());
	} else {
		event.Skip();
	}
}

void ChatPanel::UpdateNicklistHighlights()
{
	if (m_show_nick_list && (m_nicklist != nullptr)) {
		m_nicklist->Refresh();
	}
}

void ChatPanel::SortNickList()
{
	if (m_show_nick_list && (m_nicklist != 0))
		m_nicklist->Resort();
}

void ChatPanel::SetIconHighlight(HighlightType highlight)
{
	if (ui().GetActiveChatPanel() != this && m_chat_tabs && !IsShownOnScreen()) {
		for (unsigned int i = 0; i < m_chat_tabs->GetPageCount(); ++i) {
			if (m_chat_tabs->GetPage(i) == this) { //!TODO: return afterwards?
				switch (highlight) {
					case highlight_say: {
						if (m_type == CPT_Channel && m_icon_index < 6) {
							m_icon_index = 6;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						} else if (m_type == CPT_User && m_icon_index < 7) {
							m_icon_index = 7;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						}
						break;
					}
					case highlight_join_leave: {
						if (m_type == CPT_Channel && m_icon_index < 4) {
							m_icon_index = 4;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						} else if (m_type == CPT_User && m_icon_index < 5) {
							m_icon_index = 5;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						}
						break;
					}
					case highlight_important: {
						if (m_type == CPT_Channel && m_icon_index < 8) {
							m_icon_index = 8;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						} else if (m_type == CPT_User && m_icon_index < 9) {
							m_icon_index = 9;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						} else if (m_type == CPT_Server && m_icon_index < 10) {
							m_icon_index = 10;
							m_chat_tabs->SetPageBitmap(i, m_imagelist->GetBitmap(m_icon_index));
						}
						break;
					}
				}
			}
		}
	}
}

void ChatPanel::OnMenuItem(wxCommandEvent& event)
{
	m_popup_menu->OnMenuItem(event);
}

void ChatPanel::SetBattle(IBattle* battle)
{
	if (m_battle == battle) { // battle didn't change, skip
		return;
	}

	if (m_battle != NULL) {
		OutputLine(_("** Left Battle."), sett().GetChatColorNotification());
	}

	if (battle == NULL) {
		SetLogFile(wxEmptyString);
		return;
	}

	OutputLine(_("** Joined Battle."), sett().GetChatColorNotification());

	for (unsigned int i = 0; i < battle->GetNumUsers(); ++i) {
		const wxString nick = TowxString(battle->GetUser(i).GetNick());
		textcompletiondatabase.Insert_Mapping(nick, nick);
	}

	SetLogFile(_T( "_BATTLE_" ) + TowxString(battle->GetFounder().GetNick()));
	m_battle = battle;
}

void ChatPanel::OnLogin(wxCommandEvent& /*data*/)
{
	switch (m_type) {
		case CPT_Channel:
			if (m_channel) {
				m_channel->Rejoin();
			}
			break;
		case CPT_Server:
			assert(m_server != nullptr);
			m_nicklist->SetUsers(m_server->GetUserList().GetUsers());
			UpdateUserCountLabel();
		case CPT_Battle:
		case CPT_User:
		case CPT_Debug:
			break;
	}
}

void ChatPanel::SetLogFile(const wxString& name)
{
	m_chat_log.SetLogFile(name);
	if (name.empty()) {
		return;
	}
	wxWindowUpdateLocker noUpdates(m_chatlog_text);
	wxArrayString lines = m_chat_log.GetLastLines();
	const size_t num_lines = sett().GetAutoloadedChatlogLinesCount();
	const size_t start = std::max<size_t>(0, lines.Count() - num_lines);
	for (size_t i = start; i < lines.Count(); ++i) {
		OutputLine(lines[i], sett().GetChatColorServer(), false);
	}
}

void ChatPanel::SetVotePanel(VotePanel* votePanel)
{
	m_votePanel = votePanel;
	m_votePanel->SetChatPanel(this);
}

/**
 * Update User Count label on top of users list
 * If available shows filtered number of users
 */
void ChatPanel::UpdateUserCountLabel()
{
	if (m_usercount_label == NULL)
		return;

	unsigned int numusers = 0;

	//Regular channel
	if ((m_type == CPT_Channel) && (GetChannel() != nullptr)) {
		if (m_nicklist != nullptr) {
			numusers = m_nicklist->GetUsersCount();
		} else {
			numusers = GetChannel()->GetNumUsers();
		}
		//Server channel
	} else if ((m_type == CPT_Server) && (m_server != NULL)) {
		if (m_nicklist != nullptr) {
			numusers = m_nicklist->GetUsersCount();
		} else {
			numusers = m_server->GetNumUsers();
		}
	}
	//Update label
	m_usercount_label->SetLabel(wxString::Format(_("%d users"), numusers));
}


void ChatPanel::OnShowPlayerOnlyCheck(wxCommandEvent& event)
{
	m_nicklist->UserFilterShowPlayersOnly(event.IsChecked());
	UpdateUserCountLabel();
}

void ChatPanel::OnSettingsChanged(wxCommandEvent&)
{
	ReadSettings();
}

void ChatPanel::ReadSettings()
{

	m_reactOnPromoteEvents = cfg().ReadBool("/GUI/ShowPromotions");
	m_display_joinitem = cfg().Read(_T( "/Channels/DisplayJoinLeave/" ) + m_chatpanelname, m_display_joinitem);

	//Hide bots by default
	m_ShowPlayersOnlyFlag = true;

	if (m_chatlog_text != nullptr) {
		m_chatlog_text->SetBackgroundColour(sett().GetChatColorBackground());
	}
	if (m_say_text != nullptr) {
		m_say_text->SetBackgroundColour(sett().GetChatColorBackground());
		m_say_text->SetForegroundColour(sett().GetChatColorNormal());
	}
}
