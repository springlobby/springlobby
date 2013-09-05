/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>
#include <wx/log.h>

#include "singleplayerbattle.h"
#include "hosting/mainsingleplayertab.h"
#include "server.h"
#include "settings.h"
#include "spring.h"
#include <lslutils/conversion.h>
#include "utils/conversion.h"

SinglePlayerBattle::SinglePlayerBattle( MainSinglePlayerTab& msptab ):
  m_sptab(msptab),
  m_me( User( TowxString(LSL::usync().IsLoaded() ? LSL::usync().GetDefaultNick() : "invalid")))
{
	OnUserAdded( m_me );
	m_me.BattleStatus().side = sett().GetBattleLastSideSel( GetHostModName() );
	m_me.BattleStatus().colour = sett().GetBattleLastColour();
    CustomBattleOptions().setSingleOption( "startpostype", LSL::Util::ToString(ST_Pick), LSL::OptionsWrapper::EngineOption );
	ConnectGlobalEvent(this, GlobalEvent::OnUnitsyncReloaded, wxObjectEventFunction(&SinglePlayerBattle::OnUnitsyncReloaded));
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
    for ( int i = 0; i < (int)LSL::OptionsWrapper::LastOption; i++)
    {
      for (const auto pair : CustomBattleOptions().getOptionsMap( (LSL::OptionsWrapper::GameOption)i))
      {
        Update(  wxFormat(_T("%d_%s") ) % i % pair.first );
      }
    }
  }
}

void SinglePlayerBattle::RemoveUnfittingBots()
{
    const auto old_ais = LSL::usync().GetAIList(STD_STRING(m_previous_local_mod_name));
    const auto new_ais = LSL::usync().GetAIList(m_local_mod.name);
    LSL::StringVector diff(old_ais.size());
    LSL::StringVector::iterator end = std::set_difference(old_ais.begin(), old_ais.end(), new_ais.begin(), new_ais.end(),diff.begin());
    for(auto it = diff.begin(); it != end; ++it) {
        for( size_t j = 0; j < GetNumUsers(); ++j  ) {
            User& u = GetUser( j );
            if (u.GetBattleStatus().airawname == TowxString(*it))
                KickPlayer( u );
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
}


