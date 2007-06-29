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

#include "images/nexists.xpm"
#include "images/exists.xpm"

#include "images/unknown_flag.xpm"

#include "flagimages.h"

IconImageList::IconImageList() : wxImageList(16,16)
{
  
  Add( wxBitmap(admin_xpm) );
  Add( wxBitmap(admin_away_xpm) );
  Add( wxBitmap(admin_ingame_xpm) );
  Add( wxBitmap(bot_xpm) );
  Add( wxBitmap(away_xpm) );
  Add( wxBitmap(ingame_xpm) );
  Add( wxBitmap(up_xpm) );
  Add( wxBitmap(down_xpm) );
  Add( wxBitmap(rank0_xpm) );
  Add( wxBitmap(rank1_xpm) );
  Add( wxBitmap(rank2_xpm) );
  Add( wxBitmap(rank3_xpm) );
  Add( wxBitmap(rank4_xpm) );
  
  Add( wxBitmap(open_game_xpm) );
  Add( wxBitmap(open_pw_game_xpm) );
  Add( wxBitmap(closed_game_xpm) );
  Add( wxBitmap(closed_pw_game_xpm) );
  Add( wxBitmap(started_game_xpm) );
  
  Add( wxBitmap(nexists_xpm) );
  Add( wxBitmap(exists_xpm) );

  Add( wxBitmap(unknown_flag_xpm) );

  AddFlagImages( *this );

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


int IconImageList::GetColorIcon( const int& num )
{
  return -1;
}

int IconImageList::GetSideIcon( const int& sidenum )
{
  return -1;
}

int IconImageList::GetReadyIcon( const bool& ready )
{
  if ( ready ) return ICON_READY;
  return ICON_NREADY;
}
