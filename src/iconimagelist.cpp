//
// Class: IconImageList
//

#include <stdexcept>

#include <wx/image.h>

#include "iconimagelist.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "iunitsync.h"

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

#include "images/ready_unsync.xpm"
#include "images/ready_q.xpm"
#include "images/nready_unsync.xpm"
#include "images/nready_q.xpm"

#include "images/spectator.xpm"

#include "images/arm.xpm"
#include "images/core.xpm"

#include "images/colourbox.xpm"

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

  Add( wxBitmap(ready_unsync_xpm) );
  Add( wxBitmap(ready_q_xpm) );
  Add( wxBitmap(nready_unsync_xpm) );
  Add( wxBitmap(nready_q_xpm) );

  Add( wxBitmap(nexists_xpm) );
  Add( wxBitmap(exists_xpm) );

  Add( wxBitmap(spectator_xpm) );

  Add( wxBitmap(arm_xpm) );
  Add( wxBitmap(core_xpm) );

  for ( int i = 0; i < NUM_COLOUR_ICONS; i++ ) Add( wxBitmap(colourbox_xpm) );

  SetColourIcon( 0, wxColour( 255,   0,   0 ) );
  SetColourIcon( 1, wxColour(   0, 255,   0 ) );
  SetColourIcon( 2, wxColour(   0,   0, 255 ) );

  Add( wxBitmap(unknown_flag_xpm) );

  AddFlagImages( *this );

}


IconImageList& icons()
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
  if ( rank <= RANK_3 ) return ICON_RANK3;
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


int IconImageList::GetColourIcon( const int& num )
{
  ASSERT_LOGIC( num < NUM_COLOUR_ICONS, "Colour index too high" );
  return ICON_COLOURS_START + num;
}


void IconImageList::SetColourIcon( const int& num, const wxColour& colour )
{
  int index = GetColourIcon( num );
  wxImage img( colourbox_xpm );

  img.Replace( 1, 1, 1, colour.Red(), colour.Green(), colour.Blue() );

  int r,g,b;
  r = colour.Red()+80; g = colour.Green()+80; b = colour.Blue()+80;
  img.Replace( 2, 2, 2, r>255?255:r, g>255?255:g, b>255?255:b );

  /*r = colour.Red()-60; g = colour.Green()-60; b = colour.Blue()-60;
  img.Replace( 200, 200, 200, r<0?0:r, g<0?0:g, b<0?0:b );*/

  Replace( index, wxBitmap( img ) );
}


int IconImageList::GetSideIcon( const std::string& side )
{
  wxString sn = WX_STRING( side );
  sn = sn.Lower();
  if ( sn  == _T("arm") ) return ICON_ARM;
  else if (  sn == _T("core") ) return ICON_CORE;
  else if (ChachedSideIcons[side] == 0){
      int IconPosition = Add(wxBitmap( usync()->GetSidePicture(side) ), wxNullBitmap);
      ChachedSideIcons[side] = IconPosition;
      return IconPosition;
  }
  else return ChachedSideIcons[side];
  return -1;
}

int IconImageList::GetReadyIcon( const bool& ready, const int& sync )
{
  if ( ready ) {
    if ( sync == SYNC_SYNCED ) return ICON_READY;
    else if ( sync == SYNC_UNSYNCED ) return ICON_READY_UNSYNC;
    else return ICON_READY_QSYNC;
  } else {
    if ( sync == SYNC_SYNCED ) return ICON_NREADY;
    else if ( sync == SYNC_UNSYNCED ) return ICON_NREADY_UNSYNC;
    else return ICON_NREADY_QSYNC;
  }
}
