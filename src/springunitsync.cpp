#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <cassert>
#include <stdexcept>

#include "springunitsync.h"

#include "utils.h"
#include "settings.h"

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

UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname )
{
  UnitSyncMap m;
  if ( !m_loaded ) return m;

  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );
  //m_get_map_info_ex( mapname.c_str(), &m.info, 0 );
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

