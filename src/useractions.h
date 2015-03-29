/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef USERACTIONS_HH_INCLUDED
#define USERACTIONS_HH_INCLUDED

#include <wx/arrstr.h>
#include <map>
#include <list>

class wxColour;


//! data handling for group / action management
/** one single static instance is exposed as a global \n
    by forcing a write to settings handler on every change data consistency is ensured \n
    to keep runtime overhead as small as possible for the often called query funcs, all data is structured in multiple
    maps and wherever possible sortedArrays (binary search instead of linear!) are used \n
    the price is that on every change operation (very rare compared to queries) maps need to be cleared/reloaded \n
    currently Gui updates are handled old fashoined way by hangling around classes, this should be improved to dynamic events

**/
class UserActions
{

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
		ActLast = ActNotifStatus
	};
	bool DoActionOnUser(const ActionType action, const wxString& name);
	wxArrayString GetGroupNames() const;
	void AddUserToGroup(const wxString& group, const wxString& name);
	void AddGroup(const wxString& name);
	void DeleteGroup(const wxString& name);
	void RemoveUser(const wxString& name);
	void ChangeAction(const wxString& group, const ActionType action, bool add = true);
	ActionType GetGroupAction(const wxString& group) const;
	wxString GetGroupOfUser(const wxString& user) const;
	void SetGroupColor(const wxString& group, const wxColour& color);
	wxColour GetGroupColor(const wxString& group) const;
	bool IsKnown(const wxString& name, bool outputWarning = false) const;

	// helper functions to access settings file
	void SetGroupActions(const wxString& group, ActionType action) const;
	ActionType GetGroupActions(const wxString& group) const;
	wxColour GetGroupHLColor(const wxString& group = _T("default")) const;
	void SetGroupHLColor(const wxColour& color, const wxString& group = _T("default"));
	wxArrayString GetGroups();
	void SetPeopleList(const wxArrayString& friends, const wxString& group = _T("default"));
	wxArrayString GetPeopleList(const wxString& group = _T("default")) const;

private:
	//!provide a simple mapping between enum type and string to display in gui
	std::list<wxString> m_actionNames;

	//! Provide the names to be used by config file.
	std::list<wxString> m_configActionNames;
	//!same for tooltips
	std::list<wxString> m_actionTooltips;


	//lotsa maps to keep runtime finds, etc ti a minimum
	typedef std::map<wxString, wxArrayString> GroupMap;
	/// groupname --> array of people in the group
	GroupMap m_groupMap;
	typedef std::map<wxString, ActionType> GroupActionMap;
	/// groupname --> ActionType for that group
	GroupActionMap m_groupActions;
	typedef std::map<ActionType, wxArrayString> ActionGroupsMap;
	/// ActionType --> array of groups with that actiontype
	ActionGroupsMap m_actionsGroups;
	typedef std::map<ActionType, wxArrayString> ActionPeopleMap;
	/// ActionType --> array of people with that actiontype
	ActionPeopleMap m_actionsPeople;
	///nickname --> group map (we don't allow users to be in more than one group
	typedef std::map<wxString, wxString> PeopleGroupMap;
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
