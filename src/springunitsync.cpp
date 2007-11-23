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


struct SpringMapInfo
{
  char* description;
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

  char* author;
};


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


struct UnitSyncColour {
  unsigned int b : 5;
  unsigned int g : 6;
  unsigned int r : 5;
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
  debug_func("");
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
  debug_func( "" );
  susynclib()->Unload();
}


bool SpringUnitSync::IsLoaded()
{
  return susynclib()->IsLoaded();
}


std::string SpringUnitSync::GetSpringVersion()
{
  debug_func("");
  return STD_STRING(susynclib()->GetSpringVersion());
}


int SpringUnitSync::GetNumMods()
{
  debug_func("");
  return susynclib()->GetPrimaryModCount();
}


int SpringUnitSync::GetModIndex( const std::string& name )
{
  debug_func( "name = \"" + name + "\"" );
  return _GetModIndex( name );
}


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


bool SpringUnitSync::ModExists( const std::string& modname )
{
  debug_func( "modname = \"" + modname + "\"" );
  try {
    return susynclib()->GetPrimaryModIndex( WX_STRING(modname) ) >= 0;
  } catch (...) {}
  return false;
}


UnitSyncMod SpringUnitSync::GetMod( const std::string& modname )
{
  debug_func( "modname = \"" + modname + "\"" );
  UnitSyncMod m;

  int i = susynclib()->GetPrimaryModIndex( WX_STRING(modname) );
  return GetMod( i );
}


UnitSyncMod SpringUnitSync::GetMod( int index )
{
  debug_func( "" );
  UnitSyncMod m;

  m.name = STD_STRING(susynclib()->GetPrimaryModName( index ));
  m.hash = i2s(susynclib()->GetPrimaryModChecksum( index ));
}


int SpringUnitSync::GetNumMaps()
{
  debug_func( "" );
  return susynclib()->GetMapCount();
}


bool SpringUnitSync::MapExists( const std::string& mapname )
{
  debug_func( "" );
  try {
    return GetMapIndex( mapname ) >= 0;
  } catch (...) {}
  return false;
}


bool SpringUnitSync::MapExists( const std::string& mapname, const std::string hash )
{
  debug_func( "" );
  try {
    int i = GetMapIndex( mapname );
    if ( i >= 0 ) {
      return ( i2s(susynclib()->GetMapChecksum( i )) == hash );
    }
  } catch (...) {}
  return false;
}


UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname, bool getmapinfo )
{
  int i = GetMapIndex( mapname );
  return GetMap( i, getmapinfo );
}


MapInfo SpringUnitSync::_GetMapInfoEx( const std::string& mapname )
{
  debug_func("");
  MapCacheType::iterator i = m_mapinfo.find(mapname);
  if ( i != m_mapinfo.end() ) {
    debug("GetMapInfoEx cache lookup.");
    MapInfo info;
    CachedMapInfo cinfo = i->second;
    _ConvertSpringMapInfo( cinfo, info );
    return info;
  }

  debug("GetMapInfoEx cache lookup failed.");

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


UnitSyncMap SpringUnitSync::GetMap( int index, bool getmapinfo )
{
  UnitSyncMap m;
  m.name = STD_STRING(susynclib()->GetMapName( index ));
  m.hash = i2s(susynclib()->GetMapChecksum( index ));
  if ( getmapinfo ) m.info = _GetMapInfoEx( m.name );
  return m;
}


int SpringUnitSync::_GetMapIndex( const std::string& name )
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
  debug_func( "" );
  LOCK_UNITSYNC;

  return _GetModArchive( index );
}


std::string SpringUnitSync::_GetModArchive( int index )
{
  return STD_STRING(susynclib()->GetPrimaryModArchive( index ));
}


void SpringUnitSync::SetCurrentMod( const std::string& modname )
{
  debug_func( "" );
  LOCK_UNITSYNC;

  debug_func("modname = \"" + modname + "\"" );
  if ( m_current_mod != modname ) {
    m_uninit();
    m_init( true, 1 );
    m_add_all_archives( _GetModArchive( _GetModIndex( modname ) ).c_str() );
    m_current_mod = modname;
    m_side_count = m_get_side_count();
    m_mod_units.Clear();
  }
}


int SpringUnitSync::GetSideCount()
{
  debug_func( "" );
  LOCK_UNITSYNC;

  if ( (!m_loaded) || (!_ModExists(m_current_mod)) ) return 0;
  return m_side_count;
}


std::string SpringUnitSync::GetSideName( int index )
{
  debug_func( "" );
  LOCK_UNITSYNC;

  if ( (!m_loaded) || (index < 0) || (!_ModExists(m_current_mod)) ) return "unknown";
  m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
  if ( index >= m_side_count ) return "unknown";
  ASSERT_LOGIC( m_side_count > index, "Side index too high." );
  return m_get_side_name( index );
}


wxImage SpringUnitSync::GetSidePicture( const std::string& SideName )
{
  debug_func( "" );
  LOCK_UNITSYNC;

  m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
  debug_func( "SideName = \"" + SideName + "\"" );
  wxString ImgName = _T("SidePics");
  ImgName += _T("/");
  ImgName += WX_STRING( SideName ).Upper();
  ImgName += _T(".bmp");

  int ini = m_open_file_vfs(STD_STRING(ImgName).c_str());
  ASSERT_RUNTIME( ini, "cannot find side image" );

  int FileSize = m_file_size_vfs(ini);
  if (FileSize == 0) {
    m_close_file_vfs(ini);
    ASSERT_RUNTIME( FileSize, "side image has size 0" );
  }

  char* FileContent = new char [FileSize];
  m_read_file_vfs(ini, FileContent, FileSize);
  wxMemoryInputStream FileContentStream( FileContent, FileSize );

  wxImage ret( FileContentStream, wxBITMAP_TYPE_ANY, -1);
  delete[] FileContent;
  return ret;
}


wxArrayString SpringUnitSync::GetAIList()
{
  debug_func( "" );
  LOCK_UNITSYNC;

  if ( !m_loaded ) return wxArrayString();

  int ini = m_init_find_vfs ( "AI/Bot-libs/*" );
  int BufferSize = 400;
  char * FilePath = new char [BufferSize];
  wxArrayString ret;

  do
  {
    ini = m_find_files_vfs ( ini, FilePath, BufferSize );
    wxString FileName = wxString ( FilePath, wxConvUTF8 );
    FileName = FileName.AfterLast ( wxFileName::GetPathSeparator() ); // strip the file path
    if ( !FileName.Contains ( _T(".dll") ) && !FileName.Contains (  _T(".so") ) ) continue; // FIXME this isn't exactly portable
    FileName = FileName.SubString(0, FileName.Find( '.', true ) - 1 ); //strip the file extension
    if ( ret.Index( FileName ) == wxNOT_FOUND ) ret.Add ( FileName ); // don't add duplicates
  } while (ini != 0);

  return ret;
}


wxString SpringUnitSync::GetBotLibPath( const wxString& botlibname )
{
  debug_func( "" );
  LOCK_UNITSYNC;

  if ( !m_loaded ) return wxEmptyString;

  debug_func( "botlibname = \"" + STD_STRING(botlibname) + "\"" );
  wxString search = _T("AI/Bot-libs/") + botlibname + _T("*");
  int ini = m_init_find_vfs ( search.mb_str( wxConvUTF8 ) );
  int BufferSize = 400;
  char * FilePath = new char [BufferSize];

  do
  {
    ini = m_find_files_vfs ( ini, FilePath, BufferSize );
    wxString FileName = wxString( FilePath, wxConvUTF8 );
    if ( !FileName.Contains ( _T(".dll") ) && !FileName.Contains (  _T(".so") ) ) continue; // FIXME this isn't exactly portable
    debug( "AIdll: " + STD_STRING(FileName) );
    return FileName;
  } while (ini != 0);

  return wxEmptyString;
}


int SpringUnitSync::GetNumUnits()
{
  debug_func( "" );
  LOCK_UNITSYNC;

  if ( !m_loaded ) return 0;
  m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
  m_proc_units_nocheck();

  return m_get_unit_count();
}


wxString _GetCachedModUnitsFileName( const wxString& mod )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mod );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  debug(STD_STRING(path));
  return path + fname + _T(".units");
}


wxArrayString SpringUnitSync::GetUnitsList()
{
  debug_func( "" );
  LOCK_UNITSYNC;

  if (!m_loaded) return wxArrayString();
  if ( m_mod_units.GetCount() > 0 ) return m_mod_units;

  wxArrayString ret;
  if (!m_loaded) return ret;

  wxString path = _GetCachedModUnitsFileName( WX_STRING(m_current_mod) );
  try {

    ASSERT_RUNTIME( wxFileName::FileExists( path ), "Cache file does not exist" );
    wxTextFile f;
    ASSERT_RUNTIME( f.Open(path), "Failed to open file" );
    ASSERT_RUNTIME( f.GetLineCount() > 0, "File empty" );

    wxString str;
    for ( str = f.GetFirstLine(); !f.Eof(); str = f.GetNextLine() ) ret.Add( str );

    return ret;

  } catch(...) {}

  m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
  while ( m_proc_units_nocheck() );
  for ( int i = 0; i < m_get_unit_count(); i++ ) {
    wxString tmp = wxString(m_get_unit_full_name(i), wxConvUTF8) + _T("(");
    tmp += wxString(m_get_unit_name(i), wxConvUTF8) + _T(")");
    ret.Add( tmp );
  }

  m_mod_units = ret;
  try {

    wxFile f( path, wxFile::write );
    ASSERT_RUNTIME( f.IsOpened(), "Couldn't create file" );

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
  ASSERT_RUNTIME( wxFileExists( fname ), "File cached image does not exist" );

  wxImage img( fname, wxBITMAP_TYPE_PNG );
  ASSERT_RUNTIME( img.Ok(), "Failed to load chache image" );

  if ( !store_size ) {

    UnitSyncMap map = _GetMap( mapname );
    if ( map.hash != m_map.hash ) map = m_map = _GetMap( mapname, true );
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
  debug_func( "" );
  LOCK_UNITSYNC;

  int height = 1024;
  int width = 512;

  try {
    return _GetCachedMinimap( mapname, max_w, max_h, store_size );
  } catch(...) {
    debug( "Cache lookup failed." );
  }

  wxImage ret( width, height );
  UnitSyncColour* colours = (UnitSyncColour*)m_get_minimap( mapname.c_str(), 0 );
  ASSERT_RUNTIME( colours , "GetMinimap failed" );
  for ( int y = 0; y < height; y++ ) {
    for ( int x = 0; x < width; x++ ) {
      int pos = y*(width)+x;
      typedef unsigned char uchar;
      ret.SetRGB( x, y, uchar((colours[pos].r/31.0)*255.0), uchar((colours[pos].g/63.0)*255.0), uchar((colours[pos].b/31.0)*255.0) );
    }
  }


  UnitSyncMap map = _GetMap( mapname, true );

  width = max_w;
  height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
  if ( height > max_h ) {
    width = (int)((double)((double)width * (double)max_h) / (double)height);
    height = max_h;
  }

  ret.Rescale( 512, 512 );

  wxString fname = _GetCachedMinimapFileName( mapname );
  if ( !wxFileExists( fname ) ) ret.SaveFile( fname, wxBITMAP_TYPE_PNG );

  ret.Rescale( width, height );

  if ( store_size ) {
    ret.SaveFile( _GetCachedMinimapFileName( mapname, max_w, max_h ), wxBITMAP_TYPE_PNG );
  }

  return ret;
}


void SpringUnitSync::_ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out )
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
  debug_func("");

  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  if ( !wxFileName::FileExists( path ) ) {
    debug( "No cache file found." );
    return;
  }

  wxFile f( path.c_str(), wxFile::read );
  if ( !f.IsOpened() ) {
    debug( "failed to open file for reading." );
    return;
  }

  m_mapinfo.clear();

  CachedMapInfo cinfo;
  while ( !f.Eof() ) {
    if ( (unsigned int)f.Read( &cinfo, sizeof(CachedMapInfo) ) < sizeof(CachedMapInfo) ) {
      debug_error( "Cache file invalid" );
      m_mapinfo.clear();
      break;
    }
    m_mapinfo[ std::string( &cinfo.name[0] ) ] = cinfo;
  }
  f.Close();
}


void SpringUnitSync::_SaveMapInfoExCache()
{
  debug_func("");
  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  wxFile f( path.c_str(), wxFile::write );
  if ( !f.IsOpened() ) {
    debug( "failed to open file for writing." );
    return;
  }

  MapCacheType::iterator i = m_mapinfo.begin();
  while ( i != m_mapinfo.end() ) {
    f.Write( &i->second, sizeof(CachedMapInfo) );
    i++;
  }
  f.Close();
}



