//
// Class: MapCtrl
//


#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>

#include "mapctrl.h"
#include "battle.h"
#include "springunitsync.h"

#include "images/select_icon.xpm"


BEGIN_EVENT_TABLE( MapCtrl, wxPanel )
  EVT_PAINT( MapCtrl::OnPaint )
END_EVENT_TABLE()


MapCtrl::MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly ):
  wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size) ), m_image(NULL), m_battle(battle)
{
  LoadMinimap();
}


MapCtrl::~MapCtrl()
{
  FreeMinimap();
}


void MapCtrl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc( this );

  int width, height;
  GetClientSize( &width, &height );

  if ( m_image == NULL ) {
    dc.DrawText( _("Minimap n/a"), 0, 0 );
  } else {
    dc.DrawBitmap( *m_image, 1, 1, false );
    dc.SetBrush( wxBrush( *wxRED, wxTRANSPARENT ) );
    dc.DrawRectangle( 0, 0, width, height );
  }
}


void MapCtrl::LoadMinimap()
{
  if ( m_image != NULL ) return;
  try {
    int w, h;
    GetClientSize( &w, &h );
    wxImage img = usync()->GetMinimap( m_battle.opts().mapname, w-2 );
    m_image = new wxBitmap( img );
  } catch (...) {}
}


void MapCtrl::FreeMinimap()
{
  if ( m_image != NULL ) {
    delete m_image;
    m_image = NULL;
  }
}
