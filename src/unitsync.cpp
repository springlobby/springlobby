//
// Class: Unitsync
//

#include "unitsync.h"
#include <assert.h>
#include <stdexcept>


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

#ifdef WIN32

  m_libhandle = LoadLibrary( loc.c_str() );
  if (m_libhandle == NULL) {
    debug_error( "Couldn't load the unitsync library" );
    return false;
  }

#else

  m_libhandle = dlopen( loc.c_str(), RTLD_LOCAL | RTLD_LAZY );
  if (m_libhandle == NULL) {
    debug_error( "Couldn't load the unitsync library" );
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
  return 0;
}


bool Unitsync::ModExists( const std::string& modname )
{
  if ( !m_loaded ) return false;
  return false;
}


UnitsyncMod Unitsync::GetMod( const std::string& modname )
{
  UnitsyncMod m;
  if ( !m_loaded ) return m;
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


UnitsyncMap Unitsync::GetMap( const std::string& mapname )
{
  UnitsyncMap m;
  if ( !m_loaded ) return m;

  int i = GetMapIndex( mapname );
  ASSERT_LOGIC( i >= 0, "Map does not exist" );
  //m_get_map_info_ex( mapname.c_str(), &m.info, 0 );
  m.name = mapname;
  m.hash = i2s( m_get_map_checksum( i ) );
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

