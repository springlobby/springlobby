#ifndef SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPCTRL_H

#include <wx/string.h>

class wxPanel;
class wxBitmap;
class wxDC;

class Battle;

#define RA_Main -1
#define RA_UpLeft 0
#define RA_UpRight 1
#define RA_DownRight 2
#define RA_DownLeft 3

typedef int RectArea;


class MapCtrl : public wxPanel
{
  public:
    MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly );
    ~MapCtrl();

    void LoadMinimap();
    void FreeMinimap();
    void UpdateMinimap();

    void OnPaint( wxPaintEvent& event );
    void OnResize( wxSizeEvent& event );

    void OnMouseMove( wxMouseEvent& event );

  protected:

    wxRect _GetMinimapRect();
    wxRect _GetStartRect( int index );
    void _DrawStartRect( wxDC& dc, int index, const wxRect& sr, const wxColour& col, bool mouseover );

    void _SetMouseOverRect( int index );

    void _SetCursor();

    wxBitmap* m_image;
    Battle& m_battle;
    wxString m_mapname;
    bool m_ro;
    int m_mover_rect;
    RectArea m_rect_area;
    RectArea m_last_rect_area;

    wxSize m_lastsize;

    wxBitmap* m_close_img;
    wxBitmap* m_close_hi_img;

  DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
