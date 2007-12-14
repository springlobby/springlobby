#ifndef SPRINGLOBBY_HEADERGUARD_IBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_IBATTLE_H


#include <wx/string.h>
#include <list>
#include <vector>

#include "iunitsync.h"
#include "user.h"

typedef int HostInfo;

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


struct BattleBot {
  UserBattleStatus bs;
  int posx;
  int posy;
  int handicap;
  std::string name;
  std::string owner;
  std::string aidll;

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

    virtual bool IsFounderMe() =0;

    virtual void SetStartMetal( const int& smetal ) { m_startmetal = smetal; }
    virtual int GetStartMetal() { return m_startmetal; }
    virtual void SetStartEnergy( const int& senergy ) { m_startenergy = senergy; }
    virtual int GetStartEnergy() { return m_startenergy; }
    virtual void SetMaxUnits( const int& maxunits ) { m_maxunits = maxunits; }
    virtual int GetMaxUnits() { return m_maxunits; }

    virtual void SetLimitDGun( const bool& limdgun ) { m_limitdgun = limdgun; }
    virtual bool LimitDGun() { return m_limitdgun; }
    virtual void SetDimMMs( const bool& dimmm ) { m_dimmms = dimmm; }
    virtual bool DimMMs() { return m_dimmms; }
    virtual void SetGhostedBuildings( const bool& gbuilds ) { m_ghostedbuildings = gbuilds; }
    virtual bool GhostedBuildings() { return m_ghostedbuildings; }

    virtual void SendHostInfo( HostInfo update ) = 0;

    virtual BattleBot* GetBotByStartPosition( unsigned int startpos ) { return 0; };
    virtual BattleBot* GetBot( unsigned int index ) = 0;
    virtual BattleBot* GetBot( const std::string& name ) { return 0; };
    virtual unsigned int GetNumBots() = 0;
    virtual unsigned int AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll );
    virtual void RemoveBot( unsigned int index ) {};

    virtual void GetFreePosition( int& x, int& y ) {}
    virtual int GetFreeAlly() { return 0; }

    virtual void DisableUnit( const std::string& unitname );
    virtual void EnableUnit( const std::string& unitname );
    virtual void EnableAllUnits();
    virtual int GetNumDisabledUnits() { return m_units_num; }
    virtual std::string DisabledUnits();

    virtual void OnUnitSyncReloaded();

    virtual std::vector<BattleStartRect*>::size_type GetNumRects() =0;

  protected:

    bool m_map_loaded;
    bool m_mod_loaded;
    bool m_map_exists;
    bool m_mod_exists;
    UnitSyncMap m_map;
    UnitSyncMod m_mod;
    wxString m_map_name;
    wxString m_mod_name;

    int m_startmetal;
    int m_startenergy;
    int m_maxunits;

    bool m_limitdgun;
    bool m_dimmms;
    bool m_ghostedbuildings;

    GameType m_gametype;
    StartType m_starttype;

    std::string m_units;
    int m_units_num;
};

#endif // SPRINGLOBBY_HEADERGUARD_IBATTLE_H
