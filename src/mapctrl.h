#ifndef SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPCTRL_H

#include <wx/string.h>

#include "ibattle.h"
#include "iunitsync.h"

class wxPanel;
class wxBitmap;
class wxDC;

class Battle;
struct BattleStartRect;
class SinglePlayerBattle;
class Ui;
struct UnitSyncMap;


typedef int RectArea;
#define RA_Main -1
#define RA_UpLeft 0
#define RA_UpRight 1
#define RA_DownRight 2
#define RA_DownLeft 3
#define RA_UpAllyButton 4
#define RA_DownAllyButton 5
#define RA_Side 6
#define RA_UpHandicapButton 7
#define RA_DownHandicapButton 8
#define RA_Handicap 9
#define RA_Close 10
#define RA_Move 11
#define RA_Download 12
#define RA_Refresh 13


typedef int MouseAction;
#define MA_None 0
#define MA_Add 1
#define MA_Delete 2
#define MA_Move 3
#define MA_ResizeUpLeft 4
#define MA_ResizeUpRight 5
#define MA_ResizeDownLeft 6
#define MA_ResizeDownRight 7


typedef int BotRectOrient;
#define BRO_TopLeft 0
#define BRO_BottomLeft 1
#define BRO_TopRight 2
#define BRO_BottomRight 3

class MapCtrl : public wxPanel
{
  public:
    MapCtrl( wxWindow* parent, int size, IBattle* battle, Ui& ui, bool readonly, bool fixed_size,
      bool draw_start_types, bool singleplayer );
    ~MapCtrl();

    void SetBattle( IBattle* battle );

    void LoadMinimap();
    void FreeMinimap();
    void UpdateMinimap();

    void OnPaint( wxPaintEvent& event );
    void OnResize( wxSizeEvent& event );

    void OnMouseMove( wxMouseEvent& event );
    void OnLeftDown( wxMouseEvent& event );
    void OnLeftUp( wxMouseEvent& event );
    void OnMouseWheel( wxMouseEvent& event );

   protected:

    BattleStartRect GetBattleRect( int x1, int y1, int x2, int y2, int ally = -1 );
    wxRect GetMinimapRect();
    wxRect GetStartRect( int index );
    wxRect GetStartRect( const BattleStartRect& sr );

    void DrawOutlinedText( wxDC& dc, const wxString& str, int x, int y, const wxColour& outline, const wxColour& font );

    wxRect GetBotRect( BattleBot& bot, bool selected );
    RectArea GetBotRectArea( const wxRect& botrect, int x, int y );

    wxRect GetBotSideRect() { return wxRect( 37, 20, 16, 16 ); }
    wxRect GetBotHandicapRect() { return wxRect( 40, 55, 16, 16 ); }
    wxRect GetBotCloseRect() { return wxRect( 59, 4, 14, 14 ); }
    wxRect GetBotUpAllyButtonRect() { return wxRect( 61, 35, 12, 8 ); }
    wxRect GetBotDownAllyButtonRect() { return wxRect( 61, 43, 12, 8 ); }
    wxRect GetBotUpHandicapButtonRect() { return wxRect( 61, 52, 12, 8 ); }
    wxRect GetBotDownHandicapButtonRect() { return wxRect( 61, 60, 12, 8 ); }

    wxRect GetRefreshRect();
    wxRect GetDownloadRect();

    int GetNewRectIndex();

    void RequireImages();

    void RelocateBots();

    void GetClosestStartPos( int fromx, int fromy, int& index, int& x, int& y, int& range );

    void DrawBot( wxDC& dc, BattleBot& bot, bool selected, bool moving );
    void DrawSinglePlayer( wxDC& dc );
    void DrawBackground( wxDC& dc );
    void DrawStartRects( wxDC& dc );
    void DrawStartPositions( wxDC& dc );
    void DrawStartRect( wxDC& dc, int index, const wxRect& sr, const wxColour& col, bool mouseover, int alphalevel = 70 );

    void SetMouseOverRect( int index );

    void _SetCursor();

    wxBitmap* m_image;

    IBattle* m_battle;
    SinglePlayerBattle* m_sp_battle;

    Ui& m_ui;
    wxString m_mapname;

    bool m_draw_start_types;
    bool m_fixed_size;
    bool m_ro;
    bool m_sp;

    int m_mover_rect;
    int m_mdown_rect;

    RectArea m_rect_area;
    RectArea m_last_rect_area;

    RectArea m_mdown_area;
    BattleStartRect m_tmp_brect;

    MouseAction m_maction;
    int m_mdown_x;
    int m_mdown_y;

    wxSize m_lastsize;

    wxBitmap* m_close_img;
    wxBitmap* m_close_hi_img;

    wxBitmap* m_start_ally;
    wxBitmap* m_start_enemy;
    wxBitmap* m_start_unused;

    wxBitmap* m_player_img;
    wxBitmap* m_bot_img;

    wxBitmap* m_nfound_img;
    wxBitmap* m_reload_img;
    wxBitmap* m_dl_img;

    UnitSyncMap m_map;

    int m_bot_expanded;

  DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
