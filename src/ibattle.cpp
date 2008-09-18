/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>

#include "ibattle.h"
#include "utils.h"
#include "settings.h"

IBattle::IBattle():
  m_map_loaded(false),
  m_mod_loaded(false),
  m_map_exists(false),
  m_mod_exists(false)
{
}


IBattle::~IBattle()
{
}


void IBattle::SetHostMap(const wxString& mapname, const wxString& hash)
{
  if ( mapname != m_host_map_name || hash != m_host_map_hash ) {
    m_map_loaded = false;
    m_host_map_name = mapname;
    m_host_map_hash = hash;
    if ( !m_host_map_hash.IsEmpty() ) m_map_exists = usync()->MapExists( m_host_map_name, m_host_map_hash );
    else m_map_exists = usync()->MapExists( m_host_map_name );
  }
}


void IBattle::SetLocalMap(const UnitSyncMap& map)
{
  if ( map.name != m_local_map.name || map.hash != m_local_map.hash ) {
    m_local_map = map;
    m_map_loaded = true;
    if ( !m_host_map_hash.IsEmpty() ) m_map_exists = usync()->MapExists( m_host_map_name, m_host_map_hash );
    else m_map_exists = usync()->MapExists( m_host_map_name );
  }
}


const UnitSyncMap& IBattle::LoadMap()
{

  if ( !m_map_loaded ) {
    try {
      ASSERT_LOGIC( m_map_exists, _T("Map does not exist.") );
      m_local_map = usync()->GetMapEx( m_host_map_name );
      m_map_loaded = true;

    } catch (...) {}
  }
  return m_local_map;
}


wxString IBattle::GetHostMapName() const
{
  return m_host_map_name;
}


wxString IBattle::GetHostMapHash() const
{
  return m_host_map_hash;
}


void IBattle::SetHostMod( const wxString& modname, const wxString& hash )
{
  if ( m_host_mod_name != modname || m_host_mod_hash != hash ){
    m_mod_loaded = false;
    m_host_mod_name = modname;
    m_host_mod_hash = hash;
    if ( !m_host_mod_hash.IsEmpty() ) m_mod_exists = usync()->ModExists( m_host_mod_name, m_host_mod_hash );
    else m_mod_exists = usync()->ModExists( m_host_mod_name );
  }
}


void IBattle::SetLocalMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_local_mod.name || mod.hash != m_local_mod.hash ) {
    m_local_mod = mod;
    m_mod_loaded = true;
    if ( !m_host_mod_hash.IsEmpty() ) m_mod_exists = usync()->ModExists( m_host_mod_name, m_host_mod_hash );
    else m_mod_exists = usync()->ModExists( m_host_mod_name );
  }
}


const UnitSyncMod& IBattle::LoadMod()
{
  if ( !m_mod_loaded ) {
    try {
      ASSERT_LOGIC( m_mod_exists, _T("Mod does not exist.") );
      m_local_mod = usync()->GetMod( m_host_mod_name );
      m_mod_loaded = true;
    } catch (...) {}
  }
  return m_local_mod;
}


wxString IBattle::GetHostModName() const
{
  return m_host_mod_name;
}


wxString IBattle::GetHostModHash() const
{
  return m_host_mod_hash;
}


bool IBattle::MapExists()
{
  return m_map_exists;
  //return usync()->MapExists( m_map_name, m_map.hash );
}


bool IBattle::ModExists()
{
  return m_mod_exists;
  //return usync()->ModExists( m_mod_name );
}



void IBattle::DisableUnit( const wxString& unitname )
{
  if ( m_units.Index( unitname ) == wxNOT_FOUND ) m_units.Add( unitname );
}


void IBattle::EnableUnit( const wxString& unitname )
{
  int pos = m_units.Index( unitname );
  if ( pos == wxNOT_FOUND ) return;
  m_units.RemoveAt( pos );
}


void IBattle::EnableAllUnits()
{
  m_units.Empty();
}


wxArrayString IBattle::DisabledUnits()
{
  return m_units;
}


void IBattle::OnUnitSyncReloaded()
{
  if ( !m_host_mod_hash.IsEmpty() ) m_mod_exists = usync()->ModExists( m_host_mod_name, m_host_mod_hash);
  else m_mod_exists = usync()->ModExists( m_host_mod_name );
  if ( !m_host_map_hash.IsEmpty() )  m_map_exists = usync()->MapExists( m_host_map_name, m_host_map_hash );
  else  m_map_exists = usync()->MapExists( m_host_map_name );
}

unsigned int IBattle::AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll ) {
  return (unsigned int)(-1);/// note: that looks pretty crappy and needs to be investigated.
}

void IBattle::LoadOptionsPreset( const wxString& name )
{
  for ( int i = 0; i < (int)LastOption; i++)
  {
    std::map<wxString,wxString> options = sett().GetHostingPreset( name, i );
    for ( std::map<wxString,wxString>::iterator itor = options.begin(); itor != options.end(); itor++ )
    {
      CustomBattleOptions().setSingleOption( itor->first, itor->second, (GameOption)i );
    }
  }
}
