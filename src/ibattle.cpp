/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>
#include <wx/tokenzr.h>
#include <wx/log.h>

#include "ibattle.h"
#include "utils.h"
#include "settings.h"
#include "ui.h"

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
  if ( mapname != m_host_map.name || hash != m_host_map.hash ) {
    m_map_loaded = false;
    m_host_map.name = mapname;
    m_host_map.hash = hash;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
    if ( m_map_exists ) usync().PrefetchMap( m_host_map.name );
  }
}


void IBattle::SetLocalMap(const UnitSyncMap& map)
{
  if ( map.name != m_local_map.name || map.hash != m_local_map.hash ) {
    m_local_map = map;
    m_map_loaded = true;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
    if ( m_map_exists ) usync().PrefetchMap( m_host_map.name );
  }
}


const UnitSyncMap& IBattle::LoadMap()
{

  if ( !m_map_loaded ) {
    try {
      ASSERT_EXCEPTION( m_map_exists, _T("Map does not exist.") );
      m_local_map = usync().GetMapEx( m_host_map.name );
      m_map_loaded = true;

    } catch (...) {}
  }
  return m_local_map;
}


wxString IBattle::GetHostMapName() const
{
  return m_host_map.name;
}


wxString IBattle::GetHostMapHash() const
{
  return m_host_map.hash;
}


void IBattle::SetHostMod( const wxString& modname, const wxString& hash )
{
  if ( m_host_mod.name != modname || m_host_mod.hash != hash ){
    m_mod_loaded = false;
    m_host_mod.name = modname;
    m_host_mod.hash = hash;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


void IBattle::SetLocalMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_local_mod.name || mod.hash != m_local_mod.hash ) {
    m_local_mod = mod;
    m_mod_loaded = true;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


const UnitSyncMod& IBattle::LoadMod()
{
  if ( !m_mod_loaded ) {
    try {
      ASSERT_EXCEPTION( m_mod_exists, _T("Mod does not exist.") );
      m_local_mod = usync().GetMod( m_host_mod.name );
      m_mod_loaded = true;
    } catch (...) {}
  }
  return m_local_mod;
}


wxString IBattle::GetHostModName() const
{
  return m_host_mod.name;
}


wxString IBattle::GetHostModHash() const
{
  return m_host_mod.hash;
}


bool IBattle::MapExists()
{
  return m_map_exists;
  //return usync().MapExists( m_map_name, m_map.hash );
}


bool IBattle::ModExists()
{
  return m_mod_exists;
  //return usync().ModExists( m_mod_name );
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
  if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash);
  else m_mod_exists = usync().ModExists( m_host_mod.name );
  if ( !m_host_map.hash.IsEmpty() )  m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
  else  m_map_exists = usync().MapExists( m_host_map.name );
}


unsigned int IBattle::AddBot( int ally, int posx, int posy, int handicap, const wxString& aidll )
{
  return (unsigned int)(-1);/// note: that looks pretty crappy and needs to be investigated.
}


static wxString FixPresetName( const wxString& name )
{
  // look name up case-insensitively
  const wxArrayString& presetList = sett().GetPresetList();
  int index = presetList.Index( name, false /*case insensitive*/ );
  if ( index == -1 ) return _T("");

  // set preset to the actual name, with correct case
  return presetList[index];
}


bool IBattle::LoadOptionsPreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if (preset == _T("")) return false; ///preset not found
  m_preset = preset;

  for ( unsigned int i = 0; i < OptionsWrapper::LastOption; i++)
  {
    std::map<wxString,wxString> options = sett().GetHostingPreset( m_preset, i );
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      for ( std::map<wxString,wxString>::iterator itor = options.begin(); itor != options.end(); itor++ )
      {
        CustomBattleOptions().setSingleOption( itor->first, itor->second, (OptionsWrapper::GameOption)i );
      }
    }
    else
    {
      if ( !options[_T("mapname")].IsEmpty() )
      {
        UnitSyncMap map = usync().GetMapEx( options[_T("mapname")] );
        SetLocalMap( map );

        SendHostInfo( HI_Map );
      }
      unsigned int localrectcount = GetNumRects();
      for( unsigned int localrect = 0 ; localrect < localrectcount; ++localrect) if ( GetStartRect( localrect ).exist ) RemoveStartRect( localrect );
      SendHostInfo( HI_StartRects );

      unsigned int rectcount = s2l( options[_T("numrects")] );
      for ( unsigned int loadrect = 0; loadrect < rectcount; loadrect++)
      {
        int ally = s2l(options[_T("rect_") + TowxString(loadrect) + _T("_ally")]);
        if ( ally == 0 ) continue;
        AddStartRect( ally - 1, s2l(options[_T("rect_") + TowxString(loadrect) + _T("_left")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_top")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_right")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_bottom")]) );
      }
      SendHostInfo( HI_StartRects );

      m_units = wxStringTokenize( options[_T("restrictions")], _T('\t') );
      SendHostInfo( HI_Restrictions );
      Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );

    }
  }
  SendHostInfo( HI_Send_All_opts );
  ui().ReloadPresetList();
  return true;
}


void IBattle::SaveOptionsPreset( const wxString& name )
{
  m_preset = FixPresetName(name);
  if (m_preset == _T("")) m_preset = name; ///new preset

  for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
  {
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, CustomBattleOptions().getOptionsMap( (OptionsWrapper::GameOption)i ) );
    }
    else
    {
      std::map<wxString,wxString> opts;
      opts[_T("mapname")] = GetHostMapName();
      unsigned int validrectcount = 0;
      if ( s2l (CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) ) == ST_Choose )
      {
        unsigned int boxcount = GetNumRects();
        for ( unsigned int boxnum = 0; boxnum < boxcount; boxnum++ )
        {
          BattleStartRect rect = GetStartRect( boxnum );
          if ( rect.IsOk() )
          {
            opts[_T("rect_") + TowxString(validrectcount) + _T("_ally")] = TowxString( rect.ally + 1 );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_left")] = TowxString( rect.left );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_top")] = TowxString( rect.top );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_bottom")] = TowxString( rect.bottom );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_right")] = TowxString( rect.right );
            validrectcount++;
          }
        }
      }
      opts[_T("numrects")] = TowxString( validrectcount );

      unsigned int restrcount = m_units.GetCount();
      wxString restrictionsstring;
      for ( unsigned int restrnum = 0; restrnum < restrcount; restrnum++ )
      {
        restrictionsstring << m_units[restrnum] << _T('\t');
      }
      opts[_T("restrictions")] = restrictionsstring;

      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, opts );
    }
  }
  sett().SaveSettings();
  ui().ReloadPresetList();
}


wxString IBattle::GetCurrentPreset()
{
  return m_preset;
}


void IBattle::DeletePreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if ( m_preset == preset ) m_preset = _T("");
  sett().DeletePreset( preset );
  ui().ReloadPresetList();
}


wxArrayString IBattle::GetPresetList()
{
  return sett().GetPresetList();
}
