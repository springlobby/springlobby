#include "useractions.hh"

#include <wx/intl.h>
#include "settings.h"
#include <cmath>

UserActions& useractions()
{
    static UserActions m_useractions;
    return m_useractions;
}

UserActions::UserActions()
{
//    m_actionNames = wxArrayString ( { _T("highlight"),_T("notify login"),_T("ignore"),_T("autokick") } );
    m_groupNames = sett().GetGroups();
    for ( unsigned int i = 0; i < m_groupNames.GetCount(); ++i)
    {
        wxString name = m_groupNames[i];
        m_groupMap[name] = sett().GetPeopleList( name );
        m_groupActions[name] = sett().GetGroupActions( name );
    }
    SetActionsGroupMap();
}

UserActions::~UserActions()
{

}

bool UserActions::DoActionOnUser( const ActionType action, const wxString& name )
{
    if ( action == ActNone )
        return false;
    else
        return ( m_actionsGroups.find( action ) != m_actionsGroups.end() &&
            m_actionsGroups[action].Index( name ) != -1 );
    return false;
}

void UserActions::SetActionsGroupMap()
{
    for ( int i = 0; i < m_numActions; ++i)
    {
        ActionType cur = (ActionType) std::pow( 2.0, i);
        wxSortedArrayString tmp;
        for ( unsigned int i = 0; i < m_groupNames.GetCount(); ++i)
        {
            wxString name = m_groupNames[i];
            if ( ( m_groupActions[name] & cur ) != 0 )
                tmp.Add( name );
        }
        m_actionsGroups[cur] = tmp;
    }
    m_actionsGroups[ActNone] = m_groupNames;
}

wxSortedArrayString UserActions::GetGroupNames() const
{
    return m_groupNames;
}

void UserActions::AddUserToGroup( const wxString& group, const wxString& name )
{
    m_groupMap[group].Add(name);
    sett().SetPeopleList( m_groupMap[group], group );
}
