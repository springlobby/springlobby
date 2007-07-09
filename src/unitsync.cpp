//
// Class: Unitsync
//

#include "unitsync.h"
#include <assert.h>
#include <stdexcept>

#ifdef WIN32
static char* dllname = "\\unitsync.dll";
#else
static char* dllname = "/unitsync.so";
#endif

Unitsync& usync()
{
  static Unitsync m_sync;
  return m_sync;
}


void* Unitsync::_GetLibFuncPtr( const std::string& name )
{
  assert( m_loaded );
  void* ptr = NULL;
#ifdef WIN32
  ptr = (void*)GetProcAddress( m_libhandle, name.c_str() );
#else
  ptr = dlsym( m_libhandle, name.c_str() );
#endif
  ASSERT_RUNTIME( ptr != NULL, "Couldn't load " + name + " from unitsync library" );
  return ptr;
}


bool Unitsync::LoadUnitsyncLib()
{
  if ( m_loaded ) return true;
  
  wxSetWorkingDirectory( WX_STRING(sett().GetSpringDir()) );

  // Load the library.
  std::string loc;
  if ( sett().GetUnitsyncUseDefLoc() ) loc = sett().GetSpringDir() + dllname;
  else loc = sett().GetUnitsyncLoc();

  debug( "Loading from: " + loc );
#ifdef WIN32

  m_libhandle = LoadLibrary( loc.c_str() );
  if (m_libhandle == NULL) {
    debug_error( "Couldn't load the unitsync library" );
    return false;
  }

#else

  try {
    m_libhandle = dlopen( loc.c_str(), RTLD_LOCAL | RTLD_LAZY );
  } catch(...) {
    m_libhandle = NULL;
  }
  
  if (m_libhandle == NULL) {
    debug_error( "Couldn't load the unitsync library" );
    std::string dlerr = dlerror();
    debug_error( dlerr );
    
    wxMessageDialog msg( NULL, _T("The unitsync library failed to load from the location \"") + WX_STRING(loc) + _T("\".\nIt failed with the error message \"") + WX_STRING(dlerr)+ _T("\".\n\nYou might want to look at the Spring Options again. If you need any help setting unitsync up you will find it under the Help main menu."), _T("Error loading unitsync"), wxOK | wxICON_ERROR );
    
    msg.ShowModal();
    
    return false;
  }

#endif

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
    FreeUnitsyncLib();
    return false;
  }

  return true;
}


void Unitsync::FreeUnitsyncLib()
{
  if ( !m_loaded ) return;
  m_uninit();

#ifdef WIN32
  FreeLibrary(m_libhandle);
#else
  dlclose(m_libhandle);
#endif
  
  m_loaded = false;
}


bool Unitsync::IsLoaded()
{
  return m_loaded;
}



int Unitsync::GetNumMods()
{
  if ( !m_loaded ) return 0;
  return m_get_mod_count();
}


int Unitsync::GetModIndex( const std::string& name )
{
  if ( !m_loaded ) return -1;
  int mc = m_get_mod_count();
  for ( int i = 0; i < mc; i++ ) {
    std::string cmp = m_get_mod_name( i );
    if ( name == cmp )
      return i;
  }
  return -1;
}


bool Unitsync::ModExists( const std::string& modname )
{
  if ( !m_loaded ) return false;
  return GetModIndex( modname ) >= 0;
}


UnitsyncMod Unitsync::GetMod( const std::string& modname )
{
  UnitsyncMod m;
  if ( !m_loaded ) return m;

  int i = GetModIndex( modname );
  ASSERT_LOGIC( i >= 0, "Mod does not exist" );

  m.name = m_get_mod_name( i );
  m.hash = i2s(m_get_mod_checksum( i ));

  return m;
}


int Unitsync::GetNumMaps()
{
  if ( !m_loaded ) return 0;
  return m_get_map_count();
}


bool Unitsync::MapExists( const std::string& mapname )
{
  if ( !m_loaded ) return false;
  return GetMapIndex( mapname ) >= 0;
}


bool Unitsync::MapExists( const std::string& mapname, const std::string hash )
{
  if ( !m_loaded ) return false;
  int i = GetMapIndex( mapname );
  if ( i >= 0 ) {
    return ( i2s(m_get_map_checksum( i )) == hash );
  }
  return false;
}


UnitsyncMap Unitsync::GetMap( const std::string& mapname )
{
  UnitsyncMap m;
  if ( !m_loaded ) return m;

  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );
  //m_get_map_info_ex( mapname.c_str(), &m.info, 0 );
  m.name = m_get_map_name( i );
  m.hash = i2s(m_get_map_checksum( i ));
  return m;
}


int Unitsync::GetMapIndex( const std::string& name )
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


std::string Unitsync::GetModArchive( int index )
{
  if ( !m_loaded ) return "unknown";
  return m_get_mod_archive( index );
}


int Unitsync::GetSideCount( const std::string& modname )
{
  if ( !m_loaded ) return 0;
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  return m_get_side_count();
}


std::string Unitsync::GetSideName( const std::string& modname, int index )
{
  if ( !m_loaded ) return "unknown";
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  ASSERT_LOGIC( m_get_side_count() > index, "Side index too high." );
  return m_get_side_name( index );
}


int Unitsync::GetNumUnits( const std::string& modname )
{
  if ( !m_loaded ) return 0;
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  m_proc_units_nocheck();
  return m_get_unit_count();
}


int Unitsync::GetUnitIndex( const std::string& modname, const std::string& name )
{
  if ( !m_loaded ) return -1;
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  m_proc_units_nocheck();
  for ( int i = 0; i < m_get_unit_count(); i++ ) {
    if ( m_get_unit_name( i ) == name ) return i;
  }
  return -1;
}


std::string Unitsync::GetFullUnitName( const std::string& modname, int index )
{
  if ( (!m_loaded) || (index < 0) ) return "unknown";
  m_add_all_archives( GetModArchive( GetModIndex( modname ) ).c_str() );
  m_proc_units_nocheck();
  return m_get_unit_full_name( index );
}

