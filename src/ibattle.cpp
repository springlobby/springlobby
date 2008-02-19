/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>

#include "ibattle.h"
#include "utils.h"

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


void IBattle::SetMap(const wxString& mapname, const wxString& hash)
{
  if ( mapname != m_map.name ) {
    m_map_loaded = false;
    m_map_exists = usync()->MapExists( mapname, hash );
    m_map.hash = hash;
    m_map_name = mapname;
  }
}


void IBattle::SetMap(const UnitSyncMap& map)
{
  if ( map.name != m_map.name ) {
    m_map = map;
    m_map_name = map.name;
    m_map_loaded = true;
    m_map_exists = usync()->MapExists( m_map_name, m_map.hash );
  }
}


const UnitSyncMap& IBattle::Map()
{
  ASSERT_LOGIC( m_map_exists, _T("Map does not exist.") );

  if ( !m_map_loaded ) {
    try {

      m_map = usync()->GetMapEx( m_map_name );
      m_map_loaded = true;
      m_map_name = m_map.name;

    } catch (...) {}
  }
  return m_map;
}


wxString IBattle::GetMapName()
{
  return m_map_name;
}


wxString IBattle::GetMapHash()
{
  if ( m_map.hash == _T("") ) {
    try {
      m_map.hash = usync()->GetMap( m_map.name ).hash;
    } catch (...) { wxLogWarning( _T("Couldn't get map hash from unitsync.") ); }
  }
  return m_map.hash;
}


void IBattle::SetMod( const wxString& modname, const wxString& hash )
{
  if ( m_mod_name != modname ){
    m_mod_loaded = false;
    m_mod_exists = usync()->ModExists( modname );
    m_mod_name = modname;
  }
}


void IBattle::SetMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_mod.name ) {
    m_mod = mod;
    m_mod_name = mod.name;
    m_mod_loaded = true;
    m_mod_exists = usync()->ModExists( m_mod_name );
  }
}


const UnitSyncMod& IBattle::Mod()
{
  ASSERT_LOGIC( m_mod_exists, _T("Mod does not exist.") );
  if ( !m_mod_loaded ) {
    try {
      m_mod = usync()->GetMod( m_mod_name );
      m_mod_loaded = true;
      m_mod_name = m_mod.name;
    } catch (...) {}
  }
  return m_mod;
}


wxString IBattle::GetModName()
{
  return m_mod_name;
}


wxString IBattle::GetModHash()
{
  return m_mod.hash;
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
  m_mod_exists = usync()->ModExists( m_mod_name );
  m_map_exists = usync()->MapExists( m_map_name );
}

unsigned int IBattle::AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll ) {
  return (unsigned int)(-1);/// note: that looks pretty crappy and needs to be investigated.
}


