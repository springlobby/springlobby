/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>

#include "ibattle.h"
#include "utils.h"

IBattle::IBattle():
  m_map_loaded(false),
  m_mod_loaded(false),
  m_map_exists(false),
  m_mod_exists(false),

  m_startmetal(1000),
  m_startenergy(1000),
  m_maxunits(500),

  m_limitdgun(false),
  m_dimmms(false),
  m_ghostedbuildings(true),

  m_gametype(GT_ComContinue),
  m_starttype(ST_Fixed)
{
}


IBattle::~IBattle()
{
}


void IBattle::SetMap(const wxString& mapname, const wxString& hash)
{
  if ( mapname != WX_STRING(m_map.name) ) {
    m_map_loaded = false;
    m_map_exists = usync()->MapExists( STD_STRING(mapname), STD_STRING(hash) );
    m_map.hash = STD_STRING(hash);
    m_map_name = mapname;
  }
}


void IBattle::SetMap(const UnitSyncMap& map)
{
  if ( map.name != m_map.name ) {
    m_map = map;
    m_map_name = WX_STRING(map.name);
    m_map_loaded = true;
    m_map_exists = usync()->MapExists( STD_STRING(m_map_name), m_map.hash );
  }
}


const UnitSyncMap& IBattle::Map()
{
  ASSERT_LOGIC( m_map_exists, _T("Map does not exist.") );

  if ( !m_map_loaded ) {
    try {

      m_map = usync()->GetMapEx( STD_STRING(m_map_name) );
      m_map_loaded = true;
      m_map_name = WX_STRING(m_map.name);

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
  if ( m_map.hash == "" ) {
    try {
      m_map.hash = usync()->GetMap( m_map.name ).hash;
    } catch (...) { wxLogWarning( _T("Couldn't get map hash from unitsync.") ); }
  }
  return WX_STRING(m_map.hash);
}


void IBattle::SetMod( const wxString& modname, const wxString& hash )
{
  if ( m_mod_name != modname ){
    m_mod_loaded = false;
    m_mod_exists = usync()->ModExists( STD_STRING(modname) );
    m_mod_name = modname;
  }
}


void IBattle::SetMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_mod.name ) {
    m_mod = mod;
    m_mod_name = WX_STRING(mod.name);
    m_mod_loaded = true;
    m_mod_exists = usync()->ModExists( STD_STRING(m_mod_name) );
  }
}


const UnitSyncMod& IBattle::Mod()
{
  ASSERT_LOGIC( m_mod_exists, _T("Mod does not exist.") );
  if ( !m_mod_loaded ) {
    try {
      m_mod = usync()->GetMod( STD_STRING(m_mod_name) );
      m_mod_loaded = true;
      m_mod_name = WX_STRING(m_mod.name);
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
  return WX_STRING(m_mod.hash);
}


void IBattle::SetGameType(GameType gt)
{
  m_gametype = gt;
}


GameType IBattle::GetGameType()
{
  return m_gametype;
}


void IBattle::SetStartType(StartType st)
{
  m_starttype = st;
}


StartType IBattle::GetStartType()
{
  return m_starttype;
}


bool IBattle::MapExists()
{
  return m_map_exists;
  //return usync()->MapExists( STD_STRING(m_map_name), m_map.hash );
}


bool IBattle::ModExists()
{
  return m_mod_exists;
  //return usync()->ModExists( STD_STRING(m_mod_name) );
}



void IBattle::DisableUnit( const std::string& unitname )
{
  if ( m_units.Index( WX_STRING(unitname) ) == wxNOT_FOUND ) m_units.Add( WX_STRING( unitname ) );
}


void IBattle::EnableUnit( const std::string& unitname )
{
  int pos = m_units.Index( WX_STRING(unitname) );
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
  m_mod_exists = usync()->ModExists( STD_STRING(m_mod_name) );
  m_map_exists = usync()->MapExists( STD_STRING(m_map_name) );
}

unsigned int IBattle::AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll ) {
  return (unsigned int)(-1);/// note: that looks pretty crappy and needs to be investigated.
}


