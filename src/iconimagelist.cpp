//
// Class: IconImageList
//

#include "iconimagelist.h"
#include "user.h"
#include "battle.h"


#include "images/bot.xpm"
#include "images/admin.xpm"
#include "images/admin_ingame.xpm"
#include "images/admin_away.xpm"
#include "images/away.xpm"
#include "images/ingame.xpm"
#include "images/up.xpm"
#include "images/down.xpm"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"

#include "images/open_game.xpm"
#include "images/open_pw_game.xpm"
#include "images/closed_game.xpm"
#include "images/closed_pw_game.xpm"
#include "images/started_game.xpm"

#include "images/unknown_flag.xpm"

#include "flagimages.h"

IconImageList::IconImageList() : wxImageList(16,16)
{
  
  Add( wxBITMAP(admin) );
  Add( wxBITMAP(admin_away) );
  Add( wxBITMAP(admin_ingame) );
  Add( wxBITMAP(bot) );
  Add( wxBITMAP(away) );
  Add( wxBITMAP(ingame) );
  Add( wxBITMAP(up) );
  Add( wxBITMAP(down) );
  Add( wxBITMAP(rank0) );
  Add( wxBITMAP(rank1) );
  Add( wxBITMAP(rank2) );
  Add( wxBITMAP(rank3) );
  Add( wxBITMAP(rank4) );
  
  Add( wxBITMAP(open_game) );
  Add( wxBITMAP(open_pw_game) );
  Add( wxBITMAP(closed_game) );
  Add( wxBITMAP(closed_pw_game) );
  Add( wxBITMAP(started_game) );
  
  Add( wxBITMAP(unknown_flag) );

  ADD_FLAGS_IMAGES(this)

}


IconImageList::~IconImageList()
{
  
}


wxImageList& icons()
{
  static IconImageList m_icons;
  return m_icons;
}


int IconImageList::GetUserStateIcon( const UserStatus& us )
{
  if ( us.bot ) return ICON_BOT;
  if (us.moderator ) {
    if ( us.in_game ) return ICON_ADMIN_INGAME;
    if ( us.away ) return ICON_ADMIN_AWAY;
    return ICON_ADMIN;
  }
  
  if ( us.in_game ) return ICON_INGAME;
  if ( us.away ) return ICON_AWAY;
  
  return ICON_NOSTATE;
}


int IconImageList::GetRankIcon( const int& rank, const bool& showlowest )
{
  if ( rank <= RANK_0 ) {
    if ( showlowest ) return ICON_RANK0;
    else return ICON_RANK_NONE;
  }
  if ( rank <= RANK_1 ) return ICON_RANK1;
  if ( rank <= RANK_2 ) return ICON_RANK2;
  if ( rank <= RANK_3 ) return ICON_RANK4;
  return ICON_RANK4;
}


int IconImageList::GetFlagIcon( const std::string& flagname )
{
  return ICON_FLAGS_BASE + GetFlagIndex( flagname );
}


int IconImageList::GetBattleStatusIcon( Battle& battle )
{
  if ( battle.GetFounder().GetStatus().in_game ) return ICON_STARTED_GAME;
  if ( !battle.opts().islocked ) {
    if ( !battle.opts().ispassworded ) return ICON_OPEN_GAME;
    else return ICON_OPEN_PW_GAME;
  } else {
    if ( !battle.opts().ispassworded ) return ICON_CLOSED_GAME;
    else return ICON_CLOSED_PW_GAME;
  }
  
  return ICON_GAME_UNKNOWN;
}
