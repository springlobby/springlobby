/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "ibattle.h"
#include "utils.h"

IBattle::IBattle():
  m_map_loaded(false),
  m_mod_loaded(false),
  m_map_exists(false),
  m_mod_exists(false),
  m_gametype(GT_ComContinue),
  m_starttype(ST_Fixed)
{
}


IBattle::~IBattle()
{
}


void IBattle::SetMap(const wxString& mapname, const wxString& hash)
{
  m_map_loaded = false;
  m_map_exists = usync()->MapExists( STD_STRING(mapname), STD_STRING(hash) );
  m_map_name = mapname;
}


void IBattle::SetMap(const UnitSyncMap& map)
{
  m_map = map;
  m_map_name = WX_STRING(map.name);
  m_map_loaded = true;
  m_map_exists = usync()->MapExists( STD_STRING(m_map_name), m_map.hash );
}


const UnitSyncMap& IBattle::Map()
{
  if ( !m_map_loaded ) {
    try {

      m_map = usync()->GetMap( STD_STRING(m_map_name), true );
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
    m_map.hash = usync()->GetMap( m_map.name ).hash;
  }
  return WX_STRING(m_map.hash);
}


void IBattle::SetMod( const wxString& modname, const wxString& hash )
{
  m_mod_loaded = false;
  m_mod_exists = usync()->ModExists( STD_STRING(modname) );
  m_mod_name = modname;
}


void IBattle::SetMod( const UnitSyncMod& mod )
{
  m_mod = mod;
  m_mod_name = WX_STRING(mod.name);
  m_mod_loaded = true;
  m_mod_exists = usync()->ModExists( STD_STRING(m_mod_name) );
}


const UnitSyncMod& IBattle::Mod()
{
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
}


bool IBattle::ModExists()
{
  return m_mod_exists;
}

