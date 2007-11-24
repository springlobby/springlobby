/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/filename.h>
#include <wx/dynlib.h>
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


SpringUnitSyncLib* susync()
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
  std::string loc = STD_STRING( path );

  debug( "Loading from: " + loc );

  // Check if library exists
  if ( !wxFileName::FileExists( path ) ) {
    debug_error( "File not found: "+ loc );
    return;
  }

  try {
    m_libhandle = new wxDynamicLibrary( path );
    if ( !m_libhandle->IsLoaded() ) {
      debug_error("wxDynamicLibrary created, but not loaded!");
      delete m_libhandle;
      m_libhandle = 0;
    }
  } catch(...) {
    m_libhandle = 0;
  }

  ASSERT_RUNTIME( m_libhandle == 0, "Couldn't load the unitsync library" );

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
    ASSERT_RUNTIME( false, "Failed to load Unitsync lib." );
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
  LOCK_UNITSYNC;
  return m_loaded;
}


bool SpringUnitSyncLib::_IsLoaded()
{
  return m_loaded;
}


void* SpringUnitSyncLib::_GetLibFuncPtr( const wxString& name )
{
  ASSERT_LOGIC( m_loaded, "Unitsync not loaded" );
  void* ptr = m_libhandle->GetSymbol(WX_STRING(name));
  if ( !ptr ) debug( "Couldn't load " + STD_STRING(name) + " from unitsync library" );
  return ptr;
}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //  -- The UnitSync functions --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


