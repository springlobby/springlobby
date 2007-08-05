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
#include "user.h"

//#include "images/select_icon.xpm"


BEGIN_EVENT_TABLE( MapCtrl, wxPanel )
  EVT_PAINT( MapCtrl::OnPaint )
  EVT_SIZE( MapCtrl::OnResize )
END_EVENT_TABLE()


MapCtrl::MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly ):
  wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size), wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE ),
    m_image(0), m_battle(battle), m_mapname(_T("")), m_lastsize(-1,-1)
{

}


MapCtrl::~MapCtrl()
{
  FreeMinimap();
}


void MapCtrl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc( this );

  int width, height, top = 0, left = 0;
  GetClientSize( &width, &height );

  dc.SetPen( wxPen( *wxLIGHT_GREY ) );
  dc.SetBrush( wxBrush( *wxLIGHT_GREY, wxSOLID ) );
  dc.DrawRectangle( 0, 0, width, height );
  if ( !m_image ) {
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

    wxColour c( 140, 170, 140 );
    dc.SetBrush( wxBrush( c, wxTRANSPARENT ) );

    wxImage img( x2-x1, y2-y1 );
    if ( i == m_battle.GetMe().BattleStatus().ally ) {
      img.SetRGB( wxRect( 0, 0, x2-x1, y2-y1 ), 140, 200, 140 );
    } else {
      img.SetRGB( wxRect( 0, 0, x2-x1, y2-y1 ), 200, 140, 140 );
    }
    unsigned char *alpha = (unsigned char*)malloc( (x2-x1)*(y2-y1) );
    for ( int y = 0; y < y2-y1; y++ ) {
      int a;
      if ( (y%3) == 0 ) a = 150;
      else a = 110;
      for ( int x = 0; x < x2-x1; x++ ) {
        alpha[y*(x2-x1)+x] = a;
      }
    }
    img.SetAlpha( alpha );
    wxBitmap bmpimg( img );
    dc.DrawBitmap( bmpimg, x1, y1, false );
    if ( i == m_battle.GetMe().BattleStatus().ally ) {
      c.Set( 0, 200, 0 );
    } else {
      c.Set( 200, 0, 0 );
    }
    dc.SetPen( wxPen( c ) );
    dc.SetTextForeground( c );
    int twidth, theight;
    dc.GetTextExtent( wxString::Format( _T("%d"), i+1), &twidth, &theight );
    dc.DrawText( wxString::Format( _T("%d"), i+1), x1 + (x2-x1) / 2 - twidth / 2, y1 + (y2-y1) / 2 - theight / 2 - 1 );
    dc.DrawRectangle( x1, y1, x2-x1, y2-y1 );
  }

}


void MapCtrl::OnResize( wxSizeEvent& event )
{
  UpdateMinimap();
}


void MapCtrl::LoadMinimap()
{
  if ( m_image )
    return;
  try {
    int w, h;
    GetClientSize( &w, &h );
    if ( w * h == 0 ) {
      m_image = 0;
      m_mapname = _T("");
      m_lastsize = wxSize( -1, -1 );
      return;
    }
    wxImage img = usync()->GetMinimap( m_battle.opts().mapname, w );
    m_image = new wxBitmap( img );
    m_mapname = WX_STRING( m_battle.opts().mapname );
    m_lastsize = wxSize( w, h );
    Refresh();
    Update();
  } catch (...) {
    m_image = 0;
    m_mapname = _T("");
    
  }

}


void MapCtrl::FreeMinimap()
{
  if ( m_image ) {
    delete m_image;
    m_image = 0;
    m_mapname = _T("");
  }
}


void MapCtrl::UpdateMinimap()
{
  int w, h;
  GetClientSize( &w, &h );
  if ( (m_mapname != WX_STRING( m_battle.opts().mapname) || ( m_lastsize != wxSize(w, h) ) ) ) {
    FreeMinimap();
    LoadMinimap();
  }
  Refresh();
  Update();
}

