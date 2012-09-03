#include "noguisingleplayerbattle.h"

#include "settings.h"
#include "spring.h"
#include <lslunitsync/c_api.h>
#include <lslunitsync/unitsync.h>

/** @brief StartSpring
  *
  * @todo: document this function
  */
void NoGuiSinglePlayerBattle::StartSpring()
{
    spring().Run( *this );
    //	ui().OnSpringStarting(); //stops torrent system
}

/** @brief NoGuiSinglePlayerBattle
  *
  * @todo: document this function
  */
NoGuiSinglePlayerBattle::NoGuiSinglePlayerBattle()
    :  m_me( User( LSL::usync().IsLoaded() ? LSL::usync().GetDefaultNick() : _T("invalid") ) )
{
    OnUserAdded( m_me );
    m_me.BattleStatus().colour = sett().GetBattleLastColour();
    SetFounder( m_me.GetNick() );
}

int NoGuiSinglePlayerBattle::GetAiIndex( const wxString& name )
{
    int total = LSL::susynclib().GetSkirmishAICount( m_host_mod.name );
    for ( int i = 0; i < total; i++ )
    {
        const wxArrayString infos = LSL::susynclib().GetAIInfo( i );
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
    const auto sides = LSL::usync().GetSides( m_host_mod.name );
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
