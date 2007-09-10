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

#include <stdexcept>

#include "springunitsync.h"
#include "utils.h"
#include "settings.h"


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
    m_sync = new SpringUnitSync; // FIXME: This is a memleak, where to delete?
  return m_sync;
}

void* SpringUnitSync::_GetLibFuncPtr( const std::string& name )
{
  ASSERT_LOGIC( m_loaded, "Unitsync not loaded" );
  void* ptr = m_libhandle->GetSymbol(WX_STRING(name));
  ASSERT_RUNTIME( ptr, "Couldn't load " + name + " from unitsync library" );
  return ptr;
}

bool SpringUnitSync::LoadUnitSyncLib()
{
  if ( m_loaded ) return true;

  wxSetWorkingDirectory( WX_STRING(sett().GetSpringDir()) );

  // Load the library.
  std::string loc = sett().GetUnitSyncUsedLoc();

  debug( "Loading from: " + loc );

  // Check if library exists
  if ( !wxFileName::FileExists( WX_STRING(loc)) ) {
    debug_error( "File not found: "+ loc );
    return false;
  }

  try {
    m_libhandle = new wxDynamicLibrary(WX_STRING(loc));
    if (!m_libhandle->IsLoaded()) {
      debug_error("wxDynamicLibrary created, but not loaded!");
      delete m_libhandle;
      m_libhandle = 0;
    }
  } catch(...) {
    m_libhandle = 0;
  }

  if (m_libhandle == 0) {
    debug_error( "Couldn't load the unitsync library" );
    return false;
  }

  m_loaded = true;

  // Load all function from library.
  try {
    m_init = (InitPtr)_GetLibFuncPtr("Init");
    m_uninit = (UnInitPtr)_GetLibFuncPtr("UnInit");

    m_get_map_count = (GetMapCountPtr)_GetLibFuncPtr("GetMapCount");
    m_get_map_checksum = (GetMapChecksumPtr)_GetLibFuncPtr("GetMapChecksum");
    m_get_map_name = (GetMapNamePtr)_GetLibFuncPtr("GetMapName");
    m_get_map_info_ex = (GetMapInfoExPtr)_GetLibFuncPtr("GetMapInfoEx");
    m_get_minimap = (GetMinimapPtr)_GetLibFuncPtr("GetMinimap");
    m_get_mod_checksum = (GetPrimaryModChecksumPtr)_GetLibFuncPtr("GetPrimaryModChecksum");
    m_get_mod_index = (GetPrimaryModIndexPtr)_GetLibFuncPtr("GetPrimaryModIndex");
    m_get_mod_name = (GetPrimaryModNamePtr)_GetLibFuncPtr("GetPrimaryModName");
    m_get_mod_count = (GetPrimaryModCountPtr)_GetLibFuncPtr("GetPrimaryModCount");
    m_get_mod_archive = (GetPrimaryModArchivePtr)_GetLibFuncPtr("GetPrimaryModArchive");

    m_get_side_count = (GetSideCountPtr)_GetLibFuncPtr("GetSideCount");
    m_get_side_name = (GetSideNamePtr)_GetLibFuncPtr("GetSideName");

    m_add_all_archives = (AddAllArchivesPtr)_GetLibFuncPtr("AddAllArchives");

    m_get_unit_count = (GetUnitCountPtr)_GetLibFuncPtr("GetUnitCount");
    m_get_unit_name = (GetUnitNamePtr)_GetLibFuncPtr("GetUnitName");
    m_get_unit_full_name = (GetFullUnitNamePtr)_GetLibFuncPtr("GetFullUnitName");
    m_proc_units_nocheck = (ProcessUnitsNoChecksumPtr)_GetLibFuncPtr("ProcessUnitsNoChecksum");

    m_init_find_vfs = (InitFindVFSPtr)_GetLibFuncPtr("InitFindVFS");
    m_find_files_vfs = (FindFilesVFSPtr)_GetLibFuncPtr("FindFilesVFS");
    m_open_file_vfs = (OpenFileVFSPtr)_GetLibFuncPtr("OpenFileVFS");
    m_file_size_vfs = (FileSizeVFSPtr)_GetLibFuncPtr("FileSizeVFS");
    m_read_file_vfs = (ReadFileVFSPtr)_GetLibFuncPtr("ReadFileVFS");
    m_close_file_vfs = (CloseFileVFSPtr)_GetLibFuncPtr("CloseFileVFS");

    m_get_spring_version = (GetSpringVersionPtr)_GetLibFuncPtr("GetSpringVersion");

    m_init( true, 1 );
    m_map_count = m_get_map_count();
    m_mod_count = m_get_mod_count();
    m_current_mod = "";
    m_side_count = 0;
  }
  catch ( ... ) {
    debug_error( "Failed to load Unitsync lib." );
    FreeUnitSyncLib();
    return false;
  }

  return true;
}


void SpringUnitSync::FreeUnitSyncLib()
{
  debug_func( "" );
  if ( !m_loaded ) return;
  m_uninit();

  delete m_libhandle;
  m_libhandle = 0;

  m_loaded = false;
}


bool SpringUnitSync::IsLoaded()
{
  return m_loaded;
}


std::string SpringUnitSync::GetSpringVersion()
{
  if ( !m_loaded ) return "";
  std::string SpringVersion = m_get_spring_version();
  return SpringVersion;
}


int SpringUnitSync::GetNumMods()
{
  if ( !m_loaded ) return 0;
  return m_mod_count;
}


int SpringUnitSync::GetModIndex( const std::string& name )
{
  //debug_func( "name = \"" + name + "\"" );
  if ( !m_loaded ) return -1;
  for ( int i = 0; i < m_mod_count; i++ ) {
    std::string cmp = m_get_mod_name( i );
    if ( name == cmp ) return i;
  }
  //debug_warn( "Mod not found." );
  return -1;
}


bool SpringUnitSync::ModExists( const std::string& modname )
{
  if ( !m_loaded ) return false;
  return GetModIndex( modname ) >= 0;
}


UnitSyncMod SpringUnitSync::GetMod( const std::string& modname )
{
  UnitSyncMod m;
  if ( !m_loaded ) return m;

  int i = GetModIndex( modname );
  ASSERT_LOGIC( i >= 0, "Mod does not exist" );

  return GetMod( i );
}


UnitSyncMod SpringUnitSync::GetMod( int index )
{
  UnitSyncMod m;

  m.name = m_get_mod_name( index );
  m.hash = i2s(m_get_mod_checksum( index ));

  return m;
}


int SpringUnitSync::GetNumMaps()
{
  if ( !m_loaded ) return 0;
  return m_map_count;
}


bool SpringUnitSync::MapExists( const std::string& mapname )
{
  if ( !m_loaded ) return false;
  bool succ = false;
  try {
    succ = GetMapIndex( mapname ) >= 0;
  } catch (...) { return false; }
  return succ;
}


bool SpringUnitSync::MapExists( const std::string& mapname, const std::string hash )
{
  if ( !m_loaded ) return false;
  try {
    int i = GetMapIndex( mapname );
    if ( i >= 0 ) {
      return ( i2s(m_get_map_checksum( i )) == hash );
    }
  } catch (...) {}
  return false;
}


UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname, bool getmapinfo )
{
  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );

  return GetMap( i, getmapinfo );
}


MapInfo SpringUnitSync::_GetMapInfoEx( const std::string& mapname )
{
  MapCacheType::iterator i = m_mapinfo.find(mapname);
  if ( i != m_mapinfo.end() ) {
    debug("GetMapInfoEx cache lookup.");
    MapInfo info;
    CachedMapInfo cinfo = i->second;
    ConvertSpringMapInfo( cinfo, info );
    return info;
  }

  debug("GetMapInfoEx cache lookup failed.");

  char tmpdesc[256];
  char tmpauth[256];

  SpringMapInfo tm;
  tm.description = &tmpdesc[0];
  tm.author = &tmpauth[0];

  m_get_map_info_ex( mapname.c_str(), &tm, 0 );

  MapInfo info;
  ConvertSpringMapInfo( tm, info );

  CachedMapInfo cinfo;
  ConvertSpringMapInfo( tm, cinfo, mapname );
  m_mapinfo[mapname] = cinfo;

  return info;
}


UnitSyncMap SpringUnitSync::GetMap( int index, bool getmapinfo )
{
  UnitSyncMap m;
  if ( !m_loaded ) return m;

  m.name = m_get_map_name( index );
  m.hash = i2s(m_get_map_checksum( index ));

  if ( getmapinfo ) m.info = _GetMapInfoEx( m.name );

  return m;
}


int SpringUnitSync::GetMapIndex( const std::string& name )
{
  if ( !m_loaded ) return -1;
  for ( int i = 0; i < m_map_count; i++ ) {
    std::string cmp = m_get_map_name( i );
    if ( name == cmp )
      return i;
  }
  return -1;
}


std::string SpringUnitSync::GetModArchive( int index )
{
  if ( (!m_loaded) || (index < 0) ) return "unknown";
  ASSERT_LOGIC( index < m_mod_count, "Bad index" );
  std::string arch = m_get_mod_archive( index );
  return arch;
}


void SpringUnitSync::SetCurrentMod( const std::string& modname )
{
  debug_func("modname = \"" + modname + "\"" );
  if ( m_current_mod != modname ) {
    FreeUnitSyncLib();
    LoadUnitSyncLib();
    //m_uninit();
    //m_init( true, 1 );
    m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
    m_current_mod = modname;
    m_side_count = m_get_side_count();
  }
}


int SpringUnitSync::GetSideCount()
{
  if ( (!m_loaded) || (!ModExists(m_current_mod)) ) return 0;
  //m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
  return m_side_count;
}


std::string SpringUnitSync::GetSideName( int index )
{
  //debug_func("curmod = \"" + m_current_mod + "\" side = " + i2s(index) );
  if ( (!m_loaded) || (index < 0) || (!ModExists(m_current_mod)) ) return "unknown";
  m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
  if ( index >= m_side_count ) return "unknown";
  ASSERT_LOGIC( m_side_count > index, "Side index too high." );
  return m_get_side_name( index );
}


wxImage SpringUnitSync::GetSidePicture( const std::string& SideName )
{
  m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
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
  if ( !m_loaded ) return 0;
  m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
  m_proc_units_nocheck();
  return m_get_unit_count();
}


int SpringUnitSync::GetUnitIndex( const std::string& name )
{
  if ( !m_loaded ) return -1;
  m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
  m_proc_units_nocheck();
  for ( int i = 0; i < m_get_unit_count(); i++ ) {
    if ( m_get_unit_name( i ) == name ) return i;
  }
  return -1;
}


std::string SpringUnitSync::GetFullUnitName( int index )
{
  if ( (!m_loaded) || (index < 0) ) return "unknown";
  m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
  while ( m_proc_units_nocheck() );
  return m_get_unit_full_name( index );
}


wxArrayString SpringUnitSync::GetUnitsList()
{
  wxArrayString ret;
  if (!m_loaded) return ret;
  m_add_all_archives( GetModArchive( GetModIndex( m_current_mod ) ).c_str() );
  while ( m_proc_units_nocheck() );
  for ( int i = 0; i < m_get_unit_count(); i++ ) {
    wxString tmp = wxString(m_get_unit_full_name(i), wxConvUTF8) + _T("(");
    tmp += wxString(m_get_unit_name(i), wxConvUTF8) + _T(")");
    ret.Add( tmp );
  }
  return ret;
}


wxString SpringUnitSync::GetCachedMinimapFileName( const std::string& mapname, int width, int height )
{
  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mapname );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  if ( width != -1 ) fname += wxString::Format( _T("%dx%d"), width, height );
  fname += _T(".png");
  return path + fname;
}


wxImage SpringUnitSync::GetCachedMinimap( const std::string& mapname, int max_w, int max_h, bool store_size )
{
  wxString fname = store_size? GetCachedMinimapFileName( mapname, max_w, max_h ) : GetCachedMinimapFileName( mapname );
  ASSERT_RUNTIME( wxFileExists( fname ), "File cached image does not exist" );

  wxImage img( fname, wxBITMAP_TYPE_PNG );
  ASSERT_RUNTIME( img.Ok(), "Failed to load chache image" );

  if ( !store_size ) {

    UnitSyncMap map = usync()->GetMap( mapname );
    if ( map.hash != m_map.hash ) map = m_map = usync()->GetMap( mapname, true );
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
  int height = 1024;
  int width = 512;

  try {
    return GetCachedMinimap( mapname, max_w, max_h, store_size );
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


  UnitSyncMap map = GetMap( mapname, true );

  width = max_w;
  height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
  if ( height > max_h ) {
    width = (int)((double)((double)width * (double)max_h) / (double)height);
    height = max_h;
  }

  ret.Rescale( 512, 512 );

  wxString fname = GetCachedMinimapFileName( mapname );
  if ( !wxFileExists( fname ) ) ret.SaveFile( fname, wxBITMAP_TYPE_PNG );

  ret.Rescale( width, height );

  if ( store_size ) {
    ret.SaveFile( GetCachedMinimapFileName( mapname, max_w, max_h ), wxBITMAP_TYPE_PNG );
  }

  return ret;
}


void SpringUnitSync::ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out )
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


void SpringUnitSync::ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out )
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


void SpringUnitSync::ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const std::string& mapname )
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

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

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
    if ( f.Read( &cinfo, sizeof(CachedMapInfo) ) < sizeof(CachedMapInfo) ) {
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
  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

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



