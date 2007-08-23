#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H


#include <wx/string.h>
#include <wx/gdicmn.h>

class Ui;


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


typedef int SPGameType;
enum {
	SPGT_Pick = 1,
	SPGT_Random
};


class SinglePlayerOptions
{
  public:

    void SetGameType( SPGameType gametype ) { m_type = gametype; }
    SPGameType GetGameType() { return m_type; }

  protected:

    SPGameType m_type;

};


class SinglePlayerBattle
{
  public:

    SinglePlayerBattle( Ui& ui );
    ~SinglePlayerBattle();

    int AddBot( int ally, int position, const wxString& aidll );
    void RemoveBot( int index );
    void UpdateBot( int index, int ally, int position, int side );
    SinglePlayerBot& GetBot( int index );
    int GetNumBots();

    SinglePlayerOptions& opts() { return m_opts; }

    void SetStartPos( int index, wxPoint pos );
    wxPoint GetStartPos( int index );

  protected:

    Ui& m_ui;
    SinglePlayerOptions m_opts;

};

#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERBATTLE_H
