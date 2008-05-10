#ifndef SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H
#define SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>

#include "mmoptionmodel.h"

class wxImage;

typedef std::map<wxString,mmOptionBool> optionMapBool;
typedef std::map<wxString,mmOptionFloat> optionMapFloat;
typedef std::map<wxString,mmOptionString> optionMapString;
typedef std::map<wxString,mmOptionList> optionMapList;
typedef std::map<wxString,mmOptionInt> optionMapInt;

typedef std::map<wxString,mmOptionBool>::iterator optionMapBoolIter;
typedef std::map<wxString,mmOptionFloat>::iterator optionMapFloatIter;
typedef std::map<wxString,mmOptionString>::iterator optionMapStringIter;
typedef std::map<wxString,mmOptionList>::iterator optionMapListIter;
typedef std::map<wxString,mmOptionInt>::iterator optionMapIntIter;


struct UnitSyncMod
{
  UnitSyncMod() : name(_T("")),hash(_T("NULL")) { }
  wxString name;
  wxString hash;
};

struct StartPos
{
  int x;
  int y;
};

struct MapInfo
{
  wxString description;
  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;
  int posCount;
  StartPos positions[16];

  wxString author;
};

struct UnitSyncMap
{
  UnitSyncMap() : name(_T("")),hash(_T("NULL")) { }
  wxString name;
  wxString hash;
  MapInfo info;
};

typedef int GameFeature;
enum {
  GF_XYStartPos = 1,
  USYNC_Sett_Handler = 2
};

struct GameOptions
{
  optionMapBool bool_map;
  optionMapFloat float_map;
  optionMapString string_map;
  optionMapList list_map;
  optionMapInt int_map;
};

class IUnitSync
{
  public:
    virtual ~IUnitSync() { }

    virtual int GetNumMods() = 0;
    virtual bool ModExists( const wxString& modname ) = 0;
    virtual bool ModExists( const wxString& modname, const wxString& hash ) = 0;
    virtual UnitSyncMod GetMod( const wxString& modname ) = 0;
    virtual UnitSyncMod GetMod( int index ) = 0;
    virtual int GetModIndex( const wxString& name ) = 0;
    virtual wxString GetModArchive( int index ) = 0;
    virtual GameOptions GetModOptions( const wxString& name ) = 0;

    virtual int GetNumMaps() = 0;
    virtual bool MapExists( const wxString& mapname ) = 0;
    virtual bool MapExists( const wxString& mapname, const wxString& hash ) = 0;

    virtual UnitSyncMap GetMap( const wxString& mapname ) = 0;
    virtual UnitSyncMap GetMap( int index ) = 0;
    virtual UnitSyncMap GetMapEx( const wxString& mapname ) = 0;
    virtual UnitSyncMap GetMapEx( int index ) = 0;
    virtual GameOptions GetMapOptions( const wxString& name ) = 0;

    virtual int GetMapIndex( const wxString& name ) = 0;
    virtual wxImage GetMinimap( const wxString& mapname, int max_w, int max_h, bool store_size = false ) = 0;

    virtual int GetSideCount( const wxString& modname ) = 0;
    virtual wxString GetSideName( const wxString& modname, int index ) = 0;
    virtual wxImage GetSidePicture( const wxString& modname, const wxString& SideName ) =0;

    virtual int GetNumUnits( const wxString& modname ) = 0;
    virtual wxArrayString GetUnitsList( const wxString& modname ) = 0;

    virtual bool LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc ) = 0;
    virtual void FreeUnitSyncLib() = 0;

    virtual bool IsLoaded() = 0;

    virtual wxString GetSpringVersion() = 0;
    virtual bool VersionSupports( GameFeature feature ) = 0;

    virtual wxArrayString GetAIList( const wxString& modname ) = 0;

    virtual bool CacheMapInfo( const wxString& map ) = 0;
    virtual bool CacheMinimap( const wxString& map ) = 0;
    virtual bool CacheModUnits( const wxString& mod ) = 0;
    virtual bool ReloadUnitSyncLib() = 0;

    virtual void SetSpringDataPath( const wxString& path ) = 0;
    virtual wxString GetSpringDataPath() = 0;

    virtual bool FileExists( const wxString& name ) = 0;
};

IUnitSync* usync();

#endif // SPRINGLOBBY_HEADERGUARD_IUNITSYNC_H
