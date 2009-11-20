/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>
#include <wx/log.h>

#include "singleplayerbattle.h"
#include "mainsingleplayertab.h"
#include "server.h"
#include "ui.h"
#include "settings.h"
#include "spring.h"
#include "springunitsynclib.h"
#include "utils/conversion.h"

SinglePlayerBattle::SinglePlayerBattle( MainSinglePlayerTab& msptab ):
  m_sptab(msptab),
  m_me( User( usync().IsLoaded() ? usync().GetDefaultNick() : _T("invalid") ) )
{
	OnUserAdded( m_me );
	m_me.BattleStatus().colour = sett().GetBattleLastColour();
  CustomBattleOptions().setSingleOption( _T("startpostype"), wxString::Format(_T("%d"), ST_Pick), OptionsWrapper::EngineOption );
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
    SetLocalMap( usync().GetMapEx( usync().GetMapIndex( m_host_map.name ) ) );
    CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, GetHostMapName() );
    m_sptab.ReloadMapOptContrls();
    Update(  wxString::Format(_T("%d_%s"), OptionsWrapper::PrivateOptions , _T("mapname") ) );
  }
  if ( (update & HI_Mod_Changed) != 0 )
  {
    RemoveUnfittingBots();
    CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, GetHostModName() );
    wxString presetname = sett().GetModDefaultPresetName( GetHostModName() );
    if ( !presetname.IsEmpty() )
    {
      LoadOptionsPreset( presetname );
      SendHostInfo( HI_Send_All_opts );
    }
    m_sptab.ReloadModOptContrls();
    Update(  wxString::Format(_T("%d_%s"), OptionsWrapper::PrivateOptions , _T("modname") ) );
  }
  if ( (update & HI_Send_All_opts) != 0 )
  {
    for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
    {
      const std::map<wxString,wxString>& options = CustomBattleOptions().getOptionsMap( (OptionsWrapper::GameOption)i );
      for ( std::map<wxString,wxString>::const_iterator itor = options.begin(); itor != options.end(); ++itor )
      {
        Update(  wxString::Format(_T("%d_%s"), i , itor->first.c_str() ) );
      }
    }
  }
}

void SinglePlayerBattle::RemoveUnfittingBots()
{
    while ( GetNumBots() > 0 ) {
        User& u = m_internal_bot_list.begin()->second;
        KickPlayer( u ); // remove all bots
    }
}

void SinglePlayerBattle::Update( const wxString& Tag )
{
  m_sptab.Update( Tag );
}

void SinglePlayerBattle::StartSpring()
{
	spring().Run( *this );
	ui().OnSpringStarting();
}

/** @brief StartSpring
  *
  * @todo: document this function
  */
void NoGuiSinglePlayerBattle::StartSpring()
{
	spring().Run( *this );
	ui().OnSpringStarting();
}

/** @brief NoGuiSinglePlayerBattle
  *
  * @todo: document this function
  */
 NoGuiSinglePlayerBattle::NoGuiSinglePlayerBattle()
    :  m_me( User( usync().IsLoaded() ? usync().GetDefaultNick() : _T("invalid") ) )
{
	OnUserAdded( m_me );
	m_me.BattleStatus().colour = sett().GetBattleLastColour();
}

int NoGuiSinglePlayerBattle::GetAiIndex( const wxString& name )
{
    int total = susynclib().GetSkirmishAICount( m_host_mod.name );
    for ( int i = 0; i < total; i++ )
    {
        wxArrayString infos = susynclib().GetAIInfo( i );
        int namepos = infos.Index( _T("shortName") );
//        int versionpos = infos.Index( _T("version") );
        wxString ainame;
        if ( namepos != wxNOT_FOUND )
            ainame += infos[namepos +1];
        if ( ainame == name )
            return i;
//        if ( versionpos != wxNOT_FOUND ) ainame += _T(" ") + infos[versionpos +1];
    }
    return -1;
}

int NoGuiSinglePlayerBattle::GetSideIndex( const wxString& name )
{
    wxArrayString sides = usync().GetSides( m_host_mod.name );
    for ( int i = 0; i < (int)sides.Count(); ++i ) {
        if ( name.CmpNoCase( sides[i] ) == 0 )
            return i;
    }
    return -1;
}

bool NoGuiSinglePlayerBattle::AddBot( const wxString& name, int ai_team_id, const wxString& side )
{
    int ai_id = GetAiIndex( name );
    if ( ai_id > -1 ) {
        UserBattleStatus bs;
        bs.owner = GetMe().GetNick();
        bs.aishortname = name;
        bs.aitype = ai_id;
        bs.team = ai_team_id;
        bs.ally = GetFreeAlly();
        bs.colour = GetNewColour();
        bs.side = GetSideIndex( side );
        //first arg is nick
        User& bot = OnBotAdded( name + _T("_") + TowxString( bs.team ), bs  );
        return true;
    }
    return false;
}
