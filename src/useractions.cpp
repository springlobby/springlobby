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
    Init();
}

UserActions::~UserActions()
{

}

bool UserActions::DoActionOnUser( const ActionType action, const wxString& name )
{
    if ( action == ActNone )
        return false;
    else
        return ( m_actionsGroups.find( action ) != m_actionsGroups.end() && m_actionsPeople[action].Index( name ) != -1 );

    return false;
}

void UserActions::Init()
{
    m_groupNames = sett().GetGroups();
    m_groupMap.clear();
    m_groupActions.clear();
    m_actionsGroups.clear();
    m_actionsPeople.clear();
    for ( unsigned int i = 0; i < m_groupNames.GetCount(); ++i)
    {
        wxString name = m_groupNames[i];
        m_groupMap[name] = sett().GetPeopleList( name );
        m_groupActions[name] = sett().GetGroupActions( name );
    }
    for ( int i = 0; i < m_numActions; ++i)
    {
        ActionType cur = (ActionType) std::pow( 2.0, i);
        wxSortedArrayString tmp;
        for ( unsigned int i = 0; i < m_groupNames.GetCount(); ++i)
        {
            wxString name = m_groupNames[i];
            if ( ( m_groupActions[name] & cur ) != 0 )
            {
                tmp.Add( name );
            }
            for ( unsigned int k = 0; k < m_groupMap[name].GetCount(); ++k)
            {
                m_actionsPeople[cur].Add( (m_groupMap[name])[k] );
            }
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
    Init();
}

void UserActions::AddGroup(const wxString& name )
{
    sett().AddGroup( name );
    Init();
}

void UserActions::ChangeAction( const wxString& group, const ActionType action, bool add )
{
    ActionType old = m_groupActions[group];
    old = (ActionType) ( add ? (old | action) : (old & ~action ) );
    sett().SetGroupActions( group, old );
    Init();
}

UserActions::ActionType UserActions::GetGroupAction( const wxString& group )
{
    return m_groupActions[group];
}

