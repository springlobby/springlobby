#ifndef SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
#define SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H

#include <wx/imaglist.h>
#include <wx/colour.h>

#include <map>


class Battle;
struct UserStatus;

class IconImageList : public wxImageList
{
  public:
    IconImageList();

    static int GetUserListStateIcon( const UserStatus& us, bool chanop, bool inbroom );
    static int GetUserBattleStateIcon( const UserStatus& us );

    static int GetRankIcon( const int& rank, const bool& showlowest = true );
    static int GetFlagIcon( const wxString& flagname );
    static int GetBattleStatusIcon( Battle& battle );
    static wxString GetBattleStatus(Battle& battle);
    static int GetHostIcon( const bool& spectator = false );
    int GetColourIcon( const int& num );
    void SetColourIcon( const int& num, const wxColour& colour );
    int GetSideIcon( const wxString& modname, const wxString& side );
    static int GetReadyIcon( const bool& ready, const int& sync = 1 );

    static int ICON_NONE;

    static int ICON_ADMIN;
    static int ICON_ADMIN_AWAY;
    static int ICON_ADMIN_BROOM;
    static int ICON_ADMIN_INGAME;

    static int ICON_BOT;
    static int ICON_BOT_BROOM;
    static int ICON_BOT_INGAME;

    static int ICON_NOSTATE;
    static int ICON_AWAY;
    static int ICON_BROOM;
    static int ICON_INGAME;

    static int ICON_OP;
    static int ICON_OP_AWAY;
    static int ICON_OP_BROOM;
    static int ICON_OP_INGAME;

    static int ICON_UP;
    static int ICON_DOWN;

    static int ICON_RANK_NONE;
    static int ICON_RANK0;
    static int ICON_RANK1;
    static int ICON_RANK2;
    static int ICON_RANK3;
    static int ICON_RANK4;
    static int ICON_RANK5;
    static int ICON_RANK6;

    static int ICON_GAME_UNKNOWN;
    static int ICON_OPEN_GAME;
    static int ICON_OPEN_PW_GAME;
    static int ICON_CLOSED_GAME;
    static int ICON_CLOSED_PW_GAME;
    static int ICON_STARTED_GAME;

    static int ICON_READY_UNSYNC;
    static int ICON_NREADY_UNSYNC;
    static int ICON_READY_QSYNC;
    static int ICON_NREADY_QSYNC;
    static int ICON_NREADY;
    static int ICON_READY;

    static int ICON_NEXISTS;
    static int ICON_EXISTS;

    static int ICON_SPECTATOR;
    static int ICON_HOST;
    static int ICON_HOST_SPECTATOR;

    static int ICON_ARM;
    static int ICON_CORE;

    static int ICON_UNK_FLAG;
    static int ICON_FLAGS_BASE;

    static int ICON_EMPTY;

  protected:
    std::map<wxString, int> m_cached_side_icons;
    // why map? because i already included and didn't want to include more stuff, it's not time-critical code anyway
    std::map<unsigned int, unsigned int> m_player_colour_icons;
};

IconImageList& icons();

#endif // SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
