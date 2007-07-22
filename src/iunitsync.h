#ifndef SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H

#include <string>

class wxImage;

struct UnitSyncMod
{
  UnitSyncMod() : name(""),hash("NULL") { }
  std::string name;
  std::string hash;
};

struct StartPos
{
  int x;
  int z;
};

struct MapInfo
{
  char* description;
  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  // 0.61b1+
  int width;
  int height;
  int posCount;
  StartPos positions[16];  // I'd rather not allocate memory, this should be enough

  // VERSION>=1
  char *author; // max 200 chars
};

struct UnitSyncMap
{
  UnitSyncMap() : name(""),hash("NULL") { }
  std::string name;
  std::string hash;
  MapInfo info;
};

class IUnitSync
{
  public:
    virtual ~IUnitSync() { }

    virtual int GetNumMods() = 0;
    virtual bool ModExists( const std::string& modname ) = 0;
    virtual UnitSyncMod GetMod( const std::string& modname ) = 0;
    virtual int GetModIndex( const std::string& name ) = 0;
    virtual std::string GetModArchive( int index ) = 0;

    virtual int GetNumMaps() = 0;
    virtual bool MapExists( const std::string& mapname ) = 0;
    virtual bool MapExists( const std::string& mapname, const std::string hash ) = 0;
    virtual UnitSyncMap GetMap( const std::string& mapname ) = 0;
    virtual int GetMapIndex( const std::string& name ) = 0;
    virtual wxImage GetMinimap( const std::string& mapname, int size ) =0;

    virtual int GetSideCount( const std::string& modname ) = 0;
    virtual std::string GetSideName( const std::string& modname, int index ) = 0;

    virtual bool LoadUnitSyncLib() = 0;
    virtual void FreeUnitSyncLib() = 0;

    virtual bool IsLoaded() = 0;

    virtual int GetNumUnits( const std::string& modname ) = 0;
    virtual int GetUnitIndex( const std::string& modname, const std::string& name ) = 0;
    virtual std::string GetFullUnitName( const std::string& modname, int index ) = 0;
};

IUnitSync* usync();

#endif // SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H
