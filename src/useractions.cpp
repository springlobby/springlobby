#include "useractions.h"

#include <wx/intl.h>
#include <wx/colour.h>
#include "settings.h"
#include <cmath>
#include "utils/customdialogs.h"

#include "mainwindow.h"
#include "mainchattab.h"
#include "hosting/mainjoinbattletab.h"
#include "hosting/battleroomtab.h"
#include "battlelist/battlelisttab.h"
#include "chatpanel.h"
#include <options/mainoptionstab.h>
#include <options/groupoptionspanel.h>
#include <lslutils/globalsmanager.h>

UserActions& useractions()
{
    static LSL::Util::LineInfo<UserActions> m( AT );
    static LSL::Util::GlobalObjectHolder<UserActions, LSL::Util::LineInfo<UserActions> > m_useractions( m );
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
        ActionType cur = (ActionType) (int) std::pow( 2.0, i);
        wxArrayString tmp;
        for ( unsigned int j = 0; j < m_groupNames.GetCount(); ++j)
        {
            wxString name = m_groupNames[j];
            if ( ( m_groupActions[name] & cur ) != 0 )
            {
                tmp.Add( name );
                for ( unsigned int k = 0; k < m_groupMap[name].GetCount(); ++k)
                {
                    m_actionsPeople[cur].Add( (m_groupMap[name])[k] );
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
    try
    {
      ui().mw().GetBattleListTab().UpdateHighlights();
    } catch(...){}

    try
    {
      ui().mw().GetChatTab().UpdateNicklistHighlights();
    } catch(...){}

    try
    {
      ui().mw().GetJoinTab().GetBattleRoomTab().UpdateHighlights();
    } catch(...){}
}

wxArrayString UserActions::GetGroupNames() const
{
    return m_groupNames;
}

void UserActions::AddUserToGroup( const wxString& group, const wxString& name )
{
    if ( IsKnown( name , false ) || ui().IsThisMe( name ) )
        return;
    m_groupMap[group].Add(name);
    sett().SetPeopleList( m_groupMap[group], group );
    Init();
    UpdateUI();
}

void UserActions::DeleteGroup(const wxString& name )
{
    sett().DeleteGroup( name );
    Init();
    UpdateUI();
}

void UserActions::AddGroup(const wxString& name )
{
    sett().AddGroup( name );
    Init();
    UpdateUI();
}

void UserActions::ChangeAction( const wxString& group, const ActionType action, bool add )
{
    ActionType old = m_groupActions[group];
    old = (ActionType) ( add ? (old | action) : (old & ~action ) );
    sett().SetGroupActions( group, old );
    Init();
    UpdateUI();
}

UserActions::ActionType UserActions::GetGroupAction( const wxString& group ) const
{
	const GroupActionMap::const_iterator res = m_groupActions.find(group);
    return res->second;
}

wxString UserActions::GetGroupOfUser( const wxString& user ) const
{
	const PeopleGroupMap::const_iterator res = m_peopleGroup.find(user);
    return res->second;
}

void UserActions::SetGroupColor( const wxString& group, const wxColour& color )
{
    sett().SetGroupHLColor( color, group );
    Init();
    UpdateUI();
}

wxColour UserActions::GetGroupColor( const wxString& group ) const
{
    return sett().GetGroupHLColor( group );
}

bool UserActions::IsKnown( const wxString& name, bool outputWarning ) const
{
    bool ret = m_knownUsers.Index( name ) != -1;
    if ( outputWarning ){
        customMessageBoxNoModal( SL_MAIN_ICON, _("To prevent logical inconsistencies, adding a user to more than one group is not allowed"),
           _("Cannot add user to group") );
    }

    return ret;
}

void UserActions::RemoveUser(const wxString& name )
{
    wxString group = m_peopleGroup[name];
    m_groupMap[group].Remove(name);
    sett().SetPeopleList( m_groupMap[group], group );
    Init();
    UpdateUI();
}
