/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_USERMENU_H
#define SPRINGLOBBY_USERMENU_H

#include <wx/menu.h>
#include <map>
#include <vector>
#include "utils/mixins.h"
//static const long GROUP_ID  = wxNewId();
static const long GROUP_ID_NEW = wxNewId();
static const long GROUP_ID_REMOVE = wxNewId();
#include "ui.h"
#include "useractions.h"
namespace SL_GENERIC
{
template <class EventHandler_>
class UserMenu : public wxMenu, public SL::NonCopyable
{
protected:
	typedef EventHandler_
	    EventHandler;
	typedef EventHandler
	    ParentType;

public:
	UserMenu(wxEvtHandler* /*connectee*/, ParentType* parent, const wxString& title = wxEmptyString, long style = 0)
	    : wxMenu(title, style)
	    , m_groupsMenu(0)
	    , m_parent(parent)
	{
		assert(m_parent);
		m_groupsMenu = new wxMenu();
		m_groupsnewItem = new wxMenuItem(m_groupsMenu, GROUP_ID_NEW, _("Create new group..."));
		Connect(GROUP_ID_NEW, wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(EventHandler::OnUserMenuCreateGroup), 0, m_parent);
		m_groupsMenu->Append(m_groupsnewItem);
		m_groupsMenu->AppendSeparator();
		m_groupsMenuItem = AppendSubMenu(m_groupsMenu, _("Add to group..."));
		m_groupsDeleteItem = new wxMenuItem(m_groupsMenu, GROUP_ID_REMOVE, _("Remove from group"));
		Connect(GROUP_ID_REMOVE, wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(EventHandler::OnUserMenuDeleteFromGroup), 0, m_parent);
		Append(m_groupsDeleteItem);
	}

	void EnableItems(bool isUserSelected, const wxString& nick)
	{
		if (isUserSelected) {
			bool enable = (!ui().IsThisMe(nick));
			m_groupsMenuItem->Enable(enable && !useractions().IsKnown(nick));
			m_groupsnewItem->Enable(enable && !useractions().IsKnown(nick));
			m_groupsDeleteItem->Enable(enable && useractions().IsKnown(nick));
			UpdateGroups();
		} else {
			m_groupsMenuItem->Enable(false);
			m_groupsDeleteItem->Enable(false);
			m_groupsnewItem->Enable(false);
		}
	}

	wxString GetGroupByEvtID(const int id)
	{
		return m_idNameMap[id];
	}

	//we need these to circumvent the submneu events not firing via connecting the events in the parent class
	std::vector<int> GetGroupIds()
	{
		std::vector<int> ids;
		std::map<int, wxString>::const_iterator it = m_idNameMap.begin();
		for (; it != m_idNameMap.end(); ++it) {
			int id = it->first;
			if (id != GROUP_ID_NEW && id != GROUP_ID_REMOVE)
				ids.push_back(id);
		}
		return ids;
	}

private:
	wxMenu* m_groupsMenu;
	wxMenuItem* m_groupsMenuItem;
	wxMenuItem* m_groupsDeleteItem;
	wxMenuItem* m_groupsnewItem;
	ParentType* m_parent;
	std::map<int, wxString> m_idNameMap;
	std::map<wxString, int> m_NameIdMap;

	void UpdateGroups()
	{
		wxArrayString groupNames = useractions().GetGroupNames();
		// delete all menu items not in groupNames
		int count = m_groupsMenu->GetMenuItemCount();
		for (int i = count - 1; i >= 0; i--) {
			wxMenuItem* cur = m_groupsMenu->FindItemByPosition(i);
			// skip default entries
			if ((cur == m_groupsMenuItem) || (cur == m_groupsDeleteItem) || (cur == m_groupsnewItem) || (cur->IsSeparator()))
				continue;

			if (groupNames.Index(cur->GetItemLabelText()) != wxNOT_FOUND)
				continue;

			const int id = cur->GetId();
			m_NameIdMap.erase(cur->GetItemLabelText());
			m_idNameMap.erase(id);
			m_groupsMenu->Delete(id);
		}

		// add all missing menu items in groupNames
		for (unsigned int i = 0; i < groupNames.GetCount(); ++i) {
			const int id = m_NameIdMap[groupNames[i]];
			if (id != 0)
				continue;
			wxMenuItem* item = m_groupsMenu->FindItem(id);
			if (item == NULL) {
				wxMenuItem* addItem = new wxMenuItem(m_groupsMenu, wxID_ANY, groupNames[i], wxEmptyString, wxITEM_NORMAL);
				const int newID = addItem->GetId();
				m_groupsMenu->Append(addItem);

				m_idNameMap[newID] = groupNames[i];
				m_NameIdMap[groupNames[i]] = newID;
			}
		}
	}
	//DECLARE_EVENT_TABLE();
};
} // namespace SL_GENERIC
#endif // SPRINGLOBBY_USERMENU_H
