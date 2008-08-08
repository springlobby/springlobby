#ifndef USERACTIONS_HH_INCLUDED
#define USERACTIONS_HH_INCLUDED

#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>

class UserActions {

public:
    UserActions();
    ~UserActions();

     enum ActionType {
       ActNone = 1,
       ActHighlight = 2,
       ActNotifLogin = 4,
       ActIgnore = 8,
       ActAutokick = 16
     };
    static const int m_numActions = 5;
    bool DoActionOnUser( const ActionType action, const wxString& name ) ;
    wxSortedArrayString GetGroupNames() const;
    void AddUserToGroup( const wxString& group, const wxString& name );

protected:
    typedef std::map<wxString,wxSortedArrayString> GroupMap;
    GroupMap m_groupMap;
    typedef std::map<wxString,ActionType> GroupActionMap;
    GroupActionMap m_groupActions;
    typedef std::map<ActionType,wxSortedArrayString> ActionGroupsMap;
    ActionGroupsMap m_actionsGroups;

    //set another map so we don't have to evaluate actions at runtime
    void SetActionsGroupMap();

    wxSortedArrayString m_groupNames;

};
#endif // USERACTIONS_HH_INCLUDED
