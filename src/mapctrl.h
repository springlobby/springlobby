#ifndef SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPCTRL_H

#include <wx/string.h>

class wxPanel;
class wxBitmap;
class Battle;

class MapCtrl : public wxPanel
{
  public:
    MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly );
    ~MapCtrl();

    void LoadMinimap();
    void FreeMinimap();
    void UpdateMinimap();

    void OnPaint( wxPaintEvent& event );

  protected:

    wxBitmap* m_image;
    Battle& m_battle;
    wxString m_mapname;


  DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
