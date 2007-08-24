#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H


#include <wx/string.h>
#include <wx/gdicmn.h>

#include "iunitsync.h"
#include "ibattle.h"

class Ui;

class MainSinglePlayerTab;


class SinglePlayerBot
{
  public:

    SinglePlayerBot(): m_ally(-1),m_pos(-1) { }
    ~SinglePlayerBot() {}

    void SetAIDll( const wxString& dll ) { m_dll = dll; }
    wxString GetAIDll() { return m_dll; }

    void SetAlly( int ally ) { m_ally = ally; }
    int GetAlly() { return m_ally; }

    void SetSide( int side ) { m_side = side; }
    int GetSide() { return m_side; }

    void SetPosistion( int pos ) { m_pos = pos; }
    int GetPosistion() { return m_pos; }

    bool IsPlayer() { return m_dll.IsEmpty(); }
    bool IsBot() { return !m_dll.IsEmpty(); }

  private:

    int m_ally;
    int m_pos;
    int m_side;
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

    int AddBot( int ally, int position, const wxString& aidll );
    void RemoveBot( int index );
    void UpdateBot( int index, int ally, int position, int side );
    SinglePlayerBot& GetBot( int index );
    int GetNumBots();

    //SinglePlayerOptions& opts() { return m_opts; }

    void SetStartPos( int index, wxPoint pos );
    wxPoint GetStartPos( int index );
    void ResetStartPos( int index );

    void GetFreeColour( int& r, int& g, int& b, bool excludeme = true ) {}

    int GetMyAlly() { return -1; }
    void SetMyAlly( int ally ) {}

    void SendHostInfo( HostInfo update );

  protected:

    Ui& m_ui;
    MainSinglePlayerTab& m_sptab;

};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
