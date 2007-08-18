#ifndef SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPCTRL_H

#include <wx/string.h>
#include "battle.h"

class wxPanel;
class wxBitmap;
class wxDC;

class Battle;
struct BattleStartRect;
class Ui;

#define RA_Main -1
#define RA_UpLeft 0
#define RA_UpRight 1
#define RA_DownRight 2
#define RA_DownLeft 3

typedef int RectArea;

#define MA_None 0
#define MA_Add 1
#define MA_Delete 2
#define MA_Move 3
#define MA_ResizeUpLeft 4
#define MA_ResizeUpRight 5
#define MA_ResizeDownLeft 6
#define MA_ResizeDownRight 7

typedef int MouseAction;


class MapCtrl : public wxPanel
{
  public:
    MapCtrl( wxWindow* parent, int size, Battle* battle, Ui& ui, bool readonly );
    ~MapCtrl();

    void SetBattle( Battle* battle );

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

    BattleStartRect _GetBattleRect( int x1, int y1, int x2, int y2, int ally = -1 );
    wxRect _GetMinimapRect();
    wxRect _GetStartRect( int index );
    wxRect _GetStartRect( const BattleStartRect& sr );

    int _GetNewRectIndex();

    void _DrawStartRect( wxDC& dc, int index, const wxRect& sr, const wxColour& col, bool mouseover );

    void _SetMouseOverRect( int index );

    void _SetCursor();

    wxBitmap* m_image;
    Battle* m_battle;
    Ui& m_ui;
    wxString m_mapname;

    bool m_ro;
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

  DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
