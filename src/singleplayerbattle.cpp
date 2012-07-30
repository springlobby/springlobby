/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>
#include <wx/log.h>

#include "singleplayerbattle.h"
#include "hosting/mainsingleplayertab.h"
#include "server.h"
#include "ui.h"
#include "settings.h"
#include "spring.h"
#include <lslunitsync/c_api.h>
#include "utils/conversion.h"

SinglePlayerBattle::SinglePlayerBattle( MainSinglePlayerTab& msptab ):
  m_sptab(msptab),
  m_me( User( LSL::usync().IsLoaded() ? LSL::usync().GetDefaultNick() : _T("invalid") ) )
{
	OnUserAdded( m_me );
	m_me.BattleStatus().side = sett().GetBattleLastSideSel( GetHostModName() );
	m_me.BattleStatus().colour = sett().GetBattleLastColour();
	CustomBattleOptions().setSingleOption( _T("startpostype"), wxFormat(_T("%d") ) % ST_Pick, LSL::OptionsWrapper::EngineOption );
}


SinglePlayerBattle::~SinglePlayerBattle()
{

}


void SinglePlayerBattle::SendHostInfo( HostInfo update )
{
  if ( (update & HI_StartType) != 0 ) m_sptab.UpdateMinimap();
  if ( (update & HI_Restrictions) != 0 ) m_sptab.ReloadRestrictions();
  if ( (update & HI_Map_Changed) != 0 )
  {
	LoadMap();
    m_sptab.ReloadMapOptContrls();
	Update(  wxFormat(_T("%d_%s") ) % LSL::OptionsWrapper::PrivateOptions % _T("mapname") );
  }
  if ( (update & HI_Mod_Changed) != 0 )
  {
    RemoveUnfittingBots();
	LoadMod();
    wxString presetname = sett().GetModDefaultPresetName( GetHostModName() );
    if ( !presetname.IsEmpty() )
    {
      LoadOptionsPreset( presetname );
      SendHostInfo( HI_Send_All_opts );
    }
    m_sptab.ReloadModOptContrls();
	Update(  wxFormat(_T("%d_%s") ) % LSL::OptionsWrapper::PrivateOptions % _T("modname") );
  }
  if ( (update & HI_Send_All_opts) != 0 )
  {
    for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
    {
      const std::map<wxString,wxString>& options = CustomBattleOptions().getOptionsMap( (LSL::OptionsWrapper::GameOption)i );
      for ( std::map<wxString,wxString>::const_iterator itor = options.begin(); itor != options.end(); ++itor )
      {
		Update(  wxFormat(_T("%d_%s") ) % i % itor->first );
      }
    }
  }
}

void SinglePlayerBattle::RemoveUnfittingBots()
{
    wxArrayString old_ais = LSL::usync().GetAIList( m_previous_local_mod_name );
    wxArrayString new_ais = LSL::usync().GetAIList( m_local_mod.name );
    for ( size_t i = 0; i < old_ais.GetCount(); ++i) {
        if ( new_ais.Index(old_ais[i]) == wxNOT_FOUND  ) {
            for( size_t j = 0; j < GetNumUsers(); ++j  ) {
                User& u = GetUser( j );
                if ( u.GetBattleStatus().airawname == old_ais[i] )
                    KickPlayer( u );
            }
        }
    }
}

void SinglePlayerBattle::Update( const wxString& Tag )
{
  m_sptab.UpdateTag( Tag );
}

void SinglePlayerBattle::StartSpring()
{
	spring().Run( *this );
	ui().OnSpringStarting();
}


