/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/filename.h>
#include <wx/dynlib.h>
#include <wx/image.h>
#include <stdexcept>

#include "springunitsynclib.h"
#include "utils.h"

#define LOCK_UNITSYNC wxCriticalSectionLocker lock_criticalsection(m_lock)

SpringUnitSyncLib::SpringUnitSyncLib( const wxString& path ):
  m_loaded(false),
  m_path(wxEmptyString)
{
  if ( path != wxEmptyString ) Load( path );
}


SpringUnitSyncLib::~SpringUnitSyncLib()
{
  Unload();
}


SpringUnitSyncLib* susynclib()
{
  static SpringUnitSyncLib lib;
  return &lib;
}


void SpringUnitSyncLib::Load( const wxString& path )
{
  LOCK_UNITSYNC;

  _Unload();

  m_path = path;

  // Load the library.
  wxLogMessage( _T("Loading from: %s"), path.c_str());

  // Check if library exists
  if ( !wxFileName::FileExists( path ) ) {
    wxLogError( _T("File not found: %s"), path.c_str() );
    ASSERT_EXCEPTION( false, _T("Failed to load Unitsync lib.") );
  }

  {
    wxLog *currentarget = wxLog::GetActiveTarget();
    wxLog *templogger = new wxLogGui();
    wxLog::SetActiveTarget( templogger );
    try {
  #ifdef __WXMSW__
      wxSetWorkingDirectory( path.BeforeLast('\\') );
  #endif
      m_libhandle = new wxDynamicLibrary( path );
      if ( !m_libhandle->IsLoaded() ) {
        delete m_libhandle;
        m_libhandle = 0;
      }
    } catch(...) {
      m_libhandle = 0;
    }
    wxLog::SetActiveTarget( currentarget );
    delete templogger;
  }

  ASSERT_EXCEPTION( m_libhandle != 0, _T("Couldn't load the unitsync library") );

  m_loaded = true;

  // Load all function from library.
  try {
    m_init = (InitPtr)_GetLibFuncPtr(_T("Init"));
    m_uninit = (UnInitPtr)_GetLibFuncPtr(_T("UnInit"));

    m_get_map_count = (GetMapCountPtr)_GetLibFuncPtr(_T("GetMapCount"));
    m_get_map_checksum = (GetMapChecksumPtr)_GetLibFuncPtr(_T("GetMapChecksum"));
    m_get_map_name = (GetMapNamePtr)_GetLibFuncPtr(_T("GetMapName"));
    m_get_map_info_ex = (GetMapInfoExPtr)_GetLibFuncPtr(_T("GetMapInfoEx"));
    m_get_minimap = (GetMinimapPtr)_GetLibFuncPtr(_T("GetMinimap"));
    m_get_infomap_size = (GetInfoMapSizePtr)_GetLibFuncPtr(_T("GetInfoMapSize"));
    m_get_infomap = (GetInfoMapPtr)_GetLibFuncPtr(_T("GetInfoMap"));

    m_get_mod_checksum = (GetPrimaryModChecksumPtr)_GetLibFuncPtr(_T("GetPrimaryModChecksum"));
    m_get_mod_index = (GetPrimaryModIndexPtr)_GetLibFuncPtr(_T("GetPrimaryModIndex"));
    m_get_mod_name = (GetPrimaryModNamePtr)_GetLibFuncPtr(_T("GetPrimaryModName"));
    m_get_mod_count = (GetPrimaryModCountPtr)_GetLibFuncPtr(_T("GetPrimaryModCount"));
    m_get_mod_archive = (GetPrimaryModArchivePtr)_GetLibFuncPtr(_T("GetPrimaryModArchive"));

    m_get_side_count = (GetSideCountPtr)_GetLibFuncPtr(_T("GetSideCount"));
    m_get_side_name = (GetSideNamePtr)_GetLibFuncPtr(_T("GetSideName"));

    m_add_all_archives = (AddAllArchivesPtr)_GetLibFuncPtr(_T("AddAllArchives"));

    m_get_unit_count = (GetUnitCountPtr)_GetLibFuncPtr(_T("GetUnitCount"));
    m_get_unit_name = (GetUnitNamePtr)_GetLibFuncPtr(_T("GetUnitName"));
    m_get_unit_full_name = (GetFullUnitNamePtr)_GetLibFuncPtr(_T("GetFullUnitName"));
    m_proc_units_nocheck = (ProcessUnitsNoChecksumPtr)_GetLibFuncPtr(_T("ProcessUnitsNoChecksum"));

    m_init_find_vfs = (InitFindVFSPtr)_GetLibFuncPtr(_T("InitFindVFS"));
    m_find_files_vfs = (FindFilesVFSPtr)_GetLibFuncPtr(_T("FindFilesVFS"));
    m_open_file_vfs = (OpenFileVFSPtr)_GetLibFuncPtr(_T("OpenFileVFS"));
    m_file_size_vfs = (FileSizeVFSPtr)_GetLibFuncPtr(_T("FileSizeVFS"));
    m_read_file_vfs = (ReadFileVFSPtr)_GetLibFuncPtr(_T("ReadFileVFS"));
    m_close_file_vfs = (CloseFileVFSPtr)_GetLibFuncPtr(_T("CloseFileVFS"));

    m_get_spring_version = (GetSpringVersionPtr)_GetLibFuncPtr(_T("GetSpringVersion"));

    m_process_units = (ProcessUnitsPtr)_GetLibFuncPtr(_T("ProcessUnits"));
    m_add_archive = (AddArchivePtr)_GetLibFuncPtr(_T("AddArchive"));
    m_get_archive_checksum = (GetArchiveChecksumPtr)_GetLibFuncPtr(_T("GetArchiveChecksum"));
    m_get_archive_path = (GetArchivePathPtr)_GetLibFuncPtr(_T("GetArchivePath"));

    m_get_map_archive_count = (GetMapArchiveCountPtr)_GetLibFuncPtr(_T("GetMapArchiveCount"));
    m_get_map_archive_name = (GetMapArchiveNamePtr)_GetLibFuncPtr(_T("GetMapArchiveName"));
    m_get_map_checksum = (GetMapChecksumPtr)_GetLibFuncPtr(_T("GetMapChecksum"));
    m_get_map_checksum_from_name = (GetMapChecksumFromNamePtr)_GetLibFuncPtr(_T("GetMapChecksumFromName"));

    m_get_primary_mod_short_name = (GetPrimaryModShortNamePtr)_GetLibFuncPtr(_T("GetPrimaryModShortName"));
    m_get_primary_mod_version = (GetPrimaryModVersionPtr)_GetLibFuncPtr(_T("GetPrimaryModVersion"));
    m_get_primary_mod_mutator = (GetPrimaryModMutatorPtr)_GetLibFuncPtr(_T("GetPrimaryModMutator"));
    m_get_primary_mod_game = (GetPrimaryModGamePtr)_GetLibFuncPtr(_T("GetPrimaryModGame"));
    m_get_primary_mod_short_game = (GetPrimaryModShortGamePtr)_GetLibFuncPtr(_T("GetPrimaryModShortGame"));
    m_get_primary_mod_description = (GetPrimaryModDescriptionPtr)_GetLibFuncPtr(_T("GetPrimaryModDescription"));
    m_get_primary_mod_archive = (GetPrimaryModArchivePtr)_GetLibFuncPtr(_T("GetPrimaryModArchive"));
    m_get_primary_mod_archive_count = (GetPrimaryModArchiveCountPtr)_GetLibFuncPtr(_T("GetPrimaryModArchiveCount"));
    m_get_primary_mod_archive_list = (GetPrimaryModArchiveListPtr)_GetLibFuncPtr(_T("GetPrimaryModArchiveList"));
    m_get_primary_mod_checksum_from_name = (GetPrimaryModChecksumFromNamePtr)_GetLibFuncPtr(_T("GetPrimaryModChecksumFromName"));

    m_get_luaai_count = (GetLuaAICountPtr)_GetLibFuncPtr(_T("GetLuaAICount"));
    m_get_luaai_name = (GetLuaAINamePtr)_GetLibFuncPtr(_T("GetLuaAIName"));
    m_get_luaai_desc = (GetLuaAIDescPtr)_GetLibFuncPtr(_T("GetLuaAIDesc"));

    m_get_map_option_count = (GetMapOptionCountPtr)_GetLibFuncPtr(_T("GetMapOptionCount"));
    m_get_Mod_option_count = (GetModOptionCountPtr)_GetLibFuncPtr(_T("GetModOptionCount"));
    m_get_option_key = (GetOptionKeyPtr)_GetLibFuncPtr(_T("GetOptionKey"));
    m_get_option_name = (GetOptionNamePtr)_GetLibFuncPtr(_T("GetOptionName"));
    m_get_option_desc = (GetOptionDescPtr)_GetLibFuncPtr(_T("GetOptionDesc"));
    m_get_option_type = (GetOptionTypePtr)_GetLibFuncPtr(_T("GetOptionType"));
    m_get_option_bool_def = (GetOptionBoolDefPtr)_GetLibFuncPtr(_T("GetOptionBoolDef"));
    m_get_option_number_def = (GetOptionNumberDefPtr)_GetLibFuncPtr(_T("GetOptionNumberDef"));
    m_get_option_number_min = (GetOptionNumberMinPtr)_GetLibFuncPtr(_T("GetOptionNumberMin"));
    m_get_option_number_max = (GetOptionNumberMaxPtr)_GetLibFuncPtr(_T("GetOptionNumberMax"));
    m_get_option_number_step = (GetOptionNumberStepPtr)_GetLibFuncPtr(_T("GetOptionNumberStep"));
    m_get_option_string_def = (GetOptionStringDefPtr)_GetLibFuncPtr(_T("GetOptionStringDef"));
    m_get_option_string_max_len = (GetOptionStringMaxLenPtr)_GetLibFuncPtr(_T("GetOptionStringMaxLen"));
    m_get_option_list_count = (GetOptionListCountPtr)_GetLibFuncPtr(_T("GetOptionListCount"));
    m_get_option_list_def = (GetOptionListDefPtr)_GetLibFuncPtr(_T("GetOptionListDef"));
    m_get_option_list_item_key = (GetOptionListItemKeyPtr)_GetLibFuncPtr(_T("GetOptionListItemKey"));
    m_get_option_list_item_name = (GetOptionListItemNamePtr)_GetLibFuncPtr(_T("GetOptionListItemName"));
    m_get_option_list_item_desc = (GetOptionListItemDescPtr)_GetLibFuncPtr(_T("GetOptionListItemDesc"));

    m_open_archive = (OpenArchivePtr)_GetLibFuncPtr(_T("OpenArchive"));
    m_close_archive = (CloseArchivePtr)_GetLibFuncPtr(_T("CloseArchive"));
    m_find_Files_archive = (FindFilesArchivePtr)_GetLibFuncPtr(_T("FindFilesArchive"));
    m_open_archive_file = (OpenArchiveFilePtr)_GetLibFuncPtr(_T("OpenArchiveFile"));
    m_read_archive_file = (ReadArchiveFilePtr)_GetLibFuncPtr(_T("ReadArchiveFile"));
    m_close_archive_file = (CloseArchiveFilePtr)_GetLibFuncPtr(_T("CloseArchiveFile"));
    m_size_archive_file = (SizeArchiveFilePtr)_GetLibFuncPtr(_T("SizeArchiveFile"));

    m_set_spring_config_float = (SetSpringConfigFloatPtr)_GetLibFuncPtr(_T("SetSpringConfigFloat"));
    m_get_spring_config_float = (GetSpringConfigFloatPtr)_GetLibFuncPtr(_T("GetSpringConfigFloat"));
    m_get_spring_config_int = (GetSpringConfigIntPtr)_GetLibFuncPtr(_T("GetSpringConfigInt"));
    m_get_spring_config_string = (GetSpringConfigStringPtr)_GetLibFuncPtr(_T("GetSpringConfigString"));
    m_set_spring_config_string = (SetSpringConfigStringPtr)_GetLibFuncPtr(_T("SetSpringConfigString"));
    m_set_spring_config_int = (SetSpringConfigIntPtr)_GetLibFuncPtr(_T("SetSpringConfigInt"));

    if ( m_init ) m_init( true, 1 );
    else _Unload();

  }
  catch ( ... ) {
    _Unload();
    ASSERT_EXCEPTION( false, _T("Failed to load Unitsync lib.") );
  }

}


void SpringUnitSyncLib::Unload()
{
  LOCK_UNITSYNC;
  if ( !_IsLoaded() ) return;

  _Unload();
}


void SpringUnitSyncLib::_Unload()
{
  if ( !_IsLoaded() ) return;

  m_uninit();

  delete m_libhandle;
  m_libhandle = 0;

  m_loaded = false;
}


void SpringUnitSyncLib::Reload()
{
  Load( m_path );
}


bool SpringUnitSyncLib::IsLoaded()
{
  return m_loaded;
}


bool SpringUnitSyncLib::_IsLoaded()
{
  return m_loaded;
}


void* SpringUnitSyncLib::_GetLibFuncPtr( const wxString& name )
{
  ASSERT_LOGIC( m_libhandle != 0, _T("Unitsync not loaded") );
  if ( m_libhandle->HasSymbol( name ) ){
    void* ptr = m_libhandle->GetSymbol( name );
    if ( !ptr ) wxLogMessage( _T("Couldn't load %s from unitsync library"),name.c_str() );
    return ptr;
  }
  wxLogMessage( _T("Couldn't load %s from unitsync library"), name.c_str() );
  return 0;
}


void SpringUnitSyncLib::_ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out )
{
  out.author = WX_STRINGC(in.author);
  out.description = WX_STRINGC(in.description);

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


void SpringUnitSyncLib::SetCurrentMod( const wxString& modname )
{
  wxLogDebugFunc( _T("") );
  if ( m_current_mod != modname ) {
    m_uninit();
    m_init( true, 1 );
    AddAllArchives( GetPrimaryModArchive( GetModIndex( modname ) ) );
    m_current_mod = modname;
  }
}


int SpringUnitSyncLib::GetModIndex( const wxString& name )
{
  int count = GetPrimaryModCount();
  for ( int i = 0; i < count; i++ ) {
    if ( name == GetPrimaryModName( i ) ) return i;
  }
  return -1;
}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //  -- The UnitSync functions --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


wxString SpringUnitSyncLib::GetSpringVersion()
{
  InitLib( m_get_spring_version );

  return WX_STRINGC( m_get_spring_version() );
}

int SpringUnitSyncLib::GetMapCount()
{
  InitLib( m_get_map_count );

  return m_get_map_count();
}


wxString SpringUnitSyncLib::GetMapChecksum( int index )
{
  InitLib( m_get_map_checksum );

  return i2s( (int)m_get_map_checksum( index ) );
}


wxString SpringUnitSyncLib::GetMapName( int index )
{
  InitLib( m_get_map_name );

  return WX_STRINGC( m_get_map_name( index ) );
}


int SpringUnitSyncLib::GetMapArchiveCount( int index )
{
  InitLib( m_get_map_archive_count );

  return m_get_map_archive_count( m_get_map_name( index ) );
}


wxString SpringUnitSyncLib::GetMapArchiveName( int arnr )
{
  InitLib( m_get_map_archive_name );

  return WX_STRINGC( m_get_map_archive_name( arnr ) );
}


MapInfo SpringUnitSyncLib::GetMapInfoEx( const wxString& mapName, int version )
{
  InitLib( m_get_map_info_ex );

  char tmpdesc[256];
  char tmpauth[256];

  MapInfo info;

  SpringMapInfo tm;
  tm.description = &tmpdesc[0];
  tm.author = &tmpauth[0];

  bool result = m_get_map_info_ex( mapName.mb_str( wxConvUTF8 ), &tm, version );
  ASSERT_EXCEPTION( result, _T("Failed to get map infos") );
  _ConvertSpringMapInfo( tm, info );

  return info;
}


wxImage SpringUnitSyncLib::GetMinimap( const wxString& mapFileName )
{
  InitLib( m_get_minimap );

  const int miplevel = 0;  // miplevel should not be 10 ffs
  const int width  = 1024 >> miplevel;
  const int height = 1024 >> miplevel;

  wxLogMessage( _T("Minimap: %s"), mapFileName.c_str() );

  unsigned short* colours = (unsigned short*)m_get_minimap( mapFileName.mb_str(wxConvUTF8), miplevel );
  ///if you don't like explicit delete, feel free to make patch
  if ( colours == 0 )
  {
    delete[] colours;
    ASSERT_EXCEPTION( colours, _T("Get minimap failed") );
  }

  typedef unsigned char uchar;
  wxImage minimap(width, height, false);
  uchar* true_colours = minimap.GetData();

  for ( int i = 0; i < width*height; i++ ) {
    true_colours[(i*3)  ] = uchar( (( colours[i] >> 11 )/31.0)*255.0 );
    true_colours[(i*3)+1] = uchar( (( (colours[i] >> 5) & 63 )/63.0)*255.0 );
    true_colours[(i*3)+2] = uchar( (( colours[i] & 31 )/31.0)*255.0 );
  }

  return minimap;
}


wxImage SpringUnitSyncLib::GetMetalmap( const wxString& mapFileName )
{
  InitLib( m_get_infomap_size ); // assume GetInfoMap is available too

  wxLogMessage( _T("Metalmap: %s"), mapFileName.c_str() );

  int width = 0, height = 0, retval;

  retval = m_get_infomap_size(mapFileName.mb_str(wxConvUTF8), "metal", &width, &height);
  ASSERT_EXCEPTION( retval != 0 && width * height != 0, _T("Get metalmap size failed") );

  typedef unsigned char uchar;
  wxImage metalmap(width, height, false);
  uchar* grayscale = new uchar[width * height];
  uchar* true_colours = metalmap.GetData();

  retval = m_get_infomap(mapFileName.mb_str(wxConvUTF8), "metal", grayscale, 1 /*byte per pixel*/);
  ///if you don't like explicit delete, feel free to make patch
  if ( retval == 0 )
  {
    delete[] grayscale;
    ASSERT_EXCEPTION( retval != 0, _T("Get metalmap failed") );
  }

  for ( int i = 0; i < width*height; i++ ) {
    true_colours[(i*3)  ] = 0;
    true_colours[(i*3)+1] = grayscale[i];
    true_colours[(i*3)+2] = 0;
  }

  delete[] grayscale;

  return metalmap;
}


int SpringUnitSyncLib::GetPrimaryModChecksum( int index )
{
  InitLib( m_get_mod_checksum );

  return (int)m_get_mod_checksum( index );
}


int SpringUnitSyncLib::GetPrimaryModIndex( const wxString& modName )
{
  InitLib( m_get_mod_index );

  return m_get_mod_index( modName.mb_str( wxConvUTF8 ) );
}


wxString SpringUnitSyncLib::GetPrimaryModName( int index )
{
  InitLib( m_get_mod_name );

  return WX_STRINGC( m_get_mod_name( index ) );
}


int SpringUnitSyncLib::GetPrimaryModCount()
{
  InitLib( m_get_mod_count );

  return m_get_mod_count();
}


wxString SpringUnitSyncLib::GetPrimaryModArchive( int index )
{
  InitLib( m_get_mod_archive );

  return WX_STRINGC( m_get_mod_archive( index ) );
}


wxString SpringUnitSyncLib::GetPrimaryModShortName( int index )
{
  InitLib( m_get_primary_mod_short_name );

  return WX_STRINGC( m_get_primary_mod_short_name( index ) );
}


wxString SpringUnitSyncLib::GetPrimaryModVersion( int index )
{
  InitLib( m_get_primary_mod_version );

  return WX_STRINGC( m_get_primary_mod_version( index ) );
}


wxString SpringUnitSyncLib::GetPrimaryModMutator( int index )
{
  InitLib( m_get_primary_mod_mutator );

  return WX_STRINGC( m_get_primary_mod_mutator( index ) );
}


wxString SpringUnitSyncLib::GetPrimaryModGame( int index )
{
  InitLib( m_get_primary_mod_game );

  return WX_STRINGC( m_get_primary_mod_game( index ) );
}


wxString SpringUnitSyncLib::GetPrimaryModShortGame( int index )
{
  InitLib( m_get_primary_mod_short_game );

  return WX_STRINGC( m_get_primary_mod_short_game( index ) );
}


wxString SpringUnitSyncLib::GetPrimaryModDescription( int index )
{
  InitLib( m_get_primary_mod_description );

  return WX_STRINGC( m_get_primary_mod_description( index ) );
}


int SpringUnitSyncLib::GetPrimaryModArchiveCount( int index )
{
  InitLib( m_get_primary_mod_archive_count );

  return m_get_primary_mod_archive_count( index );
}


wxString SpringUnitSyncLib::GetPrimaryModArchiveList( int arnr )
{
  InitLib( m_get_primary_mod_archive_list );

  return WX_STRINGC( m_get_primary_mod_archive_list( arnr ) );
}


int SpringUnitSyncLib::GetPrimaryModChecksumFromName( const wxString& name )
{
  InitLib( m_get_primary_mod_checksum_from_name );

  return (int)m_get_primary_mod_checksum_from_name( name.mb_str( wxConvUTF8 ) );
}


int SpringUnitSyncLib::GetSideCount( const wxString& modName )
{
  InitLib( m_get_side_count );

  SetCurrentMod( modName );
  return m_get_side_count();
}


wxString SpringUnitSyncLib::GetSideName( const wxString& modName, int index )
{
  InitLib( m_get_side_name );

  SetCurrentMod( modName );
  return WX_STRINGC( m_get_side_name( index ) );
}


void SpringUnitSyncLib::AddAllArchives( const wxString& root )
{
  InitLib( m_add_all_archives );

  m_add_all_archives( root.mb_str() );
}


wxString SpringUnitSyncLib::GetFullUnitName( int index )
{
  InitLib( m_get_unit_full_name );

  return WX_STRINGC( m_get_unit_full_name( index ) );
}


wxString SpringUnitSyncLib::GetUnitName( int index )
{
  InitLib( m_get_unit_name );

  return WX_STRINGC( m_get_unit_name( index ) );
}


int SpringUnitSyncLib::GetUnitCount()
{
  InitLib( m_get_unit_count );

  return m_get_unit_count();
}


int SpringUnitSyncLib::ProcessUnitsNoChecksum()
{
  InitLib( m_proc_units_nocheck );

  return m_proc_units_nocheck();
}


int SpringUnitSyncLib::InitFindVFS( const wxString& pattern )
{
  InitLib( m_proc_units_nocheck );

  return m_init_find_vfs( pattern.mb_str(wxConvUTF8) );
}


int SpringUnitSyncLib::FindFilesVFS( int handle, wxString& name )
{
  InitLib( m_find_files_vfs );

  char buffer[1025];
  int ret = m_find_files_vfs( handle, &buffer[0], 1024 );
  buffer[1024] = 0;
  name = WX_STRINGC( &buffer[0] );

  return ret;
}


int SpringUnitSyncLib::OpenFileVFS( const wxString& name )
{
  InitLib( m_open_file_vfs );

  return m_open_file_vfs( name.mb_str( wxConvUTF8 ) );
}


int SpringUnitSyncLib::FileSizeVFS( int handle )
{
  InitLib( m_file_size_vfs );

  return m_file_size_vfs( handle );
}


int SpringUnitSyncLib::ReadFileVFS( int handle, void* buffer, int bufferLength )
{
  InitLib( m_read_file_vfs );

  return m_read_file_vfs( handle, buffer, bufferLength );
}


void SpringUnitSyncLib::CloseFileVFS( int handle )
{
  InitLib( m_close_file_vfs );

  m_close_file_vfs( handle );
}


int SpringUnitSyncLib::GetLuaAICount( const wxString& modname )
{
  InitLib( m_get_luaai_count );
  SetCurrentMod( modname );
  return m_get_luaai_count();
}


wxString SpringUnitSyncLib::GetLuaAIName( int aiIndex )
{
  InitLib( m_get_luaai_name );

  return WX_STRINGC(m_get_luaai_name( aiIndex ));
}


wxString SpringUnitSyncLib::GetLuaAIDesc( int aiIndex )
{
  InitLib( m_get_luaai_desc );

  return WX_STRINGC( m_get_luaai_desc( aiIndex ) );
}


int SpringUnitSyncLib::GetMapOptionCount( const wxString& name )
{
  InitLib( m_get_map_option_count );
  ASSERT_EXCEPTION( !name.IsEmpty(), _T("passing void mapname to unitsync") );
  return m_get_map_option_count( name.mb_str( wxConvUTF8 ) );
}


int SpringUnitSyncLib::GetModOptionCount( const wxString& name )
{
  InitLib( m_get_Mod_option_count );
  ASSERT_EXCEPTION( !name.IsEmpty(), _T("passing void modname to unitsync") );
  SetCurrentMod( name );
  return m_get_Mod_option_count();
}


wxString SpringUnitSyncLib::GetOptionKey( int optIndex )
{
  InitLib( m_get_option_key );

  return WX_STRINGC( m_get_option_key( optIndex ) );
}


wxString SpringUnitSyncLib::GetOptionName( int optIndex )
{
  InitLib( m_get_option_name );

  return WX_STRINGC( m_get_option_name( optIndex ) );
}


wxString SpringUnitSyncLib::GetOptionDesc( int optIndex )
{
  InitLib( m_get_option_desc );

  return WX_STRINGC( m_get_option_desc( optIndex ) );
}


int SpringUnitSyncLib::GetOptionType( int optIndex )
{
  InitLib( m_get_option_type );

  return m_get_option_type( optIndex );
}


int SpringUnitSyncLib::GetOptionBoolDef( int optIndex )
{
  InitLib( m_get_option_bool_def );

  return m_get_option_bool_def( optIndex );
}


float SpringUnitSyncLib::GetOptionNumberDef( int optIndex )
{
  InitLib( m_get_option_number_def );

  return m_get_option_number_def( optIndex );
}


float SpringUnitSyncLib::GetOptionNumberMin( int optIndex )
{
  InitLib( m_get_option_number_min );

  return m_get_option_number_min( optIndex );
}


float SpringUnitSyncLib::GetOptionNumberMax( int optIndex )
{
  InitLib( m_get_option_number_max );

  return m_get_option_number_max( optIndex );
}


float SpringUnitSyncLib::GetOptionNumberStep( int optIndex )
{
  InitLib( m_get_option_number_step );

  return m_get_option_number_step( optIndex );
}


wxString SpringUnitSyncLib::GetOptionStringDef( int optIndex )
{
  InitLib( m_get_option_string_def );

  return WX_STRINGC( m_get_option_string_def( optIndex ) );
}


int SpringUnitSyncLib::GetOptionStringMaxLen( int optIndex )
{
  InitLib( m_get_option_string_max_len );

  return m_get_option_string_max_len( optIndex );
}


int SpringUnitSyncLib::GetOptionListCount( int optIndex )
{
  InitLib( m_get_option_list_count );

  return m_get_option_list_count( optIndex );
}


wxString SpringUnitSyncLib::GetOptionListDef( int optIndex )
{
  InitLib( m_get_option_list_def );

  return WX_STRINGC( m_get_option_list_def( optIndex ) );
}


wxString SpringUnitSyncLib::GetOptionListItemKey( int optIndex, int itemIndex )
{
  InitLib( m_get_option_list_item_key );

  return WX_STRINGC( m_get_option_list_item_key( optIndex, itemIndex  ) );
}


wxString SpringUnitSyncLib::GetOptionListItemName( int optIndex, int itemIndex )
{
  InitLib( m_get_option_list_item_name );

  return WX_STRINGC( m_get_option_list_item_name( optIndex, itemIndex  ) );
}


wxString SpringUnitSyncLib::GetOptionListItemDesc( int optIndex, int itemIndex )
{
  InitLib( m_get_option_list_item_desc );

  return WX_STRINGC( m_get_option_list_item_desc( optIndex, itemIndex  ) );
}


int SpringUnitSyncLib::OpenArchive( const wxString& name )
{
  InitLib( m_open_archive );

  return m_open_archive( name.mb_str( wxConvUTF8 ) );
}


void SpringUnitSyncLib::CloseArchive( int archive )
{
  InitLib( m_close_archive );

  m_close_archive( archive );
}


int SpringUnitSyncLib::FindFilesArchive( int archive, int cur, wxString& nameBuf )
{
  InitLib( m_find_Files_archive );

  char buffer[1025];
  int size = 1024;
  bool ret = m_find_Files_archive( archive, cur, &buffer[0], &size );
  buffer[1024] = 0;
  nameBuf = WX_STRINGC( &buffer[0] );

  return ret;
}


int SpringUnitSyncLib::OpenArchiveFile( int archive, const wxString& name )
{
  InitLib( m_open_archive_file );

  return m_open_archive_file( archive, name.mb_str( wxConvUTF8 ) );
}


int SpringUnitSyncLib::ReadArchiveFile( int archive, int handle, void* buffer, int numBytes)
{
  InitLib( m_read_archive_file );

  return m_read_archive_file( archive, handle, buffer, numBytes );
}


void SpringUnitSyncLib::CloseArchiveFile( int archive, int handle )
{
  InitLib( m_close_archive_file );

  m_close_archive_file( archive, handle );
}


int SpringUnitSyncLib::SizeArchiveFile( int archive, int handle )
{
  InitLib( m_size_archive_file );

  return m_size_archive_file( archive, handle );
}


wxString SpringUnitSyncLib::GetArchivePath( const wxString& name )
{
  InitLib( m_get_archive_path );

  return WX_STRINGC( m_get_archive_path( name.mb_str( wxConvUTF8 ) ) );
}


int SpringUnitSyncLib::GetSpringConfigInt( const wxString& key, int defValue )
{
  InitLib( m_get_spring_config_int );

  return m_get_spring_config_int( key.mb_str( wxConvUTF8 ), defValue );
}


wxString SpringUnitSyncLib::GetSpringConfigString( const wxString& key, const wxString& defValue )
{
  InitLib( m_get_spring_config_string );

  return WX_STRINGC( m_get_spring_config_string( key.mb_str( wxConvUTF8 ), defValue.mb_str( wxConvUTF8 ) ) );
}


float SpringUnitSyncLib::GetSpringConfigFloat( const wxString& key, const float defValue )
{
  InitLib( m_get_spring_config_float );

  return m_get_spring_config_float( key.mb_str( wxConvUTF8 ), defValue );
}


void SpringUnitSyncLib::SetSpringConfigString( const wxString& key, const wxString& value )
{
  InitLib( m_set_spring_config_string );

  m_set_spring_config_string( key.mb_str( wxConvUTF8 ), value.mb_str( wxConvUTF8 ) );
}


void SpringUnitSyncLib::SetSpringConfigInt( const wxString& key, int value )
{
  InitLib( m_set_spring_config_int );

  m_set_spring_config_int( key.mb_str( wxConvUTF8 ), value );
}


void SpringUnitSyncLib::SetSpringConfigFloat( const wxString& key, const float value )
{
  InitLib( m_set_spring_config_float );

  m_set_spring_config_float( key.mb_str( wxConvUTF8 ), value );
}


