#ifndef SPRINGLOBBY_HEADERGUARD_IBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_IBATTLE_H


#include <wx/string.h>

#include "iunitsync.h"


typedef int StartType;
enum {
  ST_Fixed = 0,
  ST_Random = 1,
  ST_Choose = 2,
  ST_Pick = 3
};


typedef int GameType;
enum  {
  GT_ComContinue = 0,
  GT_ComEnds = 1,
  GT_Lineage = 2
};


typedef int BattleType;
enum {
  BT_Unknown = 0,
  BT_Multi = 1,
  BT_Single = 2
};


struct BattleStartRect
{
  BattleStartRect() { local = true; updated = false; deleted = false; }
  bool local;
  bool updated;
  bool deleted;

  int ally;
  int top;
  int left;
  int right;
  int bottom;
};


class IBattle
{
  public:

    IBattle();
    virtual ~IBattle();

    virtual void SetMap( const wxString& mapname, const wxString& hash );
    virtual void SetMap( const UnitSyncMap& map );
    virtual const UnitSyncMap& Map();
    virtual wxString GetMapName();
    virtual wxString GetMapHash();

    virtual void SetMod( const wxString& modname, const wxString& hash );
    virtual void SetMod( const UnitSyncMod& mod );
    virtual const UnitSyncMod& Mod();
    virtual wxString GetModName();
    virtual wxString GetModHash();

    virtual void SetGameType( GameType gt );
    virtual GameType GetGameType();
    virtual void SetStartType( StartType st );
    virtual StartType GetStartType();

    virtual bool MapExists();
    virtual bool ModExists();

    virtual void GetFreeColour( int& r, int& g, int& b, bool excludeme = true ) = 0;

    virtual BattleStartRect* GetStartRect( int allyno ) { return 0; };
    virtual void AddStartRect( int allyno, int left, int top, int right, int bottom ) {};
    virtual void RemoveStartRect( int allyno ) {};
    virtual void UpdateStartRect( int allyno ) {};

    virtual int GetMyAlly() = 0;
    virtual void SetMyAlly( int ally ) = 0;
  protected:

    bool m_map_loaded;
    bool m_mod_loaded;
    bool m_map_exists;
    bool m_mod_exists;
    UnitSyncMap m_map;
    UnitSyncMod m_mod;
    wxString m_map_name;
    wxString m_mod_name;

    GameType m_gametype;
    StartType m_starttype;

};

#endif // SPRINGLOBBY_HEADERGUARD_IBATTLE_H
