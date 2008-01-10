#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H


#include <wx/string.h>
#include <wx/gdicmn.h>
#include <vector>

#include "iunitsync.h"
#include "ibattle.h"

class Ui;

class MainSinglePlayerTab;


class SinglePlayerBot
{
  public:

    SinglePlayerBot(): m_ally(-1),m_pos_x(0),m_pos_y(0) { }
    ~SinglePlayerBot() {}

    void SetAIDll( const wxString& dll ) { m_dll = dll; }
    wxString GetAIDll() { return m_dll; }

    void SetAlly( int ally ) { m_ally = ally; }
    int GetAlly() { return m_ally; }

    void SetSide( int side ) { m_side = side; }
    int GetSide() { return m_side; }

    void SetPosistion( int posx, int posy ) { m_pos_x = posx; m_pos_y = posy; }
    void SetPosistionX( int pos ) { m_pos_x = pos; }
    int GetPosistionX() { return m_pos_x; }
    void SetPosistionY( int pos ) { m_pos_y = pos; }
    int GetPosistionY() { return m_pos_y; }

    void SetHandicap( int handicap ) { m_handicap = handicap; }
    int GetHandicap() { return m_handicap; }

    bool IsPlayer() { return m_dll.IsEmpty(); }
    bool IsBot() { return !m_dll.IsEmpty(); }

  private:

    int m_ally;
    unsigned int m_pos_x;
    unsigned int m_pos_y;
    int m_side;
    int m_handicap;
    wxString m_dll;

};

/*
class SinglePlayerOptions
{
  public:

    void SetGameType( StartType gametype ) { m_type = gametype; }
    StartType GetGameType() { return m_type; }

    void SetMap( UnitSyncMap map ) { m_map = map; }
    UnitSyncMap& Map() { return m_map; }

  protected:

    StartType m_type;
    wxString m_mapname;
    UnitSyncMap m_map;

};*/


class SinglePlayerBattle: public IBattle
{
  public:

    SinglePlayerBattle( Ui& ui, MainSinglePlayerTab& msptab );
    ~SinglePlayerBattle();

    unsigned int AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll );
    void RemoveBot( unsigned int index );
    void UpdateBot( unsigned int index, int ally, int posx, int posy, int side );
    BattleBot* GetBot( unsigned int index );
    BattleBot* GetBotByStartPosition( unsigned int startpos );
    unsigned int GetNumBots();

    void GetFreeColour( int& r, int& g, int& b, bool excludeme = true );

    int GetMyAlly() { return -1; }
    void SetMyAlly( int ally ) {}

    bool IsFounderMe() { return true; }

    int GetFreeAlly();
    void GetFreePosition( int& x, int& y );

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag ){}

    std::vector<BattleStartRect*>::size_type GetNumRects() { return 0; }

  protected:

    Ui& m_ui;
    MainSinglePlayerTab& m_sptab;
    std::vector<BattleBot*> m_bots;

};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
