/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/filename.h>
#include <wx/dynlib.h>
#include <wx/image.h>
#include <wx/log.h>
#include <stdexcept>
#include <cmath>

#include "springunitsynclib.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "utils/misc.h"
#include "globalsmanager.h"

#define LOCK_UNITSYNC wxCriticalSectionLocker lock_criticalsection(m_lock)

//! Macro that checks if a function is present/loaded, unitsync is loaded, and locks it on call.
#define InitLib( arg ) \
	LOCK_UNITSYNC; \
	UNITSYNC_EXCEPTION( m_loaded, _T("Unitsync not loaded.") ); \
	UNITSYNC_EXCEPTION( arg, _T("Function was not in unitsync library.") );


SpringUnitSyncLib::SpringUnitSyncLib():
  m_loaded(false),
  m_libhandle(NULL),
  m_path(wxEmptyString),
  m_init(NULL),
  m_uninit(NULL)
{
}


SpringUnitSyncLib::~SpringUnitSyncLib()
{
  Unload();
}


SpringUnitSyncLib& susynclib()
{
    static LineInfo<SpringUnitSyncLib> m( AT );
  static GlobalObjectHolder<SpringUnitSyncLib, LineInfo<SpringUnitSyncLib> > lib( m );
  return lib.GetInstance();
}


void SpringUnitSyncLib::Load( const wxString& path, const wxString& ForceConfigFilePath )
{
  LOCK_UNITSYNC;

  _Load( path );

  if ( !ForceConfigFilePath.IsEmpty() )
  {
    if ( m_set_spring_config_file_path )
    {
        m_set_spring_config_file_path( ForceConfigFilePath.mb_str() );
    }
  }

  _Init();
}


void SpringUnitSyncLib::_Load( const wxString& path )
{
  if ( _IsLoaded() && path == m_path ) return;

  _Unload();

  m_path = path;

  // Load the library.
  wxLogMessage( _T("Loading from: %s"), path.c_str() );

  // Check if library exists
  if ( !wxFileName::FileExists( path ) )
  {
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

  // Load all function from library.
  try {
    m_init = (InitPtr)_GetLibFuncPtr(_T("Init"));
    m_uninit = (UnInitPtr)_GetLibFuncPtr(_T("UnInit"));
    m_get_next_error = (GetNextErrorPtr)_GetLibFuncPtr(_T("GetNextError"));
    m_get_writeable_data_dir = (GetWritableDataDirectoryPtr)_GetLibFuncPtr(_T("GetWritableDataDirectory"));

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
    m_remove_all_archives = (RemoveAllArchivesPtr)_GetLibFuncPtr(_T("RemoveAllArchives"));

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

    m_get_mod_valid_map_count = (GetModValidMapCountPtr)_GetLibFuncPtr(_T("GetModValidMapCount"));
    m_get_valid_map = (GetModValidMapPtr)_GetLibFuncPtr(_T("GetModValidMap"));

    m_get_luaai_count = (GetLuaAICountPtr)_GetLibFuncPtr(_T("GetLuaAICount"));
    m_get_luaai_name = (GetLuaAINamePtr)_GetLibFuncPtr(_T("GetLuaAIName"));
    m_get_luaai_desc = (GetLuaAIDescPtr)_GetLibFuncPtr(_T("GetLuaAIDesc"));

    m_get_map_option_count = (GetMapOptionCountPtr)_GetLibFuncPtr(_T("GetMapOptionCount"));
    m_get_custom_option_count = (GetCustomOptionCountPtr)_GetLibFuncPtr(_T("GetCustomOptionCount"));
    m_get_mod_option_count = (GetModOptionCountPtr)_GetLibFuncPtr(_T("GetModOptionCount"));
    m_get_skirmish_ai_option_count = (GetSkirmishAIOptionCountPtr)_GetLibFuncPtr(_T("GetSkirmishAIOptionCount"));
    m_get_option_key = (GetOptionKeyPtr)_GetLibFuncPtr(_T("GetOptionKey"));
    m_get_option_name = (GetOptionNamePtr)_GetLibFuncPtr(_T("GetOptionName"));
    m_get_option_desc = (GetOptionDescPtr)_GetLibFuncPtr(_T("GetOptionDesc"));
    m_get_option_type = (GetOptionTypePtr)_GetLibFuncPtr(_T("GetOptionType"));
    m_get_option_section = (GetOptionSectionPtr)_GetLibFuncPtr(_T("GetOptionSection"));
    m_get_option_style = (GetOptionStylePtr)_GetLibFuncPtr(_T("GetOptionStyle"));
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

    m_set_spring_config_file_path = (SetSpringConfigFilePtr)_GetLibFuncPtr(_T("SetSpringConfigFile"));
    m_get_spring_config_file_path = (GetSpringConfigFilePtr)_GetLibFuncPtr(_T("GetSpringConfigFile"));

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

		m_get_skirmish_ai_count = (GetSkirmishAICountPtr)_GetLibFuncPtr(_T("GetSkirmishAICount"));
		m_get_skirmish_ai_info_count = (GetSkirmishAIInfoCountPtr)_GetLibFuncPtr(_T("GetSkirmishAIInfoCount"));
		m_get_skirmish_ai_info_key = (GetInfoKeyPtr)_GetLibFuncPtr(_T("GetInfoKey"));
		m_get_skirmish_ai_info_value = (GetInfoValuePtr)_GetLibFuncPtr(_T("GetInfoValue"));
		m_get_skirmish_ai_info_description = (GetInfoDescriptionPtr)_GetLibFuncPtr(_T("GetInfoDescription"));

    // begin lua parser calls

    m_parser_close = (lpClosePtr)_GetLibFuncPtr(_T("lpClose"));
    m_parser_open_file = (lpOpenFilePtr)_GetLibFuncPtr(_T("lpOpenFile"));
    m_parser_open_source = (lpOpenSourcePtr)_GetLibFuncPtr(_T("lpOpenSource"));
    m_parser_execute = (lpExecutePtr)_GetLibFuncPtr(_T("lpExecute"));
    m_parser_error_log = (lpErrorLogPtr)_GetLibFuncPtr(_T("lpErrorLog"));

    m_parser_add_table_int = (lpAddTableIntPtr)_GetLibFuncPtr(_T("lpAddTableInt"));
    m_parser_add_table_string = (lpAddTableStrPtr)_GetLibFuncPtr(_T("lpAddTableStr"));
    m_parser_end_table = (lpEndTablePtr)_GetLibFuncPtr(_T("lpEndTable"));
    m_parser_add_int_key_int_value = (lpAddIntKeyIntValPtr)_GetLibFuncPtr(_T("lpAddIntKeyIntVal"));
    m_parser_add_string_key_int_value = (lpAddStrKeyIntValPtr)_GetLibFuncPtr(_T("lpAddStrKeyIntVal"));
    m_parser_add_int_key_bool_value = (lpAddIntKeyBoolValPtr)_GetLibFuncPtr(_T("lpAddIntKeyBoolVal"));
    m_parser_add_string_key_bool_value = (lpAddStrKeyBoolValPtr)_GetLibFuncPtr(_T("lpAddStrKeyBoolVal"));
    m_parser_add_int_key_float_value = (lpAddIntKeyFloatValPtr)_GetLibFuncPtr(_T("lpAddIntKeyFloatVal"));
    m_parser_add_string_key_float_value = (lpAddStrKeyFloatValPtr)_GetLibFuncPtr(_T("lpAddStrKeyFloatVal"));
    m_parser_add_int_key_string_value = (lpAddIntKeyStrValPtr)_GetLibFuncPtr(_T("lpAddIntKeyStrVal"));
    m_parser_add_string_key_string_value = (lpAddStrKeyStrValPtr)_GetLibFuncPtr(_T("lpAddStrKeyStrVal"));

    m_parser_root_table = (lpRootTablePtr)_GetLibFuncPtr(_T("lpRootTable"));
    m_parser_root_table_expression = (lpRootTableExprPtr)_GetLibFuncPtr(_T("lpRootTableExpr"));
    m_parser_sub_table_int = (lpSubTableIntPtr)_GetLibFuncPtr(_T("lpSubTableInt"));
    m_parser_sub_table_string = (lpSubTableStrPtr)_GetLibFuncPtr(_T("lpSubTableStr"));
    m_parser_sub_table_expression = (lpSubTableExprPtr)_GetLibFuncPtr(_T("lpSubTableExpr"));
    m_parser_pop_table = (lpPopTablePtr)_GetLibFuncPtr(_T("lpPopTable"));

    m_parser_key_int_exists = (lpGetKeyExistsIntPtr)_GetLibFuncPtr(_T("lpGetKeyExistsInt"));
    m_parser_key_string_exists = (lpGetKeyExistsStrPtr)_GetLibFuncPtr(_T("lpGetKeyExistsStr"));

    m_parser_int_key_get_type = (lpGetIntKeyTypePtr)_GetLibFuncPtr(_T("lpGetIntKeyType"));
    m_parser_string_key_get_type = (lpGetStrKeyTypePtr)_GetLibFuncPtr(_T("lpGetStrKeyType"));

    m_parser_int_key_get_list_count = (lpGetIntKeyListCountPtr)_GetLibFuncPtr(_T("lpGetIntKeyListCount"));
    m_parser_int_key_get_list_entry = (lpGetIntKeyListEntryPtr)_GetLibFuncPtr(_T("lpGetIntKeyListEntry"));
    m_parser_string_key_get_list_count = (lpGetStrKeyListCountPtr)_GetLibFuncPtr(_T("lpGetStrKeyListCount"));
    m_parser_string_key_get_list_entry = (lpGetStrKeyListEntryPtr)_GetLibFuncPtr(_T("lpGetStrKeyListEntry"));

    m_parser_int_key_get_int_value = (lpGetIntKeyIntValPtr)_GetLibFuncPtr(_T("lpGetIntKeyIntVal"));
    m_parser_string_key_get_int_value = (lpGetStrKeyIntValPtr)_GetLibFuncPtr(_T("lpGetStrKeyIntVal"));
    m_parser_int_key_get_bool_value = (lpGetIntKeyBoolValPtr)_GetLibFuncPtr(_T("lpGetIntKeyBoolVal"));
    m_parser_string_key_get_bool_value = (lpGetStrKeyBoolValPtr)_GetLibFuncPtr(_T("lpGetStrKeyBoolVal"));
    m_parser_int_key_get_float_value = (lpGetIntKeyFloatValPtr)_GetLibFuncPtr(_T("lpGetIntKeyFloatVal"));
    m_parser_string_key_get_float_value = (lpGetStrKeyFloatValPtr)_GetLibFuncPtr(_T("lpGetStrKeyFloatVal"));
    m_parser_int_key_get_string_value = (lpGetIntKeyStrValPtr)_GetLibFuncPtr(_T("lpGetIntKeyStrVal"));
    m_parser_string_key_get_string_value = (lpGetStrKeyStrValPtr)_GetLibFuncPtr(_T("lpGetStrKeyStrVal"));

    // only when we end up here unitsync was succesfully loaded.
    m_loaded = true;
  }
  catch ( ... )
  {
    // don't uninit unitsync in _Unload -- it hasn't been init'ed yet
    m_uninit = NULL;
    _Unload();
    ASSERT_EXCEPTION( false, _T("Failed to load Unitsync lib.") );
  }
}


void SpringUnitSyncLib::_Init()
{
  if ( _IsLoaded() && m_init != NULL )
  {
    wxLog *currentarget = wxLog::GetActiveTarget();
    wxLog *templogger = new wxLogGui();
    wxLog::SetActiveTarget( templogger );

    m_current_mod = wxEmptyString;
    m_init( true, 1 );

    wxArrayString errors = GetUnitsyncErrors();
    for ( unsigned int i = 0; i < errors.GetCount(); i++ )
    {
    	wxLogError( _T("%s"), errors[i].c_str() );
    }

    wxLog::SetActiveTarget( currentarget );
    delete templogger;
  }
}


void SpringUnitSyncLib::_RemoveAllArchives()
{
  if (m_remove_all_archives)
  	m_remove_all_archives();
  else
    _Init();
}


void SpringUnitSyncLib::Unload()
{
  if ( !_IsLoaded() ) return;// dont even lock anything if unloaded.
  LOCK_UNITSYNC;

  _Unload();
}


void SpringUnitSyncLib::_Unload()
{
  // as soon as we enter m_uninit unitsync technically isn't loaded anymore.
  m_loaded = false;

  m_path = wxEmptyString;

  // can't call UnSetCurrentMod() because it takes the unitsync lock
  m_current_mod = wxEmptyString;

  if (m_uninit)
    m_uninit();

  delete m_libhandle;
  m_libhandle = NULL;

  m_init = NULL;
  m_uninit = NULL;
}


bool SpringUnitSyncLib::IsLoaded()
{
  return m_loaded;
}


bool SpringUnitSyncLib::_IsLoaded()
{
  return m_loaded;
}


void SpringUnitSyncLib::AssertUnitsyncOk()
{
	UNITSYNC_EXCEPTION( m_loaded, _T("Unitsync not loaded.") );
	UNITSYNC_EXCEPTION( m_get_next_error, _T("Function was not in unitsync library.") );
  UNITSYNC_EXCEPTION( false, WX_STRINGC( m_get_next_error() ) );
}


wxArrayString SpringUnitSyncLib::GetUnitsyncErrors()
{
  wxArrayString ret;
  try
  {
		UNITSYNC_EXCEPTION( m_loaded, _T("Unitsync not loaded.") );
		UNITSYNC_EXCEPTION( m_get_next_error, _T("Function was not in unitsync library.") );

    wxString msg = WX_STRINGC( m_get_next_error() );
    while ( !msg.IsEmpty() )
    {
      ret.Add( msg );
      msg = WX_STRINGC( m_get_next_error() );
    }
    return ret;
  }
  catch ( unitsync_assert &e )
  {
    ret.Add( WX_STRINGC( e.what() ) );
    return ret;
  }
}


bool SpringUnitSyncLib::VersionSupports( IUnitSync::GameFeature feature )
{
  LOCK_UNITSYNC;

  switch (feature)
  {
    case IUnitSync::USYNC_Sett_Handler: return m_set_spring_config_string;
    case IUnitSync::USYNC_GetInfoMap:   return m_get_infomap_size;
    case IUnitSync::USYNC_GetDataDir:   return m_get_writeable_data_dir;
    case IUnitSync::USYNC_GetSkirmishAI:   return m_get_skirmish_ai_count;
    default: return false;
  }
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
  out.positions = std::vector<StartPos>( in.positions, in.positions + in.posCount );
}


void SpringUnitSyncLib::SetCurrentMod( const wxString& modname )
{
  InitLib( m_init ); // assumes the others are fine
                     // (m_add_all_archives, m_get_mod_archive, m_get_mod_index)

  _SetCurrentMod( modname );
}


void SpringUnitSyncLib::_SetCurrentMod( const wxString& modname )
{
  if ( m_current_mod != modname )
  {
    wxLogDebugFunc( modname );
    if ( !m_current_mod.IsEmpty() ) _RemoveAllArchives();
    m_add_all_archives( m_get_mod_archive( m_get_mod_index( modname.mb_str( wxConvUTF8 ) ) ) );
    m_current_mod = modname;
  }
}


void SpringUnitSyncLib::UnSetCurrentMod( )
{
  LOCK_UNITSYNC;
  if ( !m_current_mod.IsEmpty() ) _RemoveAllArchives();
  m_current_mod = wxEmptyString;
}


int SpringUnitSyncLib::GetModIndex( const wxString& name )
{
  return GetPrimaryModIndex( name );
}


std::map<wxString, wxString> SpringUnitSyncLib::GetSpringVersionList(const std::map<wxString, wxString>& usync_paths)
{
  LOCK_UNITSYNC;
  wxLogDebugFunc(_T(""));

  std::map<wxString, wxString> ret;

  for (std::map<wxString, wxString>::const_iterator it = usync_paths.begin(); it != usync_paths.end(); ++it)
  {
  	wxString path = it->second;
    try
    {

		 if ( !wxFileName::FileExists( path ) )
			{
				wxLogError( _T("File not found: %s"), path.c_str() );
				ASSERT_EXCEPTION( false, _T("Failed to load Unitsync lib.") );
			}

  #ifdef __WXMSW__
      wxSetWorkingDirectory( path.BeforeLast('\\') );
  #endif
      wxDynamicLibrary temphandle( path );
      ASSERT_EXCEPTION( temphandle.IsLoaded(), _T("Couldn't load the unitsync library") );

			GetSpringVersionPtr getspringversion = 0;
			wxString functionname = _T("GetSpringVersion");
			if ( temphandle.HasSymbol( functionname ) )
			{
				getspringversion = (GetSpringVersionPtr)temphandle.GetSymbol( functionname );
			}
			UNITSYNC_EXCEPTION( getspringversion, _T("getspringversion: function not found") );
			wxString version = WX_STRINGC( getspringversion() );
			wxLogMessage( _T("Found spring version: %s"), version.c_str() );
			ret[it->first] = version;

    }
    catch(...){}
  }

  return ret;
}


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //  -- The UnitSync functions --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


wxString SpringUnitSyncLib::GetSpringVersion()
{
  InitLib( m_get_spring_version );

  return WX_STRINGC( m_get_spring_version() );
}

wxString SpringUnitSyncLib::GetSpringDataDir()
{
  InitLib( m_get_writeable_data_dir );

  return WX_STRINGC( m_get_writeable_data_dir() );
}

wxString SpringUnitSyncLib::GetConfigFilePath()
{
  InitLib( m_get_spring_config_file_path );

  return WX_STRINGC( m_get_spring_config_file_path() );
}


int SpringUnitSyncLib::GetMapCount()
{
  InitLib( m_get_map_count );

  return m_get_map_count();
}


wxString SpringUnitSyncLib::GetMapChecksum( int index )
{
  InitLib( m_get_map_checksum );

  return TowxString( (unsigned int)m_get_map_checksum( index ) );
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


wxArrayString SpringUnitSyncLib::GetMapDeps( int index )
{
  int count = GetMapArchiveCount( index );
	wxArrayString ret;
	for ( int i = 0; i < count; i++ )
	{
		ret.Add( GetMapArchiveName( i ) );
	}
  return ret;
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

  // this unitsync call returns a pointer to a static buffer
  unsigned short* colours = (unsigned short*)m_get_minimap( mapFileName.mb_str(wxConvUTF8), miplevel );
  ASSERT_EXCEPTION( colours, _T("Get minimap failed") );

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
  uninitialized_array<uchar> grayscale(width * height);
  uchar* true_colours = metalmap.GetData();

  retval = m_get_infomap(mapFileName.mb_str(wxConvUTF8), "metal", grayscale, 1 /*byte per pixel*/);
  ASSERT_EXCEPTION( retval != 0, _T("Get metalmap failed") );

  for ( int i = 0; i < width*height; i++ ) {
    true_colours[(i*3)  ] = 0;
    true_colours[(i*3)+1] = grayscale[i];
    true_colours[(i*3)+2] = 0;
  }

  return metalmap;
}


wxImage SpringUnitSyncLib::GetHeightmap( const wxString& mapFileName )
{
  InitLib( m_get_infomap_size ); // assume GetInfoMap is available too

  wxLogMessage( _T("Heightmap: %s"), mapFileName.c_str() );

  int width = 0, height = 0, retval;

  retval = m_get_infomap_size(mapFileName.mb_str(wxConvUTF8), "height", &width, &height);
  ASSERT_EXCEPTION( retval != 0 && width * height != 0, _T("Get heightmap size failed") );

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  wxImage heightmap(width, height, false);
  uninitialized_array<ushort> grayscale(width * height);
  uchar* true_colours = heightmap.GetData();

  retval = m_get_infomap(mapFileName.mb_str(wxConvUTF8), "height", grayscale, 2 /*byte per pixel*/);
  ASSERT_EXCEPTION( retval != 0, _T("Get heightmap failed") );

  // the height is mapped to this "palette" of colors
  // the colors are linearly interpolated

  const uchar points[][3] = {
  	{   0,   0,   0 },
  	{   0,   0, 255 },
  	{   0, 255, 255 },
  	{   0, 255,   0 },
  	{ 255, 255,   0 },
  	{ 255,   0,   0 },
  };
  const int numPoints = sizeof(points) / sizeof(points[0]);

  // find range of values present in the height data returned by unitsync
  int min = 65536;
  int max = 0;

  for ( int i = 0; i < width*height; i++ ) {
    if (grayscale[i] < min) min = grayscale[i];
    if (grayscale[i] > max) max = grayscale[i];
  }

  // prevent division by zero -- heightmap wouldn't contain any information anyway
  if (min == max)
    return wxImage( 1, 1 );

  // perform the mapping from 16 bit grayscale to 24 bit true colour
  const double range = max - min + 1;
  for ( int i = 0; i < width*height; i++ ) {
    const double value = (grayscale[i] - min) / (range / (numPoints - 1));
    const int idx1 = int(value);
    const int idx2 = idx1 + 1;
    const int t = int(256.0 * (value - std::floor(value)));

    //assert(idx1 >= 0 && idx1 < numPoints-1);
    //assert(idx2 >= 1 && idx2 < numPoints);
    //assert(t >= 0 && t <= 255);

    for ( int j = 0; j < 3; ++j)
      true_colours[(i*3)+j] = (points[idx1][j] * (255 - t) + points[idx2][j] * t) / 255;
  }

  return heightmap;
}


wxString SpringUnitSyncLib::GetPrimaryModChecksum( int index )
{
  InitLib( m_get_mod_checksum );

  return TowxString( (unsigned int)m_get_mod_checksum( index ) );
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
  UNITSYNC_EXCEPTION( m_get_mod_count, _T("Function was not in unitsync library.") );

  int count = m_get_mod_count();
	UNITSYNC_EXCEPTION( index < count, _T("index out of bounds") );
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


wxString SpringUnitSyncLib::GetPrimaryModChecksumFromName( const wxString& name )
{
  InitLib( m_get_primary_mod_checksum_from_name );

  return TowxString( (unsigned int)m_get_primary_mod_checksum_from_name( name.mb_str( wxConvUTF8 ) ) );
}


wxArrayString SpringUnitSyncLib::GetModDeps( int index )
{
  int count = GetPrimaryModArchiveCount( index );
	wxArrayString ret;
	for ( int i = 0; i < count; i++ )
	{
		ret.Add( GetPrimaryModArchiveList( i ) );
	}
  return ret;
}


wxArrayString SpringUnitSyncLib::GetSides( const wxString& modName )
{
  InitLib( m_get_side_count );
	UNITSYNC_EXCEPTION( m_get_side_name, _T("Function was not in unitsync library.") )

  _SetCurrentMod( modName );
  int count = m_get_side_count();
  wxArrayString ret;
  for ( int i = 0; i < count; i ++ ) ret.Add( WX_STRINGC( m_get_side_name( i ) ) );
  return ret;
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


wxArrayString SpringUnitSyncLib::FindFilesVFS( const wxString& name )
{
  InitLib( m_find_files_vfs );
	UNITSYNC_EXCEPTION( m_init_find_vfs, _T("Function was not in unitsync library.") );
	int handle = m_init_find_vfs( name.mb_str(wxConvUTF8) );
	wxArrayString ret;
	do
	{
		char buffer[1025];
		handle = m_find_files_vfs( handle, &buffer[0], 1024 );
		buffer[1024] = 0;
		ret.Add( WX_STRINGC( &buffer[0] ) );
	} while ( handle );
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

  _SetCurrentMod( modname );
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

unsigned int SpringUnitSyncLib::GetValidMapCount( const wxString& modname )
{
  InitLib( m_get_mod_valid_map_count );

  _SetCurrentMod( modname );
  return m_get_mod_valid_map_count();
}


wxString SpringUnitSyncLib::GetValidMapName( unsigned int MapIndex )
{
  InitLib( m_get_valid_map );

  return WX_STRINGC( m_get_valid_map( MapIndex ) );
}


int SpringUnitSyncLib::GetMapOptionCount( const wxString& name )
{
  InitLib( m_get_map_option_count );
  ASSERT_EXCEPTION( !name.IsEmpty(), _T("passing void mapname to unitsync") );

  return m_get_map_option_count( name.mb_str( wxConvUTF8 ) );
}

int SpringUnitSyncLib::GetCustomOptionCount( const wxString& modname, const wxString& filename )
{
    InitLib( m_get_custom_option_count );
    ASSERT_EXCEPTION( !modname.IsEmpty(), _T("passing void XXXname to unitsync") );
    _SetCurrentMod( modname );
    return m_get_custom_option_count( filename.mb_str( wxConvUTF8 ) );
}


int SpringUnitSyncLib::GetModOptionCount( const wxString& name )
{
  InitLib( m_get_mod_option_count );
  ASSERT_EXCEPTION( !name.IsEmpty(), _T("passing void modname to unitsync") );

  _SetCurrentMod( name );
  return m_get_mod_option_count();
}


int SpringUnitSyncLib::GetAIOptionCount( const wxString& modname, int aiIndex )
{
	InitLib( m_get_skirmish_ai_option_count );
	_SetCurrentMod( modname );
	ASSERT_EXCEPTION( m_get_skirmish_ai_count , _T("Function was not in unitsync library.") );

	UNITSYNC_EXCEPTION( ( aiIndex >= 0 ) && ( aiIndex < m_get_skirmish_ai_count() ), _T("aiIndex out of bounds") );

	return m_get_skirmish_ai_option_count( aiIndex );
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

wxString SpringUnitSyncLib::GetOptionSection( int optIndex )
{
  InitLib( m_get_option_section );

  return WX_STRINGC( m_get_option_section( optIndex ) );
}

wxString SpringUnitSyncLib::GetOptionStyle( int optIndex )
{
  InitLib( m_get_option_style );

  return WX_STRINGC( m_get_option_style( optIndex ) );
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


int SpringUnitSyncLib::GetSkirmishAICount( const wxString& modname )
{
  InitLib( m_get_skirmish_ai_count );
  _SetCurrentMod( modname );

  return m_get_skirmish_ai_count();
}


wxArrayString SpringUnitSyncLib::GetAIInfo( int aiIndex )
{
	InitLib( m_get_skirmish_ai_count );
	UNITSYNC_EXCEPTION( m_get_skirmish_ai_info_count, _T("Function was not in unitsync library.") );
	UNITSYNC_EXCEPTION( m_get_skirmish_ai_info_description, _T("Function was not in unitsync library.") );
	UNITSYNC_EXCEPTION( m_get_skirmish_ai_info_key, _T("Function was not in unitsync library.") );
	UNITSYNC_EXCEPTION( m_get_skirmish_ai_info_value, _T("Function was not in unitsync library.") );

	wxArrayString ret;
	UNITSYNC_EXCEPTION( ( aiIndex >= 0 ) && ( aiIndex < m_get_skirmish_ai_count() ), _T("aiIndex out of bounds") );

	int infoCount = m_get_skirmish_ai_info_count( aiIndex );
	for( int i = 0; i < infoCount; i++ )
	{
		ret.Add( WX_STRINGC( m_get_skirmish_ai_info_key( i ) ) );
		ret.Add( WX_STRINGC( m_get_skirmish_ai_info_value( i ) ) );
		ret.Add( WX_STRINGC( m_get_skirmish_ai_info_description( i ) ) );
	}
	return ret;
}

wxString SpringUnitSyncLib::GetArchiveChecksum( const wxString& VFSPath )
{
	InitLib( m_get_archive_checksum );
	return TowxString( m_get_archive_checksum( VFSPath.mb_str() ) );
}

/// lua parser

void SpringUnitSyncLib::CloseParser()
{
	InitLib( m_parser_close );

	m_parser_close();
}


bool SpringUnitSyncLib::OpenParserFile( const wxString& filename, const wxString& filemodes, const wxString& accessModes )
{
	InitLib( m_parser_open_file );

	return m_parser_open_file( filename.mb_str(), filemodes.mb_str(), accessModes.mb_str() );
}

bool SpringUnitSyncLib::OpenParserSource( const wxString& source, const wxString& accessModes )
{
	InitLib( m_parser_open_source );

	return m_parser_open_source( source.mb_str(), accessModes.mb_str() );
}

bool SpringUnitSyncLib::ParserExecute()
{
	InitLib( m_parser_execute );

	return m_parser_execute();
}

wxString SpringUnitSyncLib::ParserErrorLog()
{
	InitLib( m_parser_error_log );

	return WX_STRINGC( m_parser_error_log() );
}


void SpringUnitSyncLib::ParserAddTable( int key, bool override )
{
	InitLib( m_parser_add_table_int );

  m_parser_add_table_int( key, override );
}

void SpringUnitSyncLib::ParserAddTable( const wxString& key, bool override )
{
	InitLib( m_parser_add_table_string );

  m_parser_add_table_string( key.mb_str(), override );
}

void SpringUnitSyncLib::ParserEndTable()
{
	InitLib( m_parser_end_table );

	m_parser_end_table();
}

void SpringUnitSyncLib::ParserAddTableValue( int key, int val )
{
	InitLib( m_parser_add_int_key_int_value );

	m_parser_add_int_key_int_value( key, val );
}

void SpringUnitSyncLib::ParserAddTableValue( const wxString& key, int val )
{
	InitLib( m_parser_add_string_key_int_value );

	m_parser_add_string_key_int_value( key.mb_str(), val );
}

void SpringUnitSyncLib::ParserAddTableValue( int key, bool val )
{
	InitLib( m_parser_add_int_key_int_value );

	m_parser_add_int_key_int_value( key, val );
}

void SpringUnitSyncLib::ParserAddTableValue( const wxString& key, bool val )
{
	InitLib( m_parser_add_string_key_int_value );

	m_parser_add_string_key_int_value( key.mb_str(), val );
}

void SpringUnitSyncLib::ParserAddTableValue( int key, const wxString& val )
{
	InitLib( m_parser_add_int_key_string_value );

	m_parser_add_int_key_string_value( key, val.mb_str() );
}

void SpringUnitSyncLib::ParserAddTableValue( const wxString& key, const wxString& val )
{
	InitLib( m_parser_add_string_key_string_value );

	m_parser_add_string_key_string_value( key.mb_str(), val.mb_str() );
}

void SpringUnitSyncLib::ParserAddTableValue( int key, float val )
{
	InitLib( m_parser_add_int_key_float_value );

	m_parser_add_int_key_float_value( key, val );
}

void SpringUnitSyncLib::ParserAddTableValue( const wxString& key, float val )
{
	InitLib( m_parser_add_string_key_float_value );

	m_parser_add_string_key_float_value( key.mb_str(), val );
}


bool SpringUnitSyncLib::ParserGetRootTable()
{
	InitLib( m_parser_root_table );

	return m_parser_root_table();
}

bool SpringUnitSyncLib::ParserGetRootTableExpression( const wxString& exp )
{
	InitLib( m_parser_root_table_expression );

	return m_parser_root_table_expression( exp.mb_str() );
}

bool SpringUnitSyncLib::ParserGetSubTableInt( int key )
{
	InitLib( m_parser_sub_table_int );

	return m_parser_sub_table_int( key );
}

bool SpringUnitSyncLib::ParserGetSubTableString( const wxString& key )
{
	InitLib( m_parser_sub_table_string );

	return m_parser_sub_table_string( key.mb_str() );
}

bool SpringUnitSyncLib::ParserGetSubTableInt( const wxString& exp )
{
	InitLib( m_parser_sub_table_expression );

	return m_parser_sub_table_expression( exp.mb_str() );
}

void SpringUnitSyncLib::ParserPopTable()
{
	InitLib( m_parser_pop_table );

  m_parser_pop_table();
}


bool SpringUnitSyncLib::ParserKeyExists( int key )
{
	InitLib( m_parser_key_int_exists );

	return m_parser_key_int_exists( key );
}

bool SpringUnitSyncLib::ParserKeyExists( const wxString& key )
{
	InitLib( m_parser_key_string_exists );

	return m_parser_key_string_exists( key.mb_str() );
}


int SpringUnitSyncLib::ParserGetKeyType( int key )
{
	InitLib( m_parser_int_key_get_type );

	return m_parser_int_key_get_type( key );
}

int SpringUnitSyncLib::ParserGetKeyType( const wxString& key )
{
	InitLib( m_parser_string_key_get_type );

	return m_parser_string_key_get_type( key.mb_str() );
}


int SpringUnitSyncLib::ParserGetIntKeyListCount()
{
	InitLib( m_parser_int_key_get_list_count );

	return m_parser_int_key_get_list_count();
}

int SpringUnitSyncLib::ParserGetIntKeyListEntry( int index )
{
	InitLib( m_parser_int_key_get_list_entry );

	return m_parser_int_key_get_list_entry( index );
}

int SpringUnitSyncLib::ParserGetStringKeyListCount()
{
	InitLib( m_parser_string_key_get_list_count );

	return m_parser_string_key_get_list_count();
}

int SpringUnitSyncLib::ParserGetStringKeyListEntry( int index )
{
	InitLib( m_parser_int_key_get_list_entry );

	return m_parser_int_key_get_list_entry( index );
}


int SpringUnitSyncLib::GetKeyValue( int key, int defval )
{
	InitLib( m_parser_int_key_get_int_value );

	return m_parser_int_key_get_int_value( key, defval );
}

bool SpringUnitSyncLib::GetKeyValue( int key, bool defval )
{
	InitLib( m_parser_int_key_get_bool_value );

	return m_parser_int_key_get_bool_value( key, defval );
}

wxString SpringUnitSyncLib::GetKeyValue( int key, const wxString& defval )
{
	InitLib( m_parser_int_key_get_string_value );

	return WX_STRINGC( m_parser_int_key_get_string_value( key, defval.mb_str() ) );
}

float SpringUnitSyncLib::GetKeyValue( int key, float defval )
{
	InitLib( m_parser_int_key_get_float_value );

	return m_parser_int_key_get_float_value( key, defval );
}

int SpringUnitSyncLib::GetKeyValue( const wxString& key, int defval )
{
	InitLib( m_parser_string_key_get_int_value );

	return m_parser_string_key_get_int_value( key.mb_str(), defval );
}

bool SpringUnitSyncLib::GetKeyValue( const wxString& key, bool defval )
{
	InitLib( m_parser_string_key_get_bool_value );

	return m_parser_string_key_get_bool_value( key.mb_str(), defval );
}

wxString SpringUnitSyncLib::GetKeyValue( const wxString& key, const wxString& defval )
{
	InitLib( m_parser_string_key_get_string_value );

	return WX_STRINGC( m_parser_string_key_get_string_value( key.mb_str(), defval.mb_str() ) );
}

float SpringUnitSyncLib::GetKeyValue( const wxString& key, float defval )
{
	InitLib( m_parser_string_key_get_float_value );

	return m_parser_string_key_get_float_value( key.mb_str(), defval );
}
