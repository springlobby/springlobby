#include "useractions.h"

#include <wx/intl.h>
#include <wx/colour.h>
#include "settings.h"
#include <cmath>
#include "settings++/custom_dialogs.h"
#include "ui.h"

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
    // preventing action on oneself wasn't the best idea, login gets disabled
    //if ( m_knownUsers.Index( name ) == -1 || ui().IsThisMe(name) || action == ActNone )

    if ( m_knownUsers.Index( name ) == -1 || action == ActNone )
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
    m_knownUsers.Clear();
    for ( unsigned int i = 0; i < m_groupNames.GetCount(); ++i)
    {
        wxString name = m_groupNames[i];
        m_groupMap[name] = sett().GetPeopleList( name );
        for ( unsigned int k = 0; k < m_groupMap[name].GetCount(); ++k)
        {
            wxString user = m_groupMap[name][k];
            m_knownUsers.Add( user );
            m_peopleGroup[ user ] = name;
        }
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
                for ( unsigned int k = 0; k < m_groupMap[name].GetCount(); ++k)
                {
                    m_actionsPeople[cur].Add( (m_groupMap[name])[k] );
                }
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
    if ( m_knownUsers.Index( name ) != -1 )
    {
        customMessageBoxNoModal( SL_MAIN_ICON, _("To prevent logical inconsistencies, adding a user to more than one group is not allowed"),
            _("Cannot add user to group") );
            return;
    }
    if ( ui().IsThisMe( name ) )
    {
        customMessageBoxNoModal( SL_MAIN_ICON, _("No good can come from adding yourself to a group"),
            _("Cannot add yourself to group") );
            return;
    }
    m_groupMap[group].Add(name);
    sett().SetPeopleList( m_groupMap[group], group );
    Init();
}

void UserActions::DeleteGroup(const wxString& name )
{
    sett().DeleteGroup( name );
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

wxString UserActions::GetGroupOfUser( const wxString& user )
{
    return m_peopleGroup[ user ];
}

void UserActions::SetGroupColor( const wxString& group, const wxColour& color )
{
    sett().SetGroupHLColor( color, group );
}

wxColor UserActions::GetGroupColor( const wxString& group )
{
    return sett().GetGroupHLColor( group );
}

bool UserActions::IsKnown( const wxString& name )
{
    return ( m_knownUsers.Index( name ) != -1 );
}
