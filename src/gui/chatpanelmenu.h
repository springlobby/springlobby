/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CHATPANELMENU_H
#define SPRINGLOBBY_HEADERGUARD_CHATPANELMENU_H

#include <wx/menu.h>
#include "usermenu.h"
#include "utils/mixins.h"
#include "servermanager.h"

//! seperating this into a friend class enables us to re-use it in the tab header right click evt
class ChatPanelMenu : public wxEvtHandler, public SL::NonCopyable
{
public:
	typedef SL_GENERIC::UserMenu<ChatPanelMenu>
	    UserMenu;

	ChatPanelMenu(ChatPanel* parent, bool addChanServ = true, const wxString& title = wxEmptyString, long style = 0);
	UserMenu* GetUserMenu();

	void OnMenuToggleAppend(wxCommandEvent& event);

	void OnChannelMenuLeave(wxCommandEvent& event);
	void OnChannelMenuDisplayJoinLeave(wxCommandEvent& event);
	void OnChannelMenuInfo(wxCommandEvent& event);
	void OnChannelMenuTopic(wxCommandEvent& event);
	void OnChannelMenuMessage(wxCommandEvent& event);
	void OnChannelMenuLock(wxCommandEvent& event);
	void OnChannelMenuUnlock(wxCommandEvent& event);
	void OnChannelMenuRegister(wxCommandEvent& event);
	void OnChannelMenuUnregister(wxCommandEvent& event);
	void OnChannelMenuSpamOn(wxCommandEvent& event);
	void OnChannelMenuSpanOff(wxCommandEvent& event);
	void OnChannelMenuSpamIsOn(wxCommandEvent& event);
	void OnChannelMenuShowMutelist(wxCommandEvent& event);

	void OnServerMenuDisconnect(wxCommandEvent& event);
	void OnServerMenuReconnect(wxCommandEvent& event);
	void OnServerMenuRemove(wxCommandEvent& event);
	void OnServerMenuChangePassword(wxCommandEvent& event);
	void OnServerMenuSetAccess(wxCommandEvent& event);
	void OnServerMenuBroadcast(wxCommandEvent& event);

	void OnUserMenuOpenChat(wxCommandEvent& event);
	void OnUserMenuJoinSame(wxCommandEvent& event);
	void OnUserMenuSlap(wxCommandEvent& event);
	void OnUserMenuMute(wxCommandEvent& event);
	void OnUserMenuUnmute(wxCommandEvent& event);
	void OnUserMenuKick(wxCommandEvent& event);
	void OnUserMenuOp(wxCommandEvent& event);
	void OnUserMenuDeop(wxCommandEvent& event);
	void OnUserMenuModeratorIngame(wxCommandEvent& event);
	void OnUserMenuModeratorCurrentIP(wxCommandEvent& event);
	void OnUserMenuModeratorKick(wxCommandEvent& event);
	void OnUserMenuModeratorBan(wxCommandEvent& event);
	void OnUserMenuModeratorUnban(wxCommandEvent& event);
	void OnUserMenuModeratorMute(wxCommandEvent& event);
	void OnUserMenuModeratorMute5(wxCommandEvent& event);
	void OnUserMenuModeratorMute10(wxCommandEvent& event);
	void OnUserMenuModeratorMute30(wxCommandEvent& event);
	void OnUserMenuModeratorMute120(wxCommandEvent& event);
	void OnUserMenuModeratorMute1440(wxCommandEvent& event);
	void OnUserMenuModeratorUnmute(wxCommandEvent& event);
	void OnUserMenuModeratorRing(wxCommandEvent& event);

	void OnUserMenuCopyLink(wxCommandEvent& event);

	void OnChatMenuOpenLog(wxCommandEvent& event);

	void OnChannelClearContents(wxCommandEvent& event);

	void OnUserMenuAddToGroup(wxCommandEvent& event);
	void OnUserMenuDeleteFromGroup(wxCommandEvent& event);
	void OnUserMenuCreateGroup(wxCommandEvent& event);

	void OnMenuItem(wxCommandEvent& event);
	void OnChannelSubscribe(wxCommandEvent& unused);

	wxMenu* GetMenu();
	ChatPanelMenu::UserMenu* GetUserMenuNoCreate();

private:
	bool HasChanserv();
	void SayChanserv(const std::string& msg);
	void CreateNickListMenu();
	void MutePlayer(long dur);

	ChatPanel* m_chatpanel;

	UserMenu* m_user_menu;
	wxMenu* m_menu_all;

	wxMenuItem* displayjoinitem;
	wxMenuItem* m_subscribe;
	wxMenuItem* m_append_menu;

	bool m_withChanserv;
};


static const long CHAT_MENU_SHOW_MUTELIST = wxNewId();
static const long CHAT_MENU_DISABLE_APPEND = wxNewId();

static const long CHAT_MENU_CH_LEAVE = wxNewId();
static const long CHAT_MENU_CH_DISPLAYJOIN = wxNewId();
static const long CHAT_MENU_CH_SUBSCRIBE = wxNewId();

static const long CHAT_MENU_CH_TOPIC = wxNewId();
static const long CHAT_MENU_CH_INFO = wxNewId();
static const long CHAT_MENU_CH_MSG = wxNewId();
static const long CHAT_MENU_CH_LOCK = wxNewId();
static const long CHAT_MENU_CH_UNLOCK = wxNewId();
static const long CHAT_MENU_CH_REG = wxNewId();
static const long CHAT_MENU_CH_UNREG = wxNewId();
static const long CHAT_MENU_CH_SPAM_ON = wxNewId();
static const long CHAT_MENU_CH_SPAM_OFF = wxNewId();
static const long CHAT_MENU_CH_SPAM_ISON = wxNewId();
static const long CHAT_MENU_CH_CLEAR = wxNewId();

static const long CHAT_MENU_SV_DISCON = wxNewId();
static const long CHAT_MENU_SV_RECON = wxNewId();
static const long CHAT_MENU_SV_REMOVE = wxNewId();
static const long CHAT_MENU_SV_CHPWD = wxNewId();
static const long CHAT_MENU_SV_ACCESS = wxNewId();
static const long CHAT_MENU_SV_BROADCAST = wxNewId();

static const long CHAT_MENU_US_CHAT = wxNewId();
static const long CHAT_MENU_US_JOIN = wxNewId();
static const long CHAT_MENU_US_SLAP = wxNewId();
static const long CHAT_MENU_US_MUTE = wxNewId();
static const long CHAT_MENU_US_UNMUTE = wxNewId();
static const long CHAT_MENU_US_KICK = wxNewId();
static const long CHAT_MENU_US_OP = wxNewId();
static const long CHAT_MENU_US_DEOP = wxNewId();
static const long CHAT_MENU_US_MODERATOR_INGAME = wxNewId();
static const long CHAT_MENU_US_MODERATOR_CURIP = wxNewId();
static const long CHAT_MENU_US_MODERATOR_KICK = wxNewId();
static const long CHAT_MENU_US_MODERATOR_BAN = wxNewId();
static const long CHAT_MENU_US_MODERATOR_UNBAN = wxNewId();
static const long CHAT_MENU_US_MODERATOR_MUTE = wxNewId();
static const long CHAT_MENU_US_MODERATOR_MUTE_5 = wxNewId();
static const long CHAT_MENU_US_MODERATOR_MUTE_10 = wxNewId();
static const long CHAT_MENU_US_MODERATOR_MUTE_30 = wxNewId();
static const long CHAT_MENU_US_MODERATOR_MUTE_120 = wxNewId();
static const long CHAT_MENU_US_MODERATOR_MUTE_1440 = wxNewId();
static const long CHAT_MENU_US_MODERATOR_UNMUTE = wxNewId();
static const long CHAT_MENU_US_MODERATOR_RING = wxNewId();

static const long CHAT_MENU_COPYLINK = wxNewId();

static const long CHAT_MENU_LOG_OPEN = wxNewId();

#endif // SPRINGLOBBY_HEADERGUARD_CHATPANELMENU_H
