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
    void UpdateBot( unsigned int index, int ally, int posx, int posy, int side );

    bool IsFounderMe() { return true; }

    User& GetMe() { return m_me; }

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag ){}

    void Update( const wxString& Tag );

  protected:

    Ui& m_ui;
    MainSinglePlayerTab& m_sptab;

    User m_me;

};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
