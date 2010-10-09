#ifndef USERACTIONS_HH_INCLUDED
#define USERACTIONS_HH_INCLUDED

#include <wx/intl.h>
#include <wx/arrstr.h>
#include <map>

class wxColour;

//!provide a simple mapping between enum type and string to display in gui
const wxString m_actionNames[] = { _("none"),_("highlight"),_("notify login/out"),_("ignore chat"),_("ignore pm"),
    _("autokick"), _("notify hosted battle"),_("notify status change")};

//! Provide the names to be used by config file.
const wxString m_configActionNames[] = { _T("none"),_T("highlight"),_T("notify_login"),_T("ignore_chat"),_T("ignore_pm"),
    _T("autokick"), _T("notify_hosted"),_T("notify_status")};

//!same for tooltips
const wxString m_actionTooltips[] = { _("no action at all"), _("highlight user in nick list and battles he participates in"),
	_("popup a message box when user logs in/out from  the server"), _("you won't see message by these users in normal channels"),
    _("ignore private messages of these users, no pm window will open if any of these try to contact you privately"),
	_("automatically kick users from battles hosted by yourself"), _("popup a message box when user hosts a new battle"),
    _("popup a message box when user changes away status") };


//! data handling for group / action management
/** one single static instance is exposed as a global \n
    by forcing a write to settings handler on every change data consistency is ensured \n
    to keep runtime overhead as small as possible for the often called query funcs, all data is structured in multiple
    maps and wherever possible sortedArrays (binary search instead of linear!) are used \n
    the price is that on every change operation (very rare compared to queries) maps need to be cleared/reloaded \n
    currently Gui updates are handled old fashoined way by hangling around classes, this should be improved to dynamic events

**/
class UserActions {

public:
    UserActions();
    ~UserActions();

     enum ActionType {
       ActNone = 1,
       ActHighlight = 2,
       ActNotifLogin = 4,
       ActIgnoreChat = 8,
       ActIgnorePM = 16,
       ActAutokick = 32,
       ActNotifBattle = 64,
       ActNotifStatus = 128,
       /// update this when adding new actions.
       ActLast=ActNotifStatus
     };
    static const int m_numActions = sizeof(m_actionNames) / sizeof(wxString);
    bool DoActionOnUser( const ActionType action, const wxString& name ) ;
    wxArrayString GetGroupNames() const;
    void AddUserToGroup( const wxString& group, const wxString& name );
    void AddGroup(const wxString& name );
    void DeleteGroup(const wxString& name );
    void RemoveUser(const wxString& name );
    void ChangeAction( const wxString& group, const ActionType action, bool add = true );
    ActionType GetGroupAction( const wxString& group ) const;
    wxString GetGroupOfUser( const wxString& user ) const;
    void SetGroupColor( const wxString& group, const wxColour& color );
    wxColour GetGroupColor( const wxString& group );
    bool IsKnown( const wxString& name, bool outputWarning = false ) const;

protected:
    //lotsa maps to keep runtime finds, etc ti a minimum
    typedef std::map<wxString,wxArrayString> GroupMap;
    /// groupname --> array of people in the group
    GroupMap m_groupMap;
    typedef std::map<wxString,ActionType> GroupActionMap;
    /// groupname --> ActionType for that group
    GroupActionMap m_groupActions;
    typedef std::map<ActionType,wxArrayString> ActionGroupsMap;
    /// ActionType --> array of groups with that actiontype
    ActionGroupsMap m_actionsGroups;
    typedef std::map<ActionType,wxArrayString> ActionPeopleMap;
    /// ActionType --> array of people with that actiontype
    ActionPeopleMap m_actionsPeople;
    ///nickname --> group map (we don't allow users to be in more than one group
    typedef std::map<wxString,wxString> PeopleGroupMap;
    PeopleGroupMap m_peopleGroup;
    ///list all known users in groups
    wxArrayString m_knownUsers;

    //reload all maps and stuff
    void Init();
    void UpdateUI();

    wxArrayString m_groupNames;
};

UserActions& useractions();
#endif // USERACTIONS_HH_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

