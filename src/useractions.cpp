/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "useractions.h"

#include <cmath>
#include <wx/intl.h>
#include <wx/colour.h>
#include <wx/log.h>

#include <lslutils/globalsmanager.h>

#include "settings.h"
#include "utils/slconfig.h"
#include "utils/conversion.h"
#include "gui/customdialogs.h"
#include "gui/mainwindow.h"
#include "gui/mainchattab.h"
#include "gui/hosting/mainjoinbattletab.h"
#include "gui/hosting/ibattleroomtab.h"
#include "gui/battlelist/battlelisttab.h"
#include "gui/ui.h"

const wxColour defaultHLcolor(255, 0, 0);

UserActions& useractions()
{
	static LSL::Util::LineInfo<UserActions> m(AT);
	static LSL::Util::GlobalObjectHolder<UserActions, LSL::Util::LineInfo<UserActions> > m_useractions(m);
	return m_useractions;
}

UserActions::UserActions()
{
	Init();
}

UserActions::~UserActions()
{
}

bool UserActions::DoActionOnUser(const UserActions::ActionType action, const wxString& name)
{
	// preventing action on oneself wasn't the best idea, login gets disabled
	//if ( m_knownUsers.Index( name ) == -1 || ui().IsThisMe(name) || action == ActNone )

	if (m_knownUsers.Index(name) == -1 || action == ActNone)
		return false;
	else
		return (m_actionsGroups.find(action) != m_actionsGroups.end() && m_actionsPeople[action].Index(name) != -1);
}

void UserActions::Init()
{
	m_actionNames.clear();
	m_actionNames.push_back(_("none"));
	m_actionNames.push_back(_("highlight"));
	m_actionNames.push_back(_("notify login/out"));
	m_actionNames.push_back(_("ignore chat"));
	m_actionNames.push_back(_("ignore pm"));
	m_actionNames.push_back(_("autokick"));
	m_actionNames.push_back(_("notify hosted battle"));
	m_actionNames.push_back(_("notify status change"));

	m_configActionNames.clear();
	m_configActionNames.push_back(_T("none"));
	m_configActionNames.push_back(_T("highlight"));
	m_configActionNames.push_back(_T("notify_login"));
	m_configActionNames.push_back(_T("ignore_chat"));
	m_configActionNames.push_back(_T("ignore_pm"));
	m_configActionNames.push_back(_T("autokick"));
	m_configActionNames.push_back(_T("notify_hosted"));
	m_configActionNames.push_back(_T("notify_status"));

	m_actionTooltips.clear();
	m_actionTooltips.push_back(_("no action at all"));
	m_actionTooltips.push_back(_("highlight user in nick list and battles he participates in"));
	m_actionTooltips.push_back(_("popup a message box when user logs in/out from  the server"));
	m_actionTooltips.push_back(_("you won't see message by these users in normal channels"));
	m_actionTooltips.push_back(_("ignore private messages of these users, no pm window will open if any of these try to contact you privately"));
	m_actionTooltips.push_back(_("automatically kick users from battles hosted by yourself"));
	m_actionTooltips.push_back(_("popup a message box when user hosts a new battle"));
	m_actionTooltips.push_back(_("popup a message box when user changes away status"));

	// setup if empty
	if (!cfg().Exists(_T( "/Groups"))) {
		AddGroup(_("Default"));
		AddGroup(_("Ignore PM"));
		ChangeAction(_("Ignore PM"), UserActions::ActIgnorePM);
		AddGroup(_("Ignore chat"));
		ChangeAction(_("Ignore chat"), UserActions::ActIgnoreChat);
		AddGroup(_("Battle Autokick"));
		ChangeAction(_("Battle Autokick"), UserActions::ActAutokick);
		AddGroup(_("Friends"));
		ChangeAction(_("Friends"), UserActions::ActNotifBattle);
		ChangeAction(_("Friends"), UserActions::ActHighlight);
		ChangeAction(_("Friends"), UserActions::ActNotifLogin);
		// TODO select better color
		SetGroupColor(_("Friends"), wxColour(0, 0, 255));
	}


	// read
	m_groupNames = GetGroups();
	m_groupMap.clear();
	m_groupActions.clear();
	m_actionsGroups.clear();
	m_actionsPeople.clear();
	m_knownUsers.Clear();
	for (unsigned int i = 0; i < m_groupNames.GetCount(); ++i) {
		wxString name = m_groupNames[i];
		m_groupMap[name] = GetPeopleList(name);
		for (unsigned int k = 0; k < m_groupMap[name].GetCount(); ++k) {
			wxString user = m_groupMap[name][k];
			m_knownUsers.Add(user);
			m_peopleGroup[user] = name;
		}
		m_groupActions[name] = GetGroupActions(name);
	}
	for (size_t i = 0; i < m_actionNames.size(); ++i) {
		UserActions::ActionType cur = (UserActions::ActionType)(1 << i);
		wxArrayString tmp;
		for (unsigned int j = 0; j < m_groupNames.GetCount(); ++j) {
			wxString name = m_groupNames[j];
			if ((m_groupActions[name] & cur) != 0) {
				tmp.Add(name);
				for (unsigned int k = 0; k < m_groupMap[name].GetCount(); ++k) {
					m_actionsPeople[cur].Add((m_groupMap[name])[k]);
				}
			}
		}
		tmp.Sort();
		m_actionsGroups[cur] = tmp;
	}
	m_actionsGroups[ActNone] = m_groupNames;
	m_groupNames.Sort();
	m_knownUsers.Sort();
}

void UserActions::UpdateUI()
{
	try {
		ui().mw().GetBattleListTab().UpdateHighlights();
	} catch (...) {
	}

	try {
		ui().mw().GetChatTab().UpdateNicklistHighlights();
	} catch (...) {
	}

	try {
		ui().mw().GetJoinTab().GetBattleRoomTab().UpdateHighlights();
	} catch (...) {
	}
}

wxArrayString UserActions::GetGroupNames() const
{
	return m_groupNames;
}

void UserActions::AddUserToGroup(const wxString& group, const wxString& name)
{
	if (IsKnown(name, false) || ui().IsThisMe(name))
		return;
	m_groupMap[group].Add(name);
	SetPeopleList(m_groupMap[group], group);
	Init();
	UpdateUI();
}

void UserActions::DeleteGroup(const wxString& group)
{
	if (cfg().Exists(_T( "/Groups/" ) + group)) {
		cfg().DeleteGroup(_T( "/Groups/" ) + group);
	}
	Init();
	UpdateUI();
}

wxArrayString UserActions::GetPeopleList(const wxString& group) const
{
	wxArrayString list;
	slConfig::PathGuard pathGuard(&cfg(), _T( "/Groups/" ) + group + _T( "/Members/" ));
	unsigned int friendsCount = cfg().GetNumberOfEntries(false);
	for (unsigned int i = 0; i < friendsCount; i++) {
		wxString ToAdd;
		if (cfg().Read(_T( "/Groups/" ) + group + _T( "/Members/" ) + TowxString(i), &ToAdd))
			list.Add(ToAdd);
	}
	return list;
}


void UserActions::AddGroup(const wxString& group)
{
	if (!cfg().Exists(_T( "/Groups/" ) + group)) {
		//set defaults
		SetGroupActions(group, UserActions::ActNone);
		SetGroupHLColor(defaultHLcolor, group);
	}
	Init();
	UpdateUI();
}


void UserActions::ChangeAction(const wxString& group, const ActionType action, bool add)
{
	ActionType old = m_groupActions[group];
	old = (ActionType)(add ? (old | action) : (old & ~action));
	SetGroupActions(group, old);
	Init();
	UpdateUI();
}

void UserActions::SetGroupActions(const wxString& group, ActionType action) const
{
	wxString key = _T( "/Groups/" ) + group + _T( "/Opts/ActionsList" );
	cfg().DeleteGroup(key);
	key += _T( "/" );
	unsigned int tmp = action & ((UserActions::ActLast << 1) - 1);
	for (auto config : m_configActionNames) {
		if (tmp & 1)
			cfg().Write(key + config, true);
		tmp >>= 1;
	}
}

UserActions::ActionType UserActions::GetGroupActions(const wxString& group) const
{
	wxString key = _T( "/Groups/" ) + group + _T( "/Opts/Actions" );
	if (cfg().HasEntry(key)) // Backward compatibility.
	{
		wxLogMessage(_T( "loading deprecated group actions and updating config" ));
		UserActions::ActionType action = (UserActions::ActionType)cfg().Read(key, (long)UserActions::ActNone);
		cfg().DeleteEntry(key);

		SetGroupActions(group, action);

		return action;
	}
	key = _T( "/Groups/" ) + group + _T( "/Opts/ActionsList" );
	if (!cfg().Exists(key))
		return UserActions::ActNone;
	key += _T( "/" );
	int mask = 1;
	int result = 0;
	for (auto config : m_configActionNames) {
		if (cfg().Read(key + config, 0l)) {
			result |= mask;
		}
		mask <<= 1;
	}
	if (result == 0)
		return UserActions::ActNone;
	return (UserActions::ActionType)result;
}

UserActions::ActionType UserActions::GetGroupAction(const wxString& group) const
{
	const GroupActionMap::const_iterator res = m_groupActions.find(group);
	return res->second;
}

wxString UserActions::GetGroupOfUser(const wxString& user) const
{
	const PeopleGroupMap::const_iterator res = m_peopleGroup.find(user);
	if (res == m_peopleGroup.end()) {
		return wxEmptyString;
	} else {
		return res->second;
	}
}

void UserActions::SetGroupColor(const wxString& group, const wxColour& color)
{
	SetGroupHLColor(color, group);
	Init();
	UpdateUI();
}

wxColour UserActions::GetGroupHLColor(const wxString& group) const
{
	return wxColour(cfg().Read(_T( "/Groups/" ) + group + _T( "/Opts/HLColor" ), _T( "#64648C" )));
}

void UserActions::SetGroupHLColor(const wxColour& color, const wxString& group)
{
	cfg().Write(_T( "/Groups/" ) + group + _T( "/Opts/HLColor" ), color.GetAsString(wxC2S_HTML_SYNTAX));
}

wxArrayString UserActions::GetGroups()
{
	return cfg().GetGroupList(_T( "/Groups/" ));
}


wxColour UserActions::GetGroupColor(const wxString& group) const
{
	return GetGroupHLColor(group);
}

bool UserActions::IsKnown(const wxString& name, bool outputWarning) const
{
	bool ret = m_knownUsers.Index(name) != -1;
	if (outputWarning) {
		customMessageBoxModal(SL_MAIN_ICON, _("To prevent logical inconsistencies, adding a user to more than one group is not allowed"),
				      _("Cannot add user to group"));
	}

	return ret;
}

void UserActions::RemoveUser(const wxString& name)
{
	wxString group = m_peopleGroup[name];
	m_groupMap[group].Remove(name);
	SetPeopleList(m_groupMap[group], group);
	Init();
	UpdateUI();
}

void UserActions::SetPeopleList(const wxArrayString& friends, const wxString& group)
{
	unsigned int friendsCount = friends.GetCount();
	cfg().DeleteGroup(_T( "/Groups/" ) + group + _T( "/Members/" ));
	for (unsigned int i = 0; i < friendsCount; i++) {
		cfg().Write(_T( "/Groups/" ) + group + _T( "/Members/" ) + TowxString(i), friends[i]);
	}
}
