/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "chatpanelmenu.h"

#include <wx/log.h>

#include "chatpanel.h"
#include "channel.h"
#include "iserver.h"
#include "serverselector.h"
#include "user.h"
#include "nicklistctrl.h"
#include "mainwindow.h"
#include "settings.h"
#include "battlelist/battlelisttab.h"
#include "utils/conversion.h"

ChatPanelMenu::ChatPanelMenu(ChatPanel* parent, bool addChanServ, const wxString& /*title */, long /*style*/)
    : wxEvtHandler()
    , m_chatpanel(parent)
    , m_user_menu(0)
    , m_menu_all(0)
    , displayjoinitem(0)
    , m_autorejoin(0)
    , m_append_menu(0)
    , m_withChanserv(addChanServ)
{
}

//!ATTENTION: _all_ event ids must be handled in ChatPanelMenu::OnMenuItem
wxMenu* ChatPanelMenu::GetMenu()
{
	m_menu_all = new wxMenu();
	CreateNickListMenu();
	m_append_menu = new wxMenuItem(m_menu_all, CHAT_MENU_DISABLE_APPEND, _("Disable text appending"), wxEmptyString, wxITEM_CHECK);
	m_menu_all->Append(m_append_menu);
	m_append_menu->Check(m_chatpanel->m_disable_append);

	wxMenuItem* copy = new wxMenuItem(m_menu_all, wxID_COPY, _("Copy"), wxEmptyString, wxITEM_NORMAL);
	m_menu_all->Append(copy);

	if (m_chatpanel->m_url_at_pos != wxEmptyString) {
		wxMenuItem* copylink = new wxMenuItem(m_menu_all, CHAT_MENU_COPYLINK, _("Copy link location"), wxEmptyString, wxITEM_NORMAL);
		m_menu_all->Append(copylink);
	}

	wxMenuItem* clear = new wxMenuItem(m_menu_all, CHAT_MENU_CH_CLEAR, _("Clear"), wxEmptyString, wxITEM_NORMAL);
	m_menu_all->Append(clear);


	if (m_chatpanel->m_type == CPT_Channel) {
		wxLogMessage(_T( "channel" ));
		m_autorejoin = new wxMenuItem(m_menu_all, CHAT_MENU_CH_AUTOJOIN, _("Auto join this channel"), wxEmptyString, wxITEM_CHECK);
		m_menu_all->Append(m_autorejoin);
		const bool isautojoin = sett().GetChannelJoinIndex(TowxString(m_chatpanel->m_channel->GetName())) >= 0;
		m_autorejoin->Check(isautojoin);

		m_subscribe = new wxMenuItem(m_menu_all, CHAT_MENU_CH_SUBSCRIBE, _("Subscribe to this channel"), wxEmptyString, wxITEM_CHECK);
		m_menu_all->Append(m_subscribe);
		m_subscribe->Check(m_chatpanel->m_channel->IsSubscribed());


		wxMenuItem* leaveitem = new wxMenuItem(m_menu_all, CHAT_MENU_CH_LEAVE, _("Leave"), wxEmptyString, wxITEM_NORMAL);
		m_menu_all->Append(leaveitem);
	}

	displayjoinitem = new wxMenuItem(m_menu_all, CHAT_MENU_CH_DISPLAYJOIN, _("Display Join/Leave Messages"), wxEmptyString, wxITEM_CHECK);
	m_menu_all->Append(displayjoinitem);
	displayjoinitem->Check(m_chatpanel->m_display_joinitem);

	if (m_chatpanel->m_type == CPT_Channel) {
		wxMenuItem* mutelistitem = new wxMenuItem(m_menu_all, CHAT_MENU_SHOW_MUTELIST, _("Show mute list"), wxEmptyString, wxITEM_NORMAL);

		m_menu_all->Append(mutelistitem);

		if (m_withChanserv) {
			m_menu_all->AppendSeparator();
			wxMenu* m_chanserv;
			m_chanserv = new wxMenu();
			wxMenuItem* infoitem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_INFO, _("Channel info"), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(infoitem);

			m_chanserv->AppendSeparator();
			wxMenuItem* chtopicitem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_TOPIC, _("Set topic..."), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(chtopicitem);
			wxMenuItem* chmessageitem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_MSG, _("Channel message..."), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(chmessageitem);

			m_chanserv->AppendSeparator();
			wxMenuItem* chlockitem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_LOCK, _("Lock..."), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(chlockitem);
			wxMenuItem* chunlockitem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_UNLOCK, _("Unlock"), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(chunlockitem);

			m_chanserv->AppendSeparator();
			wxMenuItem* chregisteritem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_REG, _("Register..."), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(chregisteritem);
			wxMenuItem* chunregisteritem = new wxMenuItem(m_chanserv, CHAT_MENU_CH_UNREG, _("Unregister"), wxEmptyString, wxITEM_NORMAL);
			m_chanserv->Append(chunregisteritem);

			m_chanserv->AppendSeparator();
			wxMenu* m_spam;
			m_spam = new wxMenu();
			wxMenuItem* spamprotonitem = new wxMenuItem(m_spam, CHAT_MENU_CH_SPAM_ON, _("On"), wxEmptyString, wxITEM_NORMAL);
			m_spam->Append(spamprotonitem);
			wxMenuItem* spamprotoffitem = new wxMenuItem(m_spam, CHAT_MENU_CH_SPAM_OFF, _("Off"), wxEmptyString, wxITEM_NORMAL);
			m_spam->Append(spamprotoffitem);

			m_spam->AppendSeparator();
			wxMenuItem* spamprotisonitem = new wxMenuItem(m_spam, CHAT_MENU_CH_SPAM_ISON, _("Is on?"), wxEmptyString, wxITEM_NORMAL);
			m_spam->Append(spamprotisonitem);
			m_chanserv->Append(-1, _("Spam protection"), m_spam);
			m_menu_all->AppendSubMenu(m_chanserv, _("ChanServ"));
		}

	} else if (m_chatpanel->m_type == CPT_Server) {

		wxLogMessage(_T( "server" ));

		wxMenuItem* disconnectitem = new wxMenuItem(m_menu_all, CHAT_MENU_SV_DISCON, _("Disconnect"), wxEmptyString, wxITEM_NORMAL);
		m_menu_all->Append(disconnectitem);
		wxMenuItem* reconnectitem = new wxMenuItem(m_menu_all, CHAT_MENU_SV_RECON, _("Reconnect"), wxEmptyString, wxITEM_NORMAL);
		m_menu_all->Append(reconnectitem);

		m_menu_all->AppendSeparator();
		wxMenu* submenu_user;

		submenu_user = new wxMenu();
		wxMenu* m_accounts;
		m_accounts = new wxMenu();
		wxMenuItem* removeitem = new wxMenuItem(m_accounts, CHAT_MENU_SV_REMOVE, _("Remove..."), wxEmptyString, wxITEM_NORMAL);
		m_accounts->Append(removeitem);
		wxMenuItem* chpwditem = new wxMenuItem(m_accounts, CHAT_MENU_SV_CHPWD, _("Change password..."), wxEmptyString, wxITEM_NORMAL);
		m_accounts->Append(chpwditem);
		wxMenuItem* setaccessitem = new wxMenuItem(m_accounts, CHAT_MENU_SV_ACCESS, _("Set access..."), wxEmptyString, wxITEM_NORMAL);
		m_accounts->Append(setaccessitem);
		submenu_user->Append(-1, _("Accounts"), m_accounts);

		submenu_user->AppendSeparator();
		wxMenuItem* broadcastitem = new wxMenuItem(submenu_user, CHAT_MENU_SV_BROADCAST, _("Broadcast..."), wxEmptyString, wxITEM_NORMAL);
		submenu_user->Append(broadcastitem);
	} else if (m_chatpanel->m_type == CPT_User) {
		if (m_chatpanel->m_user)
			m_user_menu->EnableItems(true, TowxString(m_chatpanel->m_user->GetNick()));
		m_menu_all->AppendSubMenu(m_user_menu, _("User"));
	}

	if (m_chatpanel->m_chat_log.LogEnabled()) {
		wxMenuItem* open_extern = new wxMenuItem(m_menu_all, CHAT_MENU_LOG_OPEN, _("Open log in editor"), wxEmptyString, wxITEM_NORMAL);
		m_menu_all->Append(open_extern);
	}

	//    ConnectEvents();
	//    m_user_menu->Connect( GROUP_ID_NEW, wxEVT_COMMAND_MENU_SELECTED,
	//                                    wxCommandEventHandler( ChatPanelMenu::OnUserMenuCreateGroup ), 0, this );
	return m_menu_all;
}

void ChatPanelMenu::CreateNickListMenu()
{
	m_user_menu = new ChatPanelMenu::UserMenu(this, this);
	if (m_chatpanel->m_type != CPT_User) {
		wxMenuItem* chatitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_CHAT, _("Open Chat"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(chatitem);
	}
	wxMenuItem* joinbattleitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_JOIN, _("Join same battle"), wxEmptyString, wxITEM_NORMAL);
	m_user_menu->Append(joinbattleitem);

	m_user_menu->AppendSeparator();
	bool moderator = false;
	try {
		moderator = serverSelector().GetServer().GetMe().GetStatus().moderator;
	} catch (...) {
	}
	if (moderator) {
		wxMenuItem* modingameitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_MODERATOR_INGAME, _("Ingame time"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(modingameitem);
		wxMenuItem* modipitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_MODERATOR_CURIP, _("Retrieve IP and Smurfs"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(modipitem);

		m_user_menu->AppendSeparator();

		wxMenu* m_user_menu_mute;
		m_user_menu_mute = new wxMenu();
		wxMenuItem* modmuteitem = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE, _("Mute..."), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modmuteitem);
		wxMenuItem* modmute5item = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_5, _("Mute for 5 minutes"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modmute5item);
		wxMenuItem* modmute10item = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_10, _("Mute for 10 minutes"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modmute10item);
		wxMenuItem* modmute30item = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_30, _("Mute for 30 minutes"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modmute30item);
		wxMenuItem* modmute120item = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_120, _("Mute for 2 hours"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modmute120item);
		wxMenuItem* modmute1440item = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_1440, _("Mute for 1 day"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modmute1440item);
		m_user_menu_mute->AppendSeparator();
		wxMenuItem* modunmuteitem = new wxMenuItem(m_user_menu_mute, CHAT_MENU_US_MODERATOR_UNMUTE, _("Unmute"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu_mute->Append(modunmuteitem);
		m_user_menu->Append(-1, _("Mute"), m_user_menu_mute);

		wxMenuItem* modkickitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_MODERATOR_KICK, _("Kick..."), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(modkickitem);

		m_user_menu->AppendSeparator();
		wxMenuItem* modbanitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_MODERATOR_BAN, _("Ban..."), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(modbanitem);
		wxMenuItem* modunbanitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_MODERATOR_UNBAN, _("Unban"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(modunbanitem);

		m_user_menu->AppendSeparator();
		wxMenuItem* modringitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_MODERATOR_RING, _("Ring"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(modringitem);
		//m_user_menu->Append( -1, _("Moderator"), m_user_menu );
	} else {
		wxMenuItem* slapitem = new wxMenuItem(m_user_menu, CHAT_MENU_US_SLAP, _("Slap!"), wxEmptyString, wxITEM_NORMAL);
		m_user_menu->Append(slapitem);
	}

	if (m_chatpanel->m_type != CPT_User) {
		m_user_menu->AppendSeparator();
		wxMenu* m_chanserv;
		m_chanserv = new wxMenu();
		wxMenuItem* chmuteitem = new wxMenuItem(m_chanserv, CHAT_MENU_US_MUTE, _("Mute..."), wxEmptyString, wxITEM_NORMAL);
		m_chanserv->Append(chmuteitem);
		wxMenuItem* chunmuteitem = new wxMenuItem(m_chanserv, CHAT_MENU_US_UNMUTE, _("Unmute"), wxEmptyString, wxITEM_NORMAL);
		m_chanserv->Append(chunmuteitem);
		m_chanserv->AppendSeparator();
		wxMenuItem* chkickitem = new wxMenuItem(m_chanserv, CHAT_MENU_US_KICK, _("Kick..."), wxEmptyString, wxITEM_NORMAL);
		m_chanserv->Append(chkickitem);

		m_chanserv->AppendSeparator();
		wxMenuItem* chopitem = new wxMenuItem(m_chanserv, CHAT_MENU_US_OP, _("Op"), wxEmptyString, wxITEM_NORMAL);
		m_chanserv->Append(chopitem);
		wxMenuItem* chdeopitem = new wxMenuItem(m_chanserv, CHAT_MENU_US_DEOP, _("DeOp"), wxEmptyString, wxITEM_NORMAL);
		m_chanserv->Append(chdeopitem);
		m_user_menu->Append(-1, _("ChanServ"), m_chanserv);
	}
}

ChatPanelMenu::UserMenu* ChatPanelMenu::GetUserMenu()
{
	CreateNickListMenu();
	return m_user_menu;
}

ChatPanelMenu::UserMenu* ChatPanelMenu::GetUserMenuNoCreate()
{
	return m_user_menu;
}

void ChatPanelMenu::OnUserMenuCopyLink(wxCommandEvent& /*unused*/)
{
	CopyToClipboard(m_chatpanel->m_url_at_pos);
	m_chatpanel->m_url_at_pos = wxEmptyString;
}


//////////////////////////////////////////////////////////////////////////////////////
// Menu Events
//////////////////////////////////////////////////////////////////////////////////////


void ChatPanelMenu::OnChannelMenuLeave(wxCommandEvent& /*unused*/)
{
	if (m_chatpanel->m_channel == 0)
		return;
	m_chatpanel->Part();
	m_chatpanel->SetChannel(0);
}


void ChatPanelMenu::OnChannelMenuDisplayJoinLeave(wxCommandEvent& /*unused*/)
{
	m_chatpanel->m_display_joinitem = displayjoinitem->IsChecked();

	// not needed, menu is re-created each time it is shown
	//displayjoinitem->Check(!checked);
}


void ChatPanelMenu::OnChannelAutoJoin(wxCommandEvent& /*unused*/)
{
	if (m_chatpanel->m_channel == 0)
		return;
	if (m_autorejoin == 0)
		return;

	if (m_autorejoin->IsChecked()) {
		sett().AddChannelJoin(TowxString(m_chatpanel->m_channel->GetName()), TowxString(m_chatpanel->m_channel->GetPassword()));
		m_autorejoin->Check(true);
	} else {
		sett().RemoveChannelJoin(TowxString(m_chatpanel->m_channel->GetName()));
		m_autorejoin->Check(false);
	}
}

bool ChatPanelMenu::HasChanserv()
{
	if (m_chatpanel->m_channel == 0)
		return false;
	if (!m_chatpanel->m_channel->UserExists("ChanServ")) {
		ui().ShowMessage(_("ChanServ error"), _("ChanServ is not in this channel."));
		return false;
	}
	return true;
}

void ChatPanelMenu::SayChanserv(const std::string& msg)
{
	assert(HasChanserv());
	const User& cs = m_chatpanel->m_channel->GetUser("ChanServ");
	cs.Say(msg);
}

void ChatPanelMenu::OnChannelMenuInfo(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;
	SayChanserv("!INFO #" + m_chatpanel->m_channel->GetName());
}


void ChatPanelMenu::OnChannelMenuTopic(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	wxString topic = TowxString(m_chatpanel->m_channel->GetTopic());
	if (!ui().AskText(_("Set topic..."), _("What should be the new topic?"), topic, true))
		return;
	topic.Replace(_T("\n"), _T("\\n"));
	SayChanserv("!TOPIC #" + m_chatpanel->m_channel->GetName() + " " + STD_STRING(topic));
	//TOPIC /<channame>/ {topic}
}


void ChatPanelMenu::OnChannelMenuMessage(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;
	wxString text;
	if (!ui().AskText(_("Channel message..."), _("Message:"), text))
		return;

	SayChanserv("!CHANMSG #" + m_chatpanel->m_channel->GetName() + " " + STD_STRING(text));
}


void ChatPanelMenu::OnChannelMenuLock(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	wxString text;
	if (!ui().AskText(_("Lock channel..."), _("What should the new password be?"), text))
		return;

	SayChanserv("!LOCK #" + m_chatpanel->m_channel->GetName() + " " + STD_STRING(text));
}


void ChatPanelMenu::OnChannelMenuUnlock(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	if (!ui().Ask(_("Unlock Channel"), _("Are you sure you want to unlock this channel?")))
		return;

	SayChanserv("!UNLOCK #" + m_chatpanel->m_channel->GetName());
}


void ChatPanelMenu::OnChannelMenuRegister(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	wxString text = TowxString(m_chatpanel->m_channel->GetMe().GetNick());
	if (!ui().AskText(_("Register Channel"), _("Who should be appointed founder of this channel?"), text))
		return;

	SayChanserv("!REGISTER #" + m_chatpanel->m_channel->GetName() + " " + STD_STRING(text));
}


void ChatPanelMenu::OnChannelMenuUnregister(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	if (!ui().Ask(_("Unregister Channel"), _("Are you sure you want to unregister this channel?")))
		return;

	SayChanserv("!UNREGISTER #" + m_chatpanel->m_channel->GetName());
}


void ChatPanelMenu::OnChannelMenuSpamOn(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;
	SayChanserv("!SPAMPROTECTION #" + m_chatpanel->m_channel->GetName() + " on");
}


void ChatPanelMenu::OnChannelMenuSpanOff(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;
	SayChanserv("!SPAMPROTECTION #" + m_chatpanel->m_channel->GetName() + " off");
}


void ChatPanelMenu::OnChannelMenuSpamIsOn(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;
	SayChanserv("!SPAMPROTECTION #" + m_chatpanel->m_channel->GetName());
}


void ChatPanelMenu::OnServerMenuDisconnect(wxCommandEvent& /*unused*/)
{
	ui().Disconnect();
}


void ChatPanelMenu::OnServerMenuReconnect(wxCommandEvent& /*unused*/)
{
	ui().Reconnect();
}


void ChatPanelMenu::OnServerMenuRemove(wxCommandEvent& /*unused*/)
{
	wxString user;
	if (!ui().AskText(_("Remove User Acount"), _("What user account do you want to remove today?"), user))
		return;
	if (!ui().Ask(_("Remove Account"), _("Are you sure you want to remove the account ") + user + _T( "?" )))
		return;
	m_chatpanel->Say(_T( "removeaccount " ) + user);
}


void ChatPanelMenu::OnServerMenuChangePassword(wxCommandEvent& /*unused*/)
{
	wxString user, password;
	if (!ui().AskText(_("Change User Acount Password"), _("What user account do you want to change the password for?"), user))
		return;
	if (!ui().AskPassword(_("Change User Acount Password"), _("What would be the new password?"), password))
		return;
	m_chatpanel->Say(_T( "changeaccountpass " ) + user + _T( " " ) + password);
}


void ChatPanelMenu::OnServerMenuSetAccess(wxCommandEvent& /*unused*/)
{
	ui().ShowMessage(_("Error"), _("Not Implemented"));
}


void ChatPanelMenu::OnServerMenuBroadcast(wxCommandEvent& /*unused*/)
{
	wxString msg;
	if (!ui().AskText(_("Broadcast Message"), _("Message to be broadcasted:"), msg))
		return;
	m_chatpanel->Say(_T( "broadcast " ) + msg);
}


void ChatPanelMenu::OnUserMenuOpenChat(wxCommandEvent& /*unused*/)
{
	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	ui().mw().OpenPrivateChat(*user);
}


void ChatPanelMenu::OnUserMenuJoinSame(wxCommandEvent& /*unused*/)
{
	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;
	IBattle* battle = user->GetBattle();
	if (battle == 0)
		return;

	ui().mw().GetBattleListTab().DoJoin(*battle);
}


void ChatPanelMenu::OnUserMenuSlap(wxCommandEvent& /*unused*/)
{
	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	if (m_chatpanel->m_type == CPT_Channel) {
		if (m_chatpanel->m_channel == 0)
			return;
		m_chatpanel->m_channel->DoAction("Slaps " + user->GetNick() + " around with a large PeeWee!");
	} else if (m_chatpanel->m_type == CPT_User) {
		if (m_chatpanel->m_user == 0)
			return;
		m_chatpanel->m_user->DoAction("slaps " + user->GetNick() + " around with a large PeeWee!");
	}
}


void ChatPanelMenu::OnUserMenuMute(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	wxString mutetime = _T( "5" );
	if (!ui().AskText(_("Mute User"), _("For how many minutes should the user be muted?"), mutetime))
		return;

	SayChanserv("!MUTE #" + m_chatpanel->m_channel->GetName() + " " + user->GetNick() + " " + STD_STRING(mutetime));
	//MUTE /<channame>/ <username> [<duration>]
}


void ChatPanelMenu::OnUserMenuUnmute(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	SayChanserv("!UNMUTE #" + m_chatpanel->m_channel->GetName() + " " + user->GetNick());
	//UNMUTE /<channame>/ <username>
}


void ChatPanelMenu::OnUserMenuKick(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	wxString msg;
	if (!ui().AskText(_("Kick User"), _("Reason:"), msg))
		return;

	if (msg != wxEmptyString)
		msg = _T( " " ) + msg;
	SayChanserv("!KICK #" + m_chatpanel->m_channel->GetName() + " " + user->GetNick() + STD_STRING(msg));
	//KICK /<channame>/ <username> [{reason}]
}


void ChatPanelMenu::OnUserMenuOp(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	SayChanserv("!OP #" + m_chatpanel->m_channel->GetName() + " " + user->GetNick());
	//OP /<channame>/ <username>
}


void ChatPanelMenu::OnUserMenuDeop(wxCommandEvent& /*unused*/)
{
	if (!HasChanserv())
		return;

	const User* user = m_chatpanel->GetSelectedUser();
	if (user == 0)
		return;

	SayChanserv("!DEOP #" + m_chatpanel->m_channel->GetName() + " " + user->GetNick());
	//DEOP /<channame>/ <username>
}


void ChatPanelMenu::OnUserMenuModeratorIngame(wxCommandEvent& /*unused*/)
{
	serverSelector().GetServer().RequestInGameTime(m_chatpanel->GetSelectedUser()->GetNick());
}


void ChatPanelMenu::OnUserMenuModeratorCurrentIP(wxCommandEvent& /*unused*/)
{
	serverSelector().GetServer().ModeratorGetIP(m_chatpanel->GetSelectedUser()->GetNick());
}


void ChatPanelMenu::OnUserMenuModeratorKick(wxCommandEvent& /*unused*/)
{
	wxString reason;
	if (!ui().AskText(_("Kick user"), _("Reason:"), reason))
		return;
	serverSelector().GetServer().ModeratorKick(m_chatpanel->GetSelectedUser()->GetNick(), STD_STRING(reason));
}


void ChatPanelMenu::OnUserMenuModeratorBan(wxCommandEvent& /*unused*/)
{
	ui().ShowMessage(_("Error"), _("Not Implemented"));
}


void ChatPanelMenu::OnUserMenuModeratorUnban(wxCommandEvent& /*unused*/)
{
	ui().ShowMessage(_("Error"), _("Not Implemented"));
}


void ChatPanelMenu::MutePlayer(long dur)
{
	serverSelector().GetServer().ModeratorMute(m_chatpanel->m_channel->GetName(), m_chatpanel->GetSelectedUser()->GetNick(), (int)dur, true);
}

void ChatPanelMenu::OnUserMenuModeratorMute(wxCommandEvent& /*unused*/)
{
	wxString duration;
	if (!ui().AskText(_("Mute user"), _("Duration:"), duration))
		return;
	long int dur = 0;
	duration.ToLong(&dur, dur);
	MutePlayer(dur);
}


void ChatPanelMenu::OnUserMenuModeratorMute5(wxCommandEvent& /*unused*/)
{
	MutePlayer(5);
}


void ChatPanelMenu::OnUserMenuModeratorMute10(wxCommandEvent& /*unused*/)
{
	MutePlayer(10);
}


void ChatPanelMenu::OnUserMenuModeratorMute30(wxCommandEvent& /*unused*/)
{
	MutePlayer(30);
}


void ChatPanelMenu::OnUserMenuModeratorMute120(wxCommandEvent& /*unused*/)
{
	MutePlayer(120);
}


void ChatPanelMenu::OnUserMenuModeratorMute1440(wxCommandEvent& /*unused*/)
{
	MutePlayer(1440);
}


void ChatPanelMenu::OnUserMenuModeratorUnmute(wxCommandEvent& /*unused*/)
{
	serverSelector().GetServer().ModeratorUnmute(m_chatpanel->m_channel->GetName(), m_chatpanel->GetSelectedUser()->GetNick());
}


void ChatPanelMenu::OnUserMenuModeratorRing(wxCommandEvent& /*unused*/)
{
	serverSelector().GetServer().Ring(m_chatpanel->GetSelectedUser()->GetNick());
}

void ChatPanelMenu::OnMenuToggleAppend(wxCommandEvent& /*unused*/)
{
	m_chatpanel->m_disable_append = m_append_menu->IsChecked();
	if (!m_chatpanel->m_disable_append) {
		for (std::vector<ChatLine>::const_iterator iter = m_chatpanel->m_buffer.begin(); iter < m_chatpanel->m_buffer.end(); ++iter)
			m_chatpanel->OutputLine(*iter);
		m_chatpanel->m_buffer.clear();
	}
}

void ChatPanelMenu::OnChannelMenuShowMutelist(wxCommandEvent& /*unused*/)
{
	if (m_chatpanel->m_channel && (m_chatpanel->m_type == CPT_Channel)) {
		m_chatpanel->m_channel->GetServer().SendCmd("MUTELIST", m_chatpanel->m_channel->GetName());
	}
}

void ChatPanelMenu::OnChatMenuOpenLog(wxCommandEvent&)
{
	ui().OpenFileInEditor(m_chatpanel->m_chat_log.GetCurrentLogfilePath());
}

void ChatPanelMenu::OnChannelClearContents(wxCommandEvent& /*unused*/)
{
	m_chatpanel->m_chatlog_text->SetValue(wxEmptyString);
}

void ChatPanelMenu::OnUserMenuAddToGroup(wxCommandEvent& event)
{
	int id = event.GetId();
	if (m_user_menu) {
		wxString groupname = m_user_menu->GetGroupByEvtID(id);
		const User* user = m_chatpanel->GetSelectedUser();
		if (user)
			useractions().AddUserToGroup(groupname, TowxString(user->GetNick()));
	}
}

void ChatPanelMenu::OnUserMenuDeleteFromGroup(wxCommandEvent& /*unused*/)
{
	const User* user = m_chatpanel->GetSelectedUser();
	if (user)
		useractions().RemoveUser(TowxString(user->GetNick()));
}

void ChatPanelMenu::OnUserMenuCreateGroup(wxCommandEvent& /*unused*/)
{
	wxString name;
	if (ui().AskText(_("Enter name"),
			 _("Please enter the name for the new group.\nAfter clicking ok you will be taken to adjust its settings."), name)) {
		const User* user = m_chatpanel->GetSelectedUser();
		if (user) {
			useractions().AddGroup(name);
			useractions().AddUserToGroup(name, TowxString(user->GetNick()));
			ui().mw().ShowConfigure(MainWindow::OPT_PAGE_GROUPS);
		} else
			customMessageBoxNoModal(SL_MAIN_ICON, _("couldn't add user"), _("Error"));
	}
}

void ChatPanelMenu::OnChannelSubscribe(wxCommandEvent& /*unused*/)
{
	const std::string chan = m_chatpanel->m_channel->GetName();
	IServer& serv = m_chatpanel->m_channel->GetServer();
	if (!m_chatpanel->m_channel->IsSubscribed()) {
		serv.SendCmd("SUBSCRIBE", "chanName=" + chan);
	} else {
		serv.SendCmd("UNSUBSCRIBE", "chanName=" + chan);
	}
}

void ChatPanelMenu::OnMenuItem(wxCommandEvent& event)
{
	if (event.GetId() == CHAT_MENU_SHOW_MUTELIST)
		OnChannelMenuShowMutelist(event);
	else if (event.GetId() == CHAT_MENU_DISABLE_APPEND)
		OnMenuToggleAppend(event);

	else if (event.GetId() == CHAT_MENU_CH_LEAVE)
		OnChannelMenuLeave(event);
	else if (event.GetId() == CHAT_MENU_CH_DISPLAYJOIN)
		OnChannelMenuDisplayJoinLeave(event);
	else if (event.GetId() == CHAT_MENU_CH_AUTOJOIN)
		OnChannelAutoJoin(event);

	else if (event.GetId() == CHAT_MENU_CH_CLEAR)
		OnChannelClearContents(event);

	else if (event.GetId() == CHAT_MENU_SV_DISCON)
		OnServerMenuDisconnect(event);
	else if (event.GetId() == CHAT_MENU_SV_RECON)
		OnServerMenuReconnect(event);
	else if (event.GetId() == CHAT_MENU_SV_REMOVE)
		OnServerMenuRemove(event);
	else if (event.GetId() == CHAT_MENU_SV_CHPWD)
		OnServerMenuChangePassword(event);
	else if (event.GetId() == CHAT_MENU_SV_ACCESS)
		OnServerMenuSetAccess(event);
	else if (event.GetId() == CHAT_MENU_SV_BROADCAST)
		OnServerMenuBroadcast(event);

	else if (event.GetId() == CHAT_MENU_US_CHAT)
		OnUserMenuOpenChat(event);
	else if (event.GetId() == CHAT_MENU_US_JOIN)
		OnUserMenuJoinSame(event);
	else if (event.GetId() == CHAT_MENU_US_SLAP)
		OnUserMenuSlap(event);

	else if (event.GetId() == CHAT_MENU_US_MUTE)
		OnUserMenuMute(event);
	else if (event.GetId() == CHAT_MENU_US_UNMUTE)
		OnUserMenuUnmute(event);
	else if (event.GetId() == CHAT_MENU_US_KICK)
		OnUserMenuKick(event);
	else if (event.GetId() == CHAT_MENU_US_OP)
		OnUserMenuOp(event);
	else if (event.GetId() == CHAT_MENU_US_DEOP)
		OnUserMenuDeop(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_INGAME)
		OnUserMenuModeratorIngame(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_CURIP)
		OnUserMenuModeratorCurrentIP(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_KICK)
		OnUserMenuModeratorKick(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_BAN)
		OnUserMenuModeratorBan(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_UNBAN)
		OnUserMenuModeratorUnban(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_MUTE)
		OnUserMenuModeratorMute(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_MUTE_5)
		OnUserMenuModeratorMute5(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_MUTE_10)
		OnUserMenuModeratorMute10(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_MUTE_30)
		OnUserMenuModeratorMute30(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_MUTE_120)
		OnUserMenuModeratorMute120(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_MUTE_1440)
		OnUserMenuModeratorMute1440(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_UNMUTE)
		OnUserMenuModeratorUnmute(event);
	else if (event.GetId() == CHAT_MENU_US_MODERATOR_RING)
		OnUserMenuModeratorRing(event);

	else if (event.GetId() == CHAT_MENU_COPYLINK)
		OnUserMenuCopyLink(event);

	else if (event.GetId() == CHAT_MENU_LOG_OPEN)
		OnChatMenuOpenLog(event);


	else if (event.GetId() == CHAT_MENU_CH_INFO)
		OnChannelMenuInfo(event);
	else if (event.GetId() == CHAT_MENU_CH_LOCK)
		OnChannelMenuLock(event);
	else if (event.GetId() == CHAT_MENU_CH_UNLOCK)
		OnChannelMenuUnlock(event);
	else if (event.GetId() == CHAT_MENU_CH_REG)
		OnChannelMenuRegister(event);
	else if (event.GetId() == CHAT_MENU_CH_UNREG)
		OnChannelMenuUnregister(event);
	else if (event.GetId() == CHAT_MENU_CH_SPAM_ON)
		OnChannelMenuSpamOn(event);
	else if (event.GetId() == CHAT_MENU_CH_SPAM_OFF)
		OnChannelMenuSpanOff(event);
	else if (event.GetId() == CHAT_MENU_CH_SPAM_ISON)
		OnChannelMenuSpamIsOn(event);
	else if (event.GetId() == CHAT_MENU_CH_TOPIC)
		OnChannelMenuTopic(event);
	else if (event.GetId() == CHAT_MENU_CH_MSG)
		OnChannelMenuMessage(event);

	else if (event.GetId() == GROUP_ID_NEW)
		OnUserMenuCreateGroup(event);
	else if (event.GetId() == GROUP_ID_REMOVE)
		OnUserMenuDeleteFromGroup(event);
	else if (event.GetId() == wxID_COPY)
		m_chatpanel->m_chatlog_text->OnCopy(event);
	else if (event.GetId() == CHAT_MENU_CH_SUBSCRIBE)
		OnChannelSubscribe(event);

	else
		OnUserMenuAddToGroup(event);
}
