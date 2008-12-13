#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H


#include <wx/string.h>
#include <vector>

#include "ibattle.h"
#include "user.h"

class Ui;
class BattleBot;
class MainSinglePlayerTab;
class wxColour;




class SinglePlayerBattle: public IBattle
{
  public:

    SinglePlayerBattle( Ui& ui, MainSinglePlayerTab& msptab );
    ~SinglePlayerBattle();

    unsigned int AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll );
    void RemoveBot( unsigned int index );
    void UpdateBot( unsigned int index, int ally, int posx, int posy, int side );
    BattleBot* GetBot( unsigned int index ) const;
    BattleBot* GetBotByStartPosition( unsigned int startpos ) ;
    unsigned int GetNumBots() const;

    wxColour GetFreeColour( User *for_whom ) const;

    bool IsFounderMe() const { return true; }

    int GetFreeAlly();
    void GetFreePosition( int& x, int& y );

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag ){}

    void Update( const wxString& Tag );

  protected:

    Ui& m_ui;
    MainSinglePlayerTab& m_sptab;
    std::vector<BattleBot*> m_bots;

};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
