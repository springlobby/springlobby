#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H

#include <wx/listctrl.h>

class User;
class Battle;
struct BattleBot;
//class wxMenuItem;

class BattleroomListCtrl : public wxListCtrl
{
  public:
    BattleroomListCtrl( wxWindow* parent, Battle& battle );
     ~BattleroomListCtrl();

    void AddUser( User& user );
    void RemoveUser( User& user );
    void UpdateUser( const int& index );
    void UpdateUser( User& user );
    void UpdateList();

    void AddBot( BattleBot& bot );
    void RemoveBot( BattleBot& bot );
    void UpdateBot( BattleBot& bot );
    void UpdateBot( const int& index );

    int GetUserIndex( User& user );
    int GetBotIndex( BattleBot& bot );

    void OnListRightClick( wxListEvent& event );
    void OnTeamSelect( wxCommandEvent& event );
    void OnAllySelect( wxCommandEvent& event );
    void OnColourSelect( wxCommandEvent& event );
    void OnSideSelect( wxCommandEvent& event );
    void OnSpecSelect( wxCommandEvent& event );

    void OnKickPlayer( wxCommandEvent& event );
    void OnRingPlayer( wxCommandEvent& event );

  protected:

    Battle& m_battle;

    wxMenu* m_popup;

    User* m_sel_user;
    BattleBot* m_sel_bot;

    wxMenu* m_sides;
    wxMenuItem* m_spec_item;

    DECLARE_EVENT_TABLE();

};

enum
{
  BRLIST_LIST = wxID_HIGHEST,
  BRLIST_TEAM,
  BRLIST_ALLY = BRLIST_TEAM + 1000,
  BRLIST_COLOUR = BRLIST_ALLY + 1000,
  BRLIST_SIDE = BRLIST_COLOUR + 1000,
  BRLIST_SPEC = BRLIST_SIDE + 1000,
  BRLIST_KICK,
  BRLIST_RING
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
