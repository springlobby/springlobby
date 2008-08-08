#ifndef USERACTIONS_HH_INCLUDED
#define USERACTIONS_HH_INCLUDED

#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>

const wxString m_actionNames[] = { _T("none"),_T("highlight"),_T("notify login"),_T("ignore"),_T("autokick") };


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
    void AddGroup(const wxString& name );
    void ChangeAction( const wxString& group, const ActionType action, bool add = true );
    ActionType GetGroupAction( const wxString& group );

protected:
    typedef std::map<wxString,wxSortedArrayString> GroupMap;
    GroupMap m_groupMap;
    typedef std::map<wxString,ActionType> GroupActionMap;
    GroupActionMap m_groupActions;
    typedef std::map<ActionType,wxSortedArrayString> ActionGroupsMap;
    ActionGroupsMap m_actionsGroups;

    //reload all maps and stuff
    void Init();

    wxSortedArrayString m_groupNames;
};

UserActions& useractions();
#endif // USERACTIONS_HH_INCLUDED
