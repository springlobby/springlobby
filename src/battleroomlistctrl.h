#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H

#include <wx/listctrl.h>

class User;
class Battle;
struct BattleBot;

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

  protected:

    Battle& m_battle;

};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMLISTCTRL_H
