//
// Class: MapCtrl
//


#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>

#include "utils.h"
#include "mapctrl.h"
#include "battle.h"
#include "iunitsync.h"

//#include "images/select_icon.xpm"


BEGIN_EVENT_TABLE( MapCtrl, wxPanel )
  EVT_PAINT( MapCtrl::OnPaint )
END_EVENT_TABLE()


MapCtrl::MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly ):
  wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size) ), m_image(NULL), m_battle(battle), m_mapname(_T("")) 
{
  
}


MapCtrl::~MapCtrl()
{
  FreeMinimap();
}


void MapCtrl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc( this );

  int width, height, top = 1, left = 1;
  GetClientSize( &width, &height );
  
  dc.SetBrush( wxBrush( *wxLIGHT_GREY, wxSOLID ) );
  dc.DrawRectangle( 0, 0, width, height );
  if ( m_image == NULL ) {
    dc.DrawText( _("Minimap n/a"), 10, 10 );
    width--;
    height--;
  } else {
    if ( m_image->GetWidth() > m_image->GetHeight() ) {
      top = (height - m_image->GetHeight()) / 2;
      dc.DrawBitmap( *m_image, left, top, false );
    } else {
      left = (width - m_image->GetWidth()) / 2;
      dc.DrawBitmap( *m_image, left, top, false );
    }
    width = m_image->GetWidth();
    height = m_image->GetHeight();
  }

  for ( int i = 0; i < 15; i++ ) {
    int x1,y1,x2,y2;
    BattleStartRect* sr = m_battle.GetStartRect( i );
    if ( sr == 0 ) continue;

    x1 = left + (sr->left * width) / 200;
    y1 = top + (sr->top * height) / 200;
    x2 = left + (sr->right * width) / 200;
    y2 = top + (sr->bottom * height) / 200;

    wxColour c;
    c.Set( 140, 170, 140 );

    dc.SetBrush( wxBrush( c, wxSOLID ) );
    dc.DrawRectangle( x1, y1, x2-x1, y2-y1 );
    dc.DrawText( wxString::Format( _T("%d"), i+1), x1+1, y1+1 );
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
    m_mapname = WX_STRING( m_battle.opts().mapname );
    Refresh();
    Update();
  } catch (...) {
    m_image = NULL;
    m_mapname = _T("");
  }

}


void MapCtrl::FreeMinimap()
{
  if ( m_image != NULL ) {
    delete m_image;
    m_image = NULL;
    m_mapname = _T("");
  }
}


void MapCtrl::UpdateMinimap()
{
  if ( m_mapname != WX_STRING( m_battle.opts().mapname ) ) {
    FreeMinimap();
    LoadMinimap();
  }
  Refresh();
  Update();
}

