/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/dynlib.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/file.h>
//#include <wx/txtstrm.h>
//#include <wx/wfstream.h>
#include <wx/textfile.h>

#include <stdexcept>

#include "springunitsync.h"
#include "utils.h"
#include "settings.h"
#include "springunitsynclib.h"


#define LOCK_UNITSYNC wxCriticalSectionLocker lock_criticalsection(m_lock)


struct CachedMapInfo
{
  char name[256];
  char author[256];
  char description[256];

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
};


IUnitSync* usync()
{
  static SpringUnitSync* m_sync = 0;
  if (!m_sync)
    m_sync = new SpringUnitSync;
  return m_sync;
}


bool SpringUnitSync::LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  wxLogDebugFunc( _T("") );
  LOCK_UNITSYNC;
  return _LoadUnitSyncLib( springdir, unitsyncloc );
}


bool SpringUnitSync::_LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  wxSetWorkingDirectory( springdir );
  try {
    susynclib()->Load( unitsyncloc );
  } catch (...) {
    return false;
  }
  return true;
}


void SpringUnitSync::FreeUnitSyncLib()
{
  wxLogDebugFunc( _T("") );
  susynclib()->Unload();
}


bool SpringUnitSync::IsLoaded()
{
  return susynclib()->IsLoaded();
}


std::string SpringUnitSync::GetSpringVersion()
{
  wxLogDebugFunc( _T("") );
<<<<<<< HEAD:src/springunitsync.cpp
  std::string ret;
=======
>>>>>>> 3b057734670f1d37b9ed7bc911ca2bd14a2555c9:src/springunitsync.cpp
  try
  {
<<<<<<< HEAD:src/springunitsync.cpp
    ret = STD_STRING(susynclib()->GetSpringVersion());
  }
  catch (...){}
  return ret;
}


bool SpringUnitSync::VersionSupports( GameFeature feature )
{
  wxString ver = WX_STRING( GetSpringVersion() );
  float nver = 0;
  if      ( ver == _T("0.75b2") ) nver = 75.2;
  else if ( ver == _T("0.75b2+") ) nver = 76.0;
  else if ( ver == _T("0.76b1") ) nver = 76.1;

  switch (feature) {
    case GF_XYStartPos: return nver >= 76.0;
    case USYNC_Sett_Handler: return nver >= 76.0;
  }
  return false;
=======
  return STD_STRING(susynclib()->GetSpringVersion());
  } catch (...) {}
  return "";
>>>>>>> 3b057734670f1d37b9ed7bc911ca2bd14a2555c9:src/springunitsync.cpp
}


int SpringUnitSync::GetNumMods()
{
  wxLogDebugFunc( _T("") );
  return susynclib()->GetPrimaryModCount();
}


int SpringUnitSync::GetModIndex( const std::string& name )
{
  wxLogDebugFunc( _T("name = \"") + WX_STRING(name) + _T("\"") );
  try
  {
    return susynclib()->GetModIndex( WX_STRING(name) );
  } catch (...){}
  return -1;
}

/*
int SpringUnitSync::_GetModIndex( const std::string& name )
{
  try {
    int count = susynclib()->GetPrimaryModCount();
    for ( int i = 0; i < count; i++ ) {
      std::string cmp = STD_STRING(susynclib()->GetPrimaryModName( i ));
      if ( name == cmp ) return i;
    }
  } catch (...) {}
  return -1;
}
*/

bool SpringUnitSync::ModExists( const std::string& modname )
{
  wxLogDebugFunc( _T("modname = \"") + WX_STRING(modname) + _T("\"") );
  try {
    return GetModIndex( modname ) >=0;
  } catch (...) {}
  return false;
}


UnitSyncMod SpringUnitSync::GetMod( const std::string& modname )
{
  wxLogDebugFunc( _T("modname = \"") + WX_STRING(modname) + _T("\"") );
  UnitSyncMod m;

  int i = susynclib()->GetPrimaryModIndex( WX_STRING(modname) );
  return GetMod( i );
}


UnitSyncMod SpringUnitSync::GetMod( int index )
{
  wxLogDebugFunc( _T("") );
  UnitSyncMod m;

  m.name = STD_STRING(susynclib()->GetPrimaryModName( index ));
  m.hash = i2s(susynclib()->GetPrimaryModChecksum( index ));

  return m;
}


int SpringUnitSync::GetNumMaps()
{
  wxLogDebugFunc( _T("") );
  return susynclib()->GetMapCount();
}


bool SpringUnitSync::MapExists( const std::string& mapname )
{
  wxLogDebugFunc( _T("") );
  try {
    return GetMapIndex( mapname ) >= 0;
  } catch (...) {}
  return false;
}


bool SpringUnitSync::MapExists( const std::string& mapname, const std::string hash )
{
  wxLogDebugFunc( _T("") );
  int index;
  std::string usynchash;
  try {
    index = GetMapIndex( mapname );
    if ( index >= 0 ) {
      usynchash = STD_STRING( susynclib()->GetMapChecksum( index ) );
      return ( usynchash == hash );
    }
  } catch (...) {}
  return false;
}


UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname )
{
  wxLogDebugFunc( _T("") );
  int i = GetMapIndex( mapname );
  return GetMap( i );
}

/*
>>>>>>> usync_refac:src/springunitsync.cpp
MapInfo SpringUnitSync::_GetMapInfoEx( const std::string& mapname )
{
  wxLogDebugFunc( _T("") );
  MapCacheType::iterator i = m_mapinfo.find(mapname);
  if ( i != m_mapinfo.end() ) {
    wxLogMessage( _T("GetMapInfoEx cache lookup.") );
    MapInfo info;
    CachedMapInfo cinfo = i->second;
    _ConvertSpringMapInfo( cinfo, info );
    return info;
  }

  wxLogMessage( _T("GetMapInfoEx cache lookup failed.") );

  char tmpdesc[256];
  char tmpauth[256];

  SpringMapInfo tm;
  tm.description = &tmpdesc[0];
  tm.author = &tmpauth[0];

  tm = susynclib()->GetMapInfoEx( WX_STRING(mapname), 0 );

  MapInfo info;
  _ConvertSpringMapInfo( tm, info );

  CachedMapInfo cinfo;
  _ConvertSpringMapInfo( tm, cinfo, mapname );
  m_mapinfo[mapname] = cinfo;

  return info;
}
*/

UnitSyncMap SpringUnitSync::GetMap( int index )
{
  wxLogDebugFunc( _T("") );
  UnitSyncMap m;
  m.name = STD_STRING(susynclib()->GetMapName( index ));
  m.hash = STD_STRING(susynclib()->GetMapChecksum( index ));
  return m;
}


UnitSyncMap SpringUnitSync::GetMapEx( int index )
{
  UnitSyncMap m;

  m.name = STD_STRING(susynclib()->GetMapName( index ));
  m.hash = STD_STRING(susynclib()->GetMapChecksum( index ));

  m.info = susynclib()->GetMapInfoEx( WX_STRING(m.name), 1 );

  return m;
}


UnitSyncMap SpringUnitSync::GetMapEx( const std::string& mapname )
{
  wxLogDebugFunc( _T("") );
  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, _T("Map does not exist") );
  return GetMapEx( i );
}


int SpringUnitSync::GetMapIndex( const std::string& name )
{
  try {
    int count = susynclib()->GetMapCount();
    for ( int i = 0; i < count; i++ ) {
      std::string cmp = STD_STRING(susynclib()->GetMapName( i ));
      if ( name == cmp )
        return i;
    }
  } catch(...) {}
  return -1;
}


std::string SpringUnitSync::GetModArchive( int index )
{
  wxLogDebugFunc( _T("") );
  LOCK_UNITSYNC;

  return _GetModArchive( index );
}


std::string SpringUnitSync::_GetModArchive( int index )
{
  return STD_STRING(susynclib()->GetPrimaryModArchive( index ));
}


int SpringUnitSync::GetSideCount( const std::string& modname )
{
  wxLogDebugFunc( _T("") );
  if ( !ModExists( modname ) ) return 0;
  return susynclib()->GetSideCount( WX_STRING(modname) );
}


std::string SpringUnitSync::GetSideName( const std::string& modname, int index )
{
  wxLogDebugFunc( _T("") );

  if ( (index < 0) || (!ModExists( modname )) ) return "unknown";
  susynclib()->AddAllArchives( WX_STRING(_GetModArchive( susynclib()->GetModIndex( WX_STRING(modname) )  ) ) );
  if ( index >= GetSideCount( modname ) ) return "unknown";
  ASSERT_LOGIC( GetSideCount( modname ) > index, _T("Side index too high.") );
  return STD_STRING(susynclib()->GetSideName( WX_STRING(modname), index ));
}


wxImage SpringUnitSync::GetSidePicture( const std::string& modname, const std::string& SideName )
{
  wxLogDebugFunc( _T("") );

  susynclib()->AddAllArchives( WX_STRING(_GetModArchive( susynclib()->GetModIndex( WX_STRING(modname) ) ) ) );
  wxLogDebugFunc( _T("SideName = \"") + WX_STRING(SideName) + _T("\"") );
  wxString ImgName = _T("SidePics");
  ImgName += _T("/");
  ImgName += WX_STRING( SideName ).Upper();
  ImgName += _T(".bmp");

  int ini = susynclib()->OpenFileVFS (ImgName );
  ASSERT_RUNTIME( ini, _T("cannot find side image") );

  int FileSize = susynclib()->FileSizeVFS(ini);
  if (FileSize == 0) {
    susynclib()->CloseFileVFS(ini);
    ASSERT_RUNTIME( FileSize, _T("side image has size 0") );
  }

  char* FileContent = new char [FileSize];
  susynclib()->ReadFileVFS(ini, FileContent, FileSize);
  wxMemoryInputStream FileContentStream( FileContent, FileSize );

  wxImage ret( FileContentStream, wxBITMAP_TYPE_ANY, -1);
  delete[] FileContent;
  return ret;
}


wxArrayString SpringUnitSync::GetAIList()
{
  wxLogDebugFunc( _T("") );

  int ini = susynclib()->InitFindVFS( _T("AI/Bot-libs/*") + wxString(DLL_EXTENSION) );

  wxArrayString ret;
  wxString FileName;

  ini = susynclib()->FindFilesVFS( ini, FileName );
  while ( ini ) {
    if ( ret.Index( FileName.BeforeLast( '/') ) == wxNOT_FOUND ) ret.Add ( FileName ); // don't add duplicates
    ini = susynclib()->FindFilesVFS( ini, FileName );
  }

  if(susynclib()->HasLuaAI()){
  try { // Older versions of unitsync does not have these functions.
    const int LuaAICount = susynclib()->GetLuaAICount();
    for ( int i = 0; i < LuaAICount; i++ ) ret.Add( _( "LuaAI" ) +  susynclib()->GetLuaAIName( i ) );
  } catch (...) {}
  }

  return ret;
}


int SpringUnitSync::GetNumUnits( const std::string& modname )
{
  wxLogDebugFunc( _T("") );

  susynclib()->AddAllArchives( susynclib()->GetPrimaryModArchive( susynclib()->GetModIndex( WX_STRING(modname) ) ) );
  susynclib()->ProcessUnitsNoChecksum();

  return susynclib()->GetUnitCount();
}


wxString _GetCachedModUnitsFileName( const wxString& mod )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mod );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  wxLogMessage( path );
  return path + fname + _T(".units");
}


wxArrayString SpringUnitSync::GetUnitsList( const std::string& modname )
{
  wxLogDebugFunc( _T("") );

  if ( m_mod_units.GetCount() > 0 ) return m_mod_units;

  wxArrayString ret;

  wxString path = _GetCachedModUnitsFileName( WX_STRING( modname ) );
  try {

    ASSERT_RUNTIME( wxFileName::FileExists( path ), _T("Cache file does not exist") );
    wxTextFile f;
    ASSERT_RUNTIME( f.Open(path), _T("Failed to open file") );
    ASSERT_RUNTIME( f.GetLineCount() > 0, _T("File empty") );

    wxString str;
    for ( str = f.GetFirstLine(); !f.Eof(); str = f.GetNextLine() ) ret.Add( str );

    return ret;

  } catch(...) {}

  susynclib()->AddAllArchives( (susynclib()->GetPrimaryModArchive( susynclib()->GetModIndex( WX_STRING(modname) ) )) );
  while ( susynclib()->ProcessUnitsNoChecksum() );
  for ( int i = 0; i < susynclib()->GetUnitCount(); i++ ) {
    wxString tmp = susynclib()->GetUnitName(i) + _T("(");
    tmp += susynclib()->GetFullUnitName(i) + _T(")");
    ret.Add( tmp );
  }

  m_mod_units = ret;
  try {

    wxFile f( path, wxFile::write );
    ASSERT_RUNTIME( f.IsOpened(), _T("Couldn't create file") );

    for ( unsigned int i = 0; i < ret.GetCount(); i++ ) {
      std::string tmp = STD_STRING( ret.Item(i) );
      tmp += "\n";
      f.Write( tmp.c_str(), tmp.length() );
    }

    f.Close();

  } catch(...) {}

  return ret;
}


wxString SpringUnitSync::_GetCachedMinimapFileName( const std::string& mapname, int width, int height )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mapname );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  if ( width != -1 ) fname += wxString::Format( _T("%dx%d"), width, height );
  fname += _T(".png");
  return path + fname;
}


wxImage SpringUnitSync::_GetCachedMinimap( const std::string& mapname, int max_w, int max_h, bool store_size )
{
  wxString fname = store_size? _GetCachedMinimapFileName( mapname, max_w, max_h ) : _GetCachedMinimapFileName( mapname );
  ASSERT_RUNTIME( wxFileExists( fname ), _T("File cached image does not exist") );

  wxImage img( fname, wxBITMAP_TYPE_PNG );
  ASSERT_RUNTIME( img.Ok(), _T("Failed to load chache image") );

  if ( !store_size ) {

    UnitSyncMap map = GetMap( mapname );
    if ( map.hash != m_map.hash ) map = m_map = GetMapEx( mapname );
    else map = m_map;

    int height, width;

    width = max_w;
    height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
    if ( height > max_h ) {
      width = (int)((double)((double)width * (double)max_h) / (double)height);
      height = max_h;
    }

    img.Rescale( width, height );

  }

  return img;
}


wxImage SpringUnitSync::GetMinimap( const std::string& mapname, int max_w, int max_h, bool store_size )
{
  wxLogDebugFunc( _T("") );
  CacheMinimap( WX_STRING(mapname) );
  return _GetCachedMinimap( mapname, max_w, max_h, store_size );
}

MapInfo SpringUnitSync::_GetMapInfoEx( const std::string& mapname, bool force )
{
  //debug_func("");
  /*MapCacheType::iterator i = m_mapinfo.find(mapname);
  if ( i != m_mapinfo.end() ) {
    if ( i->second.is_info_cached ) {
      debug("GetMapInfoEx cache lookup.");
      MapInfo info;
      MapCacheItem cinfo = i->second;
      _ConvertSpringMapInfo( cinfo, info );
      return info;
>>>>>>> usync_refac:src/springunitsync.cpp
    }
  }*/

  //ASSERT_RUNTIME( force, "GetMapInfoEx cache lookup failed." );
  wxLogMessage( _T("GetMapInfoEx cache lookup failed.") );

  MapInfo info = susynclib()->GetMapInfoEx( WX_STRING(mapname), 0 );

  //CachedMapInfo cinfo;
  //_ConvertSpringMapInfo( tm, cinfo, mapname, i2s(susynclib()->GetMapChecksum( _GetMapIndex( mapname ) )) );
  //_AddMapCacheInfo( cinfo, mapname );

  return info;
}


bool SpringUnitSync::CacheMapInfo( const wxString& map )
{
  return false;
}


bool SpringUnitSync::CacheMinimap( const wxString& mapname )
{
  wxLogDebug( _T("") );
  if ( wxFileExists( _GetCachedMinimapFileName( STD_STRING(mapname) ) ) &&
       wxFileExists( _GetCachedMinimapFileName( STD_STRING(mapname), 160, 160 ) ) &&
       wxFileExists( _GetCachedMinimapFileName( STD_STRING(mapname), 98, 98 ) )
     ) return false;

  int width = 512, height = 1024;

  wxImage ret;
  try {
    ret = susynclib()->GetMinimap( mapname );
  } catch (...) {
    return false;
  }

  UnitSyncMap map;
  try {
    map = GetMapEx( STD_STRING(mapname) );
  } catch(...) {
    return false;
  }

  ret.Rescale( 512, 512 );
  wxString fname = _GetCachedMinimapFileName( STD_STRING(mapname) );
  if ( !wxFileExists( fname ) ) ret.SaveFile( fname, wxBITMAP_TYPE_PNG );

  int max_w, max_h;
  for ( int i = 0; i <= 1; i++ ) {

    switch ( i ) {
      case 0: max_w = 160; max_h = 160; break;
      case 1: max_w = 98; max_h = 98; break;
    };

    width = max_w;
    height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
    if ( height > max_h ) {
      width = (int)((double)((double)width * (double)max_h) / (double)height);
      height = max_h;
    }

    ret.Rescale( width, height );
    ret.SaveFile( _GetCachedMinimapFileName( STD_STRING(mapname), max_w, max_h ), wxBITMAP_TYPE_PNG );

  }
  return true;
}


bool SpringUnitSync::CacheModUnits( const wxString& mod )
{
  return false;
}


bool SpringUnitSync::ReloadUnitSyncLib()
{
  return false;
}


void SpringUnitSync::_ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out )
{
  out.author = in.author;
  out.description = in.description;

  out.extractorRadius = in.extractorRadius;
  out.gravity = in.gravity;
  out.tidalStrength = in.tidalStrength;
  out.maxMetal = in.maxMetal;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;
  out.posCount = in.posCount;
  for ( int i = 0; i < in.posCount; i++) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const std::string& mapname )
{
  strncpy( &out.name[0], mapname.c_str(), 256 );
  strncpy( &out.author[0], in.author, 256 );
  strncpy( &out.description[0], in.description, 256 );

  out.tidalStrength = in.tidalStrength;
  out.gravity = in.gravity;
  out.maxMetal = in.maxMetal;
  out.extractorRadius = in.extractorRadius;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;

  out.posCount = in.posCount;
  for ( int i = 0; i < 16; i++ ) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_LoadMapInfoExCache()
{
  wxLogDebugFunc( _T("") );

  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  if ( !wxFileName::FileExists( path ) ) {
    wxLogMessage( _T("No cache file found.") );
    return;
  }

  wxFile f( path.c_str(), wxFile::read );
  if ( !f.IsOpened() ) {
    wxLogMessage( _T("failed to open file for reading.") );
    return;
  }

  m_mapinfo.clear();

  CachedMapInfo cinfo;
  while ( !f.Eof() ) {
    if ( (unsigned int)f.Read( &cinfo, sizeof(CachedMapInfo) ) < sizeof(CachedMapInfo) ) {
      wxLogError( _T("Cache file invalid") );
      m_mapinfo.clear();
      break;
    }
    m_mapinfo[ std::string( &cinfo.name[0] ) ] = cinfo;
  }
  f.Close();
}


void SpringUnitSync::_SaveMapInfoExCache()
{
  wxLogDebugFunc( _T("") );
  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  wxFile f( path.c_str(), wxFile::write );
  if ( !f.IsOpened() ) {
    wxLogMessage( _T("failed to open file for writing.") );
    return;
  }

  MapCacheType::iterator i = m_mapinfo.begin();
  while ( i != m_mapinfo.end() ) {
    f.Write( &i->second, sizeof(CachedMapInfo) );
    i++;
  }
  f.Close();
}


