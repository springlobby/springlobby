//
// Class: MapCtrl
//


#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/bitmap.h>

#include "mapctrl.h"
#include "battle.h"


BEGIN_EVENT_TABLE( MapCtrl, wxPanel )
  EVT_PAINT( MapCtrl::OnPaint )
END_EVENT_TABLE()


MapCtrl::MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly ):
  wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size) ), m_image(NULL), m_battle(battle)
{
  LoadImage();
}


MapCtrl::~MapCtrl()
{
  FreeImage();
}


void MapCtrl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc( this );

  int width, height;
  GetClientSize( &width, &height );
 
  if ( m_image == NULL ) {
    dc.DrawText( _("Minimap n/a"), 0, 0 );
  } else {
    dc.DrawBitmap( *m_image, 0, 0, false );
  }
}


void MapCtrl::LoadImage()
{

}


void MapCtrl::FreeImage()
{

}
