#ifndef SPRINGLOBBY_USERMENU_H
#define SPRINGLOBBY_USERMENU_H

#include <wx/menu.h>
#include <map>

static const long GROUP_ID  = wxNewId();//!TODO wxNewID, static int
#include "ui.h"
#include "useractions.h"

namespace SL_GENERIC {
template < class EventHandler_ >
class UserMenu : public wxMenu
{
    protected:
        typedef EventHandler_
            EventHandler;

    public:
        UserMenu(EventHandler* parent, const wxString& title = wxEmptyString, long style = 0)
        : wxMenu( title, style ),m_groupsMenu(0), m_parent(parent),m_groupCounter(0)
        {
            m_groupsMenu = new wxMenu();
            m_groupsnewItem = new wxMenuItem( m_groupsMenu, GROUP_ID - 2, _("Create new group...")  );
            m_parent->Connect( GROUP_ID - 2, wxEVT_COMMAND_MENU_SELECTED,
                                    wxCommandEventHandler( EventHandler::OnUserMenuCreateGroup ) );
            m_groupsMenu->Append( m_groupsnewItem );
            m_groupsMenu->AppendSeparator();
        //    if ( !ui().IsThisMe( m_parent->GetSelectedUser() ) )
            m_groupsMenuItem = AppendSubMenu( m_groupsMenu, _("Add to group..."));
            m_groupsDeleteItem = new wxMenuItem( m_groupsMenu, GROUP_ID - 1, _("Remove from group")  );
            m_parent->Connect( GROUP_ID - 1, wxEVT_COMMAND_MENU_SELECTED,
                                    wxCommandEventHandler( EventHandler::OnUserMenuDeleteFromGroup ) );
            Append( m_groupsDeleteItem );
        }

        ~UserMenu(){}

        void EnableItems(bool isUserSelected, const wxString& nick)
        {
            if ( isUserSelected )
            {
                bool enable = ( !ui().IsThisMe( nick ) );
                m_groupsMenuItem->Enable( enable && !useractions().IsKnown( nick ) ) ;
                m_groupsnewItem->Enable( enable && !useractions().IsKnown( nick ) ) ;
                m_groupsDeleteItem->Enable( enable && useractions().IsKnown( nick ) ) ;
                UpdateGroups();
            }
            else
            {
                m_groupsMenuItem->Enable( false ) ;
                m_groupsDeleteItem->Enable( false ) ;
                m_groupsnewItem->Enable( false );
            }

        }

        wxString GetGroupByEvtID( const unsigned int id )
        {
            if ( id < m_idNameMap.size() )
                return m_idNameMap[id];
            else
                return wxEmptyString;
        }

    protected:
        wxMenu* m_groupsMenu;
        wxMenuItem* m_groupsMenuItem;
        wxMenuItem* m_groupsDeleteItem;
        wxMenuItem* m_groupsnewItem;
        wxArrayString m_oldGroups;
        EventHandler* m_parent;
        unsigned int m_groupCounter;
        std::map<unsigned int, wxString> m_idNameMap;
        std::map<wxString, unsigned int> m_NameIdMap;

        void UpdateGroups()
        {
            wxSortedArrayString groupNames = useractions().GetGroupNames();
            bool first = m_oldGroups.GetCount() == 0;
            if ( first )
                m_oldGroups = groupNames;
            for ( unsigned int i = 0; i < groupNames.GetCount(); ++i)
            {
                if ( m_oldGroups.Index( groupNames[i] ) == wxNOT_FOUND || first )
                {
                    m_idNameMap[m_groupCounter] = groupNames[i];
                    wxMenuItem* addItem = new wxMenuItem( m_groupsMenu, GROUP_ID + m_groupCounter ,  groupNames[i] , wxEmptyString, wxITEM_NORMAL );
                    m_groupsMenu->Append( addItem );
                    m_parent->Connect( GROUP_ID + m_groupCounter, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( EventHandler::OnUserMenuAddToGroup ) );
                    m_oldGroups.Add( groupNames[i] );
                    m_idNameMap[GROUP_ID + m_groupCounter]  = groupNames[i];
                    m_NameIdMap[groupNames[i]]  = GROUP_ID + m_groupCounter;
                    m_groupCounter++;
                }
                else
                {
                    //wxMenuItem* old = FindItem( m_NameIdMap[groupNames[i]] );
                    Destroy( m_NameIdMap[groupNames[i]] );
                }
            }
        }
    //DECLARE_EVENT_TABLE();

};

}
#endif // SPRINGLOBBY_USERMENU_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

