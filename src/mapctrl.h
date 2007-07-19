//
// Class: MapCtrl
//

#ifndef _MAPCTRL_H_
#define _MAPCTRL_H_

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

#endif // _MAPCTRL_H_
