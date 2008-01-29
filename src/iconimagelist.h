#ifndef SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
#define SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H

#include <wx/imaglist.h>
#include <wx/colour.h>

#include <map>

#define ICON_NONE -1

#define ICON_ADMIN 0
#define ICON_ADMIN_AWAY ICON_ADMIN+1
#define ICON_ADMIN_BROOM ICON_ADMIN+2
#define ICON_ADMIN_INGAME ICON_ADMIN+3

#define ICON_BOT ICON_ADMIN+4
#define ICON_BOT_BROOM ICON_BOT+1
#define ICON_BOT_INGAME ICON_BOT+2

#define ICON_NOSTATE -1
#define ICON_AWAY ICON_BOT+3
#define ICON_BROOM ICON_AWAY+1
#define ICON_INGAME ICON_AWAY+2

#define ICON_OP ICON_AWAY+3
#define ICON_OP_AWAY ICON_OP+1
#define ICON_OP_BROOM ICON_OP+2
#define ICON_OP_INGAME ICON_OP+3

#define ICON_UP ICON_OP+4
#define ICON_DOWN ICON_UP+1

#define ICON_RANK_NONE -1
#define ICON_RANK0 ICON_UP+2
#define ICON_RANK1 ICON_RANK0+1
#define ICON_RANK2 ICON_RANK0+2
#define ICON_RANK3 ICON_RANK0+3
#define ICON_RANK4 ICON_RANK0+4
#define ICON_RANK5 ICON_RANK0+5
#define ICON_RANK6 ICON_RANK0+6

#define ICON_GAME_UNKNOWN -1
#define ICON_OPEN_GAME ICON_RANK0+7
#define ICON_OPEN_PW_GAME ICON_OPEN_GAME+1
#define ICON_CLOSED_GAME ICON_OPEN_GAME+2
#define ICON_CLOSED_PW_GAME ICON_OPEN_GAME+3
#define ICON_STARTED_GAME ICON_OPEN_GAME+4

#define ICON_READY_UNSYNC ICON_OPEN_GAME+5
#define ICON_NREADY_UNSYNC ICON_READY_UNSYNC+1
#define ICON_READY_QSYNC ICON_READY_UNSYNC+2
#define ICON_NREADY_QSYNC ICON_READY_UNSYNC+3

#define ICON_NEXISTS ICON_READY_UNSYNC+4
#define ICON_EXISTS ICON_NEXISTS+1

#define ICON_SPECTATOR ICON_NEXISTS+2
#define ICON_HOST ICON_SPECTATOR+1
#define ICON_HOST_SPECTATOR ICON_SPECTATOR+2

#define ICON_ARM ICON_SPECTATOR+3
#define ICON_CORE ICON_ARM+1

#define ICON_UNK_FLAG ICON_CORE +1
#define ICON_FLAGS_BASE ICON_UNK_FLAG+1

#define ICON_EMPTY ICON_FLAGS_BASE+243+1

// Icon Aliases
#define ICON_READY ICON_OPEN_GAME
#define ICON_NREADY ICON_CLOSED_GAME


class Battle;
struct UserStatus;

class IconImageList : public wxImageList
{
  public:
    IconImageList();

    static int GetUserListStateIcon( const UserStatus& us, bool chanop, bool inbroom );
    static int GetUserBattleStateIcon( const UserStatus& us );

    static int GetRankIcon( const int& rank, const bool& showlowest = true );
    static int GetFlagIcon( const std::string& flagname );
    static int GetBattleStatusIcon( Battle& battle );
    static wxString GetBattleStatus(Battle& battle);
    static int GetHostIcon( const bool& spectator = false );
    unsigned int GetColourIcon( const int& num );
    void SetColourIcon( const int& num, const wxColour& colour );
    int GetSideIcon( const std::string& modname, const std::string& side );
    static int GetReadyIcon( const bool& ready, const int& sync = 1 );

  protected:
    std::map<std::string, int> m_cached_side_icons;
    // why map? because i already included and didn't want to include more stuff, it's not time-critical code anyway
    std::map<unsigned int, unsigned int> m_player_colour_icons;
};

IconImageList& icons();

#endif // SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
