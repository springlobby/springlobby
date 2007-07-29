#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/string.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <cassert>
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
  assert( m_loaded );
  void* ptr = 0;
  ptr = MY_GET_PROC_ADDR( m_libhandle, name.c_str() );
  ASSERT_RUNTIME( ptr != 0, "Couldn't load " + name + " from unitsync library" );
  return ptr;
}

bool SpringUnitSync::LoadUnitSyncLib()
{
  if ( m_loaded ) return true;

  wxSetWorkingDirectory( WX_STRING(sett().GetSpringDir()) );

  // Load the library.
  std::string loc;
  if ( sett().GetUnitSyncUseDefLoc() ) loc = sett().GetSpringDir() + dllname;
  else loc = sett().GetUnitSyncLoc();

  debug( "Loading from: " + loc );

  try {
    m_libhandle = MY_LOAD_LIBRARY( loc.c_str() );
  } catch(...) {
    m_libhandle = 0;
  }

  if (m_libhandle == 0) {
    debug_error( "Couldn't load the unitsync library" );
    std::string dlerr = MY_DLERROR();
    debug_error( dlerr );

    wxMessageDialog msg( 0, _("The unitsync library failed to load from the location \"") + WX_STRING(loc) + _("\".\nIt failed with the error message \"") + WX_STRING(dlerr)+ _("\".\n\nYou might want to look at the Spring Options again. If you need any help setting unitsync up you will find it under the Help main menu."), _("Error loading unitsync"), wxOK | wxICON_ERROR );

    msg.ShowModal();

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

    m_init( true, 1 );
  }
  catch ( std::runtime_error& e ) {
    debug_error( e.what() );
    FreeUnitSyncLib();
    return false;
  }

  return true;
}

void SpringUnitSync::FreeUnitSyncLib()
{
  if ( !m_loaded ) return;
  m_uninit();

  MY_FREELIBRARY(m_libhandle);

  m_loaded = false;
}

bool SpringUnitSync::IsLoaded()
{
  return m_loaded;
}

int SpringUnitSync::GetNumMods()
{
  if ( !m_loaded ) return 0;
  return m_get_mod_count();
}

int SpringUnitSync::GetModIndex( const std::string& name )
{
  if ( !m_loaded ) return -1;
  for ( int i = 0; i < m_get_mod_count(); i++ ) {
    std::string cmp = m_get_mod_name( i );
    if ( name == cmp ) return i;
  }
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

  m.name = m_get_mod_name( i );
  m.hash = i2s(m_get_mod_checksum( i ));

  return m;
}

int SpringUnitSync::GetNumMaps()
{
  if ( !m_loaded ) return 0;
  return m_get_map_count();
}

bool SpringUnitSync::MapExists( const std::string& mapname )
{
  if ( !m_loaded ) return false;
  return GetMapIndex( mapname ) >= 0;
}

bool SpringUnitSync::MapExists( const std::string& mapname, const std::string hash )
{
  if ( !m_loaded ) return false;
  int i = GetMapIndex( mapname );
  if ( i >= 0 ) {
    return ( i2s(m_get_map_checksum( i )) == hash );
  }
  return false;
}

UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname, bool getmapinfo )
{
  UnitSyncMap m;
  if ( !m_loaded ) return m;

  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );

  if ( getmapinfo ) {
    char tmpdesc[245+1];
    char tmpauth[200+1];

    SpringMapInfo tm;
    tm.description = &tmpdesc[0];
    tm.author = &tmpauth[0];

    m_get_map_info_ex( mapname.c_str(), &tm, 0 );

    ConvertSpringMapInfo( tm, m.info );
  }

  m.name = m_get_map_name( i );
  m.hash = i2s(m_get_map_checksum( i ));
  return m;
}

int SpringUnitSync::GetMapIndex( const std::string& name )
{
  if ( !m_loaded ) return -1;
  int mc = m_get_map_count();
  for ( int i = 0; i < mc; i++ ) {
    std::string cmp = m_get_map_name( i );
    if ( name == cmp )
      return i;
  }
  return -1;
}

std::string SpringUnitSync::GetModArchive( int index )
{
  if ( (!m_loaded) || (index < 0) ) return "unknown";
  ASSERT_LOGIC( index < m_get_mod_count(), "Bad index" );
  return m_get_mod_archive( index );
}

int SpringUnitSync::GetSideCount( const std::string& modname )
{
  if ( (!m_loaded) || (!ModExists(modname)) ) return 0;
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  return m_get_side_count();
}

std::string SpringUnitSync::GetSideName( const std::string& modname, int index )
{
  if ( (!m_loaded) || (index < 0) || (!ModExists(modname)) ) return "unknown";
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  ASSERT_LOGIC( m_get_side_count() > index, "Side index too high." );
  return m_get_side_name( index );
}

wxImage SpringUnitSync::GetSidePicture(const std::string& SideName )
{
  wxString ImgName = _("SidePics");
  ImgName += _("/");
  ImgName += WX_STRING( SideName ).Upper();
  ImgName += _(".bmp");
  int ini = m_open_file_vfs(ImgName.fn_str());
  ASSERT_RUNTIME( ini != 0, "cannot find side image" );
  int FileSize = m_file_size_vfs(ini);
  if (FileSize == 0){
       m_close_file_vfs(ini);
       ASSERT_RUNTIME( FileSize != 0 , "side image has size 0" );
  }
  char* FileContent;
  FileContent = new char [FileSize];
  m_read_file_vfs(ini, FileContent, FileSize);
  wxMemoryInputStream FileContentStream( FileContent, FileSize );
//  delete[] FileContent;
  return wxImage( FileContentStream, wxBITMAP_TYPE_ANY, -1);
}

int SpringUnitSync::GetNumUnits( const std::string& modname )
{
  if ( !m_loaded ) return 0;
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  m_proc_units_nocheck();
  return m_get_unit_count();
}

int SpringUnitSync::GetUnitIndex( const std::string& modname, const std::string& name )
{
  if ( !m_loaded ) return -1;
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  m_proc_units_nocheck();
  for ( int i = 0; i < m_get_unit_count(); i++ ) {
    if ( m_get_unit_name( i ) == name ) return i;
  }
  return -1;
}

std::string SpringUnitSync::GetFullUnitName( const std::string& modname, int index )
{
  if ( (!m_loaded) || (index < 0) ) return "unknown";
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  m_proc_units_nocheck();
  return m_get_unit_full_name( index );
}

wxString SpringUnitSync::GetCachedMinimapFileName( const std::string& mapname, int size )
{
  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mapname );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  fname += wxString::Format( _T("_%dx%d.bmp"), size, size );
  return path + fname;
}

wxImage SpringUnitSync::GetCachedMinimap( const std::string& mapname, int size )
{
  wxString fname = GetCachedMinimapFileName( mapname, size );

  ASSERT_RUNTIME( wxFileExists( fname ), "File cached image does not exist" );

  wxImage img( fname, wxBITMAP_TYPE_BMP );
  ASSERT_RUNTIME( img.Ok(), "Failed to load chache image" );
  return img;
}


wxImage SpringUnitSync::GetMinimap( const std::string& mapname, int size )
{
  int height = 1024;
  int width = 512;

  try {
    return GetCachedMinimap( mapname, size );
  } catch(...) {}

  wxImage ret( width, height );
  UnitSyncColour* colours = (UnitSyncColour*)m_get_minimap( mapname.c_str(), 0 );
  ASSERT_RUNTIME( colours != NULL, "GetMinimap failed" );
  for ( int y = 0; y < height; y++ ) {
    for ( int x = 0; x < width; x++ ) {
      int pos = y*(width)+x;
      typedef unsigned char uchar;
      ret.SetRGB( x, y, uchar((colours[pos].r/31.0)*255.0), uchar((colours[pos].g/63.0)*255.0), uchar((colours[pos].b/31.0)*255.0) );
    }
  }


  UnitSyncMap map = GetMap( mapname, true );

  if ( map.info.width >= map.info.height ) {
    width = size;
    height = (double)size * (double)( (double)map.info.height / (double)map.info.width );
  } else {
    width = (double)size * (double)( (double)map.info.width / (double)map.info.height );
    height = size;
  }

  ret.Rescale( width, height );

  wxString fname = GetCachedMinimapFileName( mapname, size );
  ret.SaveFile( fname, wxBITMAP_TYPE_BMP );

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


