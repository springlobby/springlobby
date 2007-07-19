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

    void OnPaint( wxPaintEvent& event );

  protected:

    wxBitmap* m_image;
    Battle& m_battle;

    void LoadImage();
    void FreeImage();

  DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
