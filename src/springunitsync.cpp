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
#include "unitsyncthread.h"


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


void* SpringUnitSync::_GetLibFuncPtr( const std::string& name )
{
  ASSERT_LOGIC( m_loaded, "Unitsync not loaded" );
  void* ptr = m_libhandle->GetSymbol(WX_STRING(name));
  ASSERT_RUNTIME( ptr, "Couldn't load " + name + " from unitsync library" );
  return ptr;
}


bool SpringUnitSync::LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  debug_func("");
  LOCK_UNITSYNC;
  return _LoadUnitSyncLib( springdir, unitsyncloc );
}


bool SpringUnitSync::_LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  if ( m_loaded ) return true;

  wxSetWorkingDirectory( springdir );

  // Load the library.
  std::string loc = STD_STRING(unitsyncloc);

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
    m_last_index_mod = "";
  }
  catch ( ... ) {
    debug_error( "Failed to load Unitsync lib." );
    _FreeUnitSyncLib();
    return false;
  }

  return true;
}


void SpringUnitSync::FreeUnitSyncLib()
{
  debug_func( "" );
  LOCK_UNITSYNC;
  _FreeUnitSyncLib();
}


void SpringUnitSync::_FreeUnitSyncLib()
{
  if ( !m_loaded ) return;
  m_uninit();

  delete m_libhandle;
  m_libhandle = 0;

  m_loaded = false;
}


bool SpringUnitSync::IsLoaded()
{
  //LOCK_UNITSYNC;
  return m_loaded;
}


std::string SpringUnitSync::GetSpringVersion()
{
  debug_func("");
  //LOCK_UNITSYNC;
  if ( !m_loaded ) return "";
  std::string SpringVersion = m_get_spring_version();
  debug("Version reported by unitsync: " + SpringVersion );
  return SpringVersion;
}


int SpringUnitSync::GetNumMods()
{
  //debug_func("");
  //LOCK_UNITSYNC;
  if ( !m_loaded ) return 0;
  return m_mod_count;
}


int SpringUnitSync::GetModIndex( const std::string& name )
{
  //debug_func( "name = \"" + name + "\"" );
  //LOCK_UNITSYNC;
  return _GetModIndex( name );
}


int SpringUnitSync::_GetModIndex( const std::string& name )
{
  if ( !m_loaded ) return -1;

  if ( m_last_index_mod != "" ) {
    if ( m_last_index_mod == name ) return m_last_mod_index;
  }

  m_last_index_mod = name;
  for ( int i = 0; i < m_mod_count; i++ ) {
    std::string cmp = m_get_mod_name( i );
    if ( name == cmp ) {
      m_last_mod_index = i;
      return i;
    }
  }

  m_last_mod_index = -1;
  return -1;
}


bool SpringUnitSync::ModExists( const std::string& modname )
{
  //debug_func( "modname = \"" + modname + "\"" );
  //LOCK_UNITSYNC;
  return _ModExists( modname );
}


bool SpringUnitSync::_ModExists( const std::string& modname )
{
  if ( !m_loaded ) return false;

  if ( m_last_index_mod != "" ) {
    if ( m_last_index_mod == modname ) return m_last_mod_index >= 0;
  }

  return _GetModIndex( modname ) >= 0;
}


UnitSyncMod SpringUnitSync::GetMod( const std::string& modname )
{
  //debug_func( "modname = \"" + modname + "\"" );
  //LOCK_UNITSYNC;

  UnitSyncMod m;
  if ( !m_loaded ) return m;

  int i = _GetModIndex( modname );
  ASSERT_LOGIC( i >= 0, "Mod does not exist" );

  return _GetMod( i );
}


UnitSyncMod SpringUnitSync::GetMod( int index )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;
  return _GetMod( index );
}


UnitSyncMod SpringUnitSync::_GetMod( int index )
{
  UnitSyncMod m;

  m.name = m_get_mod_name( index );
  m.hash = i2s(m_get_mod_checksum( index ));

  return m;
}


int SpringUnitSync::GetNumMaps()
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  if ( !m_loaded ) return 0;
  return m_map_count;
}


bool SpringUnitSync::MapExists( const std::string& mapname )
{
  debug_func( "" );
  //LOCK_UNITSYNC;

  if ( !m_loaded ) return false;
  bool succ = false;
  try {
    succ = _GetMapIndex( mapname ) >= 0;
  } catch (...) { return false; }
  return succ;
}


bool SpringUnitSync::MapExists( const std::string& mapname, const std::string hash )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  if ( !m_loaded ) return false;
  try {
    int i = _GetMapIndex( mapname );
    if ( i >= 0 ) {
      return ( i2s(m_get_map_checksum( i )) == hash );
    }
  } catch (...) {}
  return false;
}


UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  return _GetMap( mapname );
}


UnitSyncMap SpringUnitSync::GetMapEx( const std::string& mapname, bool& cached )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  return _GetMapEx( mapname, cached );
}


UnitSyncMap SpringUnitSync::_GetMap( const std::string& mapname )
{
  int i = _GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );

  return _GetMap( i );
}


UnitSyncMap SpringUnitSync::_GetMapEx( const std::string& mapname, bool& cached, bool force )
{
  int i = _GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );

  return _GetMapEx( i, cached, force );
}


MapInfo SpringUnitSync::_GetMapInfoEx( const std::string& mapname, bool& cached, bool force )
{
  //debug_func("");
  MapCacheType::iterator i = m_mapinfo.find(mapname);
  if ( i != m_mapinfo.end() ) {
    debug("GetMapInfoEx cache lookup.");
    MapInfo info;
    CachedMapInfo cinfo = i->second;
    _ConvertSpringMapInfo( cinfo, info );
    cached = true;
    return info;
  }

  cached = false;

  debug("GetMapInfoEx cache lookup failed.");

  char tmpdesc[256];
  char tmpauth[256];

  MapInfo info;

  if ( force ) {
    SpringMapInfo tm;
    tm.description = &tmpdesc[0];
    tm.author = &tmpauth[0];

    m_get_map_info_ex( mapname.c_str(), &tm, 0 );

    _ConvertSpringMapInfo( tm, info );

    CachedMapInfo cinfo;
    _ConvertSpringMapInfo( tm, cinfo, mapname );
    m_mapinfo[mapname] = cinfo;
  }

  return info;
}


UnitSyncMap SpringUnitSync::GetMap( int index )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  return _GetMap( index );
}


UnitSyncMap SpringUnitSync::GetMapEx( int index, bool& cached )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  return _GetMapEx( index, cached );
}


UnitSyncMap SpringUnitSync::_GetMap( int index )
{
  UnitSyncMap m;
  if ( !m_loaded ) return m;

  m.name = m_get_map_name( index );
  m.hash = i2s(m_get_map_checksum( index ));

  return m;
}


UnitSyncMap SpringUnitSync::_GetMapEx( int index, bool& cached, bool force )
{
  UnitSyncMap m;
  if ( !m_loaded ) return m;

  m.name = m_get_map_name( index );
  m.hash = i2s(m_get_map_checksum( index ));

  m.info = _GetMapInfoEx( m.name, cached, force );

  return m;
}


int SpringUnitSync::GetMapIndex( const std::string& name )
{
  //debug_func( "" );
  //LOCK_UNITSYNC;

  return _GetMapIndex( name );
}


int SpringUnitSync::_GetMapIndex( const std::string& name )
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
  //debug_func( "" );
  //LOCK_UNITSYNC;

  return _GetModArchive( index );
}


std::string SpringUnitSync::_GetModArchive( int index )
{
  if ( (!m_loaded) || (index < 0) ) return "unknown";
  ASSERT_LOGIC( index < m_mod_count, "Bad index" );

  return m_get_mod_archive( index );;
}


void SpringUnitSync::SetCurrentMod( const std::string& modname )
{
  debug_func( "" );
  //LOCK_UNITSYNC;

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
  //LOCK_UNITSYNC;

  if ( (!m_loaded) || (!_ModExists(m_current_mod)) ) return 0;
  return m_side_count;
}


std::string SpringUnitSync::GetSideName( int index )
{
  debug_func( "" );
  //LOCK_UNITSYNC;

  if ( (!m_loaded) || (index < 0) || (!_ModExists(m_current_mod)) ) return "unknown";
  //m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
  if ( index >= m_side_count ) return "unknown";
  ASSERT_LOGIC( m_side_count > index, "Side index too high." );
  return m_get_side_name( index );
}


wxImage SpringUnitSync::GetSidePicture( const std::string& SideName )
{
  debug_func( "" );
 // LOCK_UNITSYNC;

  //m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
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
  //LOCK_UNITSYNC;

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
  //LOCK_UNITSYNC;

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
  //LOCK_UNITSYNC;

  if ( !m_loaded ) return 0;
  //m_add_all_archives( _GetModArchive( _GetModIndex( m_current_mod ) ).c_str() );
  m_proc_units_nocheck();

  return m_get_unit_count();
}


wxString _GetCachedModUnitsFileName( const wxString& mod )
{
  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mod );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  return path + fname + _T(".units");
}


wxArrayString SpringUnitSync::GetUnitsList()
{
  debug_func( "" );
  //LOCK_UNITSYNC;

  if (!m_loaded) return wxArrayString();
  if ( m_mod_units.GetCount() > 0 ) return m_mod_units;

  wxArrayString ret;
  if (!m_loaded) return ret;

  wxString path = _GetCachedModUnitsFileName( WX_STRING(m_current_mod) );
  try {

    ASSERT_RUNTIME( wxFileName::FileExists( path ), "Cache file does not exist" );
    wxTextFile f;
    ASSERT_RUNTIME( f.Open(path), "Failed to open file" );

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
  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
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

    bool cached = true;
    /*UnitSyncMap map = _GetMap( mapname );
    if ( map.hash != m_map.hash ) map = m_map = _GetMapEx( mapname, cached );
    else map = m_map;*/
    UnitSyncMap map = _GetMapEx( mapname, cached );
    if ( !cached ) {
      map.info.height = 512;
      map.info.width = 512;
    }

    debug("MWidth: " + i2s(map.info.width) + " MHeight: " + i2s(map.info.height) );

    int height, width;

    width = max_w;
    height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
    if ( height > max_h ) {
      width = (int)((double)((double)width * (double)max_h) / (double)height);
      height = max_h;
    }

    debug("Width: " + i2s(width) + " Height: " + i2s(height) );
    img.Rescale( width, height );

  }

  return img;
}


wxImage SpringUnitSync::GetMinimap( const std::string& mapname, int max_w, int max_h, bool store_size, bool& cached )
{
  debug_func( "" );
  //LOCK_UNITSYNC;

/*  int height = 1024;
  int width = 512;*/

  cached = true;

  try {
    return _GetCachedMinimap( mapname, max_w, max_h, store_size );
  } catch(...) {
    debug( "Cache lookup failed." );
  }

  cached = false;


/*  UnitSyncColour* colours = (UnitSyncColour*)m_get_minimap( mapname.c_str(), 0 );
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
*/
  return wxImage();
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


bool SpringUnitSync::CacheMapInfo( const wxString& mapname )
{
  debug_func( "" );

  MapCacheType::iterator i = m_mapinfo.find(STD_STRING(mapname));
  if ( i != m_mapinfo.end() ) return false;

  LOCK_UNITSYNC;
  if ( !m_loaded) return false;

  char tmpdesc[256];
  char tmpauth[256];

  SpringMapInfo tm;
  tm.description = &tmpdesc[0];
  tm.author = &tmpauth[0];

  m_get_map_info_ex( mapname.mb_str(wxConvUTF8), &tm, 0 );

  MapInfo info;
  _ConvertSpringMapInfo( tm, info );

  CachedMapInfo cinfo;
  _ConvertSpringMapInfo( tm, cinfo, STD_STRING(mapname) );
  m_mapinfo[STD_STRING(mapname)] = cinfo;

  return true;
}


bool SpringUnitSync::CacheMinimap( const wxString& mapname )
{
  debug_func( "" );
  if ( wxFileExists( _GetCachedMinimapFileName( STD_STRING(mapname) ) ) &&
       wxFileExists( _GetCachedMinimapFileName( STD_STRING(mapname), 160, 160 ) ) &&
       wxFileExists( _GetCachedMinimapFileName( STD_STRING(mapname), 98, 98 ) )
     ) return false;

  LOCK_UNITSYNC;
  if ( !m_loaded) return false;

  int width = 512, height = 1024;

  wxImage ret( width, height );
  UnitSyncColour* colours;
  try {
    colours = (UnitSyncColour*)m_get_minimap( mapname.mb_str(wxConvUTF8), 0 );
  } catch(...) { return false; }

  for ( int y = 0; y < height; y++ ) {
    for ( int x = 0; x < width; x++ ) {
      int pos = y*(width)+x;
      typedef unsigned char uchar;
      ret.SetRGB( x, y, uchar((colours[pos].r/31.0)*255.0), uchar((colours[pos].g/63.0)*255.0), uchar((colours[pos].b/31.0)*255.0) );
    }
  }

  UnitSyncMap map;
  try {
    bool cached;
    map = _GetMapEx( STD_STRING(mapname), cached, true );
  } catch(...) {}

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
  debug_func( "" );
  LOCK_UNITSYNC;
  return false;
}











