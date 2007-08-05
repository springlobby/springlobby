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
#include "utils.h"

//#include "images/select_icon.xpm"


BEGIN_EVENT_TABLE( MapCtrl, wxPanel )
  EVT_PAINT( MapCtrl::OnPaint )
  EVT_SIZE( MapCtrl::OnResize )
  EVT_MOTION( MapCtrl::OnMouseMove )
END_EVENT_TABLE()


MapCtrl::MapCtrl( wxWindow* parent, int size, Battle& battle, bool readonly ):
  wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size), wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE ),
    m_image(0), m_battle(battle), m_mapname(_T("")), m_lastsize(-1,-1),m_ro(readonly),m_mover_rect(-1)
{
  SetBackgroundStyle( wxBG_STYLE_CUSTOM );
  SetBackgroundColour( *wxLIGHT_GREY );
}


MapCtrl::~MapCtrl()
{
  FreeMinimap();
}


wxRect MapCtrl::_GetMinimapRect()
{
  if ( m_image == 0 ) return wxRect();

  int cwidth, cheight, top = 0, left = 0;
  GetClientSize( &cwidth, &cheight );

  if ( m_image->GetWidth() > m_image->GetHeight() ) {
    top = (cheight - m_image->GetHeight()) / 2;
  } else {
    left = (cwidth - m_image->GetWidth()) / 2;
  }
  return wxRect( left, top, m_image->GetWidth(), m_image->GetHeight() );
}


wxRect MapCtrl::_GetStartRect( int index )
{
  int x1,y1,x2,y2;
  
  wxRect mr = _GetMinimapRect();

  BattleStartRect* sr = m_battle.GetStartRect( index );
  if ( sr == 0 ) return wxRect();

  x1 = mr.x + sr->left * mr.width / 200;
  y1 = mr.y + sr->top * mr.height / 200;
  x2 = mr.x + sr->right * mr.width / 200;
  y2 = mr.y + sr->bottom * mr.height / 200;

  return wxRect( x1, y1, x2-x1, y2-y1 );
}


void MapCtrl::_DrawStartRect( wxDC& dc, int index, const wxRect& sr, const wxColour& col, bool mouseover )
{

  dc.SetBrush( wxBrush( *wxLIGHT_GREY, wxTRANSPARENT ) );

  wxImage img( sr.width, sr.height );
  wxColour light;
  light.Set( col.Red()+100>200?200:col.Red()+100, col.Green()+100>200?200:col.Green()+100, col.Blue()+100>200?200:col.Blue()+100  );
  img.SetRGB( wxRect( 0, 0, sr.width, sr.height ), light.Red(), light.Green(), light.Blue() );
  unsigned char *alpha = (unsigned char*)malloc( (sr.width)*(sr.height) );
  for ( int y = 0; y < sr.height; y++ ) {
    int a;
    if ( (y%3) == 0 ) a = 70;
    else a = 40;
    for ( int x = 0; x < sr.width; x++ ) {
      alpha[y*(sr.width)+x] = a;
    }
  }
  img.SetAlpha( alpha );
  wxBitmap bmpimg( img );
  dc.DrawBitmap( bmpimg, sr.x, sr.y, false );

  int twidth, theight;
/*  wxFont f( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL|wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_LIGHT );
  dc.SetFont( f );*/
  dc.GetTextExtent( wxString::Format( _T("%d"), index+1), &twidth, &theight );
  dc.SetTextForeground( col );
  dc.DrawText( wxString::Format( _T("%d"), index+1), sr.x + sr.width / 2 - twidth / 2, sr.y + sr.height / 2 - theight / 2 - 1 );

  dc.SetPen( wxPen( col ) );

  if ( mouseover && !m_ro ) {
    const int boxsize = 8;
    dc.DrawRectangle( sr.x+1, sr.y+1, sr.width-2, sr.height-2 );
    dc.SetPen( wxPen( *wxBLACK ) );
    dc.DrawRectangle( sr.x, sr.y, sr.width, sr.height );

    dc.SetBrush( wxBrush( *wxBLACK, wxSOLID ) );
    dc.DrawRectangle( sr.x, sr.y, boxsize, boxsize );
    dc.DrawRectangle( sr.x + sr.width - boxsize, sr.y + sr.height - boxsize, boxsize, boxsize );
    dc.DrawRectangle( sr.x + sr.width - boxsize, sr.y + 1, boxsize, boxsize );
    dc.DrawRectangle( sr.x, sr.y + sr.height - boxsize, boxsize, boxsize );
  } else {
    dc.DrawRectangle( sr.x, sr.y, sr.width, sr.height );
  }
}


void MapCtrl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc( this );

  int width, height, top = 0, left = 0;
  GetClientSize( &width, &height );
  
  wxRect r = _GetMinimapRect();

  // Draw background.
  dc.SetPen( wxPen( *wxLIGHT_GREY ) );
  dc.SetBrush( wxBrush( *wxLIGHT_GREY, wxSOLID ) );
  if ( r.y > 0 ) {
    dc.DrawRectangle( 0, 0, width, (height - r.height) / 2 );
    dc.DrawRectangle( 0, r.y+r.height, width, (height - r.height) / 2 + 1 );
  }
  if ( r.x > 0 ) {
    dc.DrawRectangle( 0, 0, (width - r.width) / 2, height );
    dc.DrawRectangle( r.x+r.width, 0, (width - r.width) / 2 + 1, height );
  }

  // Draw minimap.
  if ( !m_image ) {
    dc.DrawText( _("Minimap n/a"), 10, 10 );
  } else {
    wxRect mr = _GetMinimapRect();
    dc.DrawBitmap( *m_image, mr.x, mr.y, false );
    width = mr.width;
    height = mr.height;
  }

  // Draw startrects.
  for ( int i = 0; i < 15; i++ ) {
    wxRect sr = _GetStartRect( i );
    if ( sr.IsEmpty() ) continue;
    wxColour col;
    if ( i == m_battle.GetMe().BattleStatus().ally ) {
      col.Set( 0, 200, 0 );
    } else {
      col.Set( 200, 0, 0 );
    }
    _DrawStartRect( dc, i, sr, col, m_mover_rect == i );
  }

}


void MapCtrl::OnResize( wxSizeEvent& event )
{
  UpdateMinimap();
}


void MapCtrl::OnMouseMove( wxMouseEvent& event )
{
  wxPoint p = event.GetPosition();
  if ( p == wxDefaultPosition ) {
    return;
  }

  // Make sure point is inside minimap
  if ( _GetMinimapRect().Inside( p ) ) {

    // Check if point is in a startrect.
    for ( int i = 0; i < 16; i++ ) {

      wxRect r = _GetStartRect( i );
      if ( r.IsEmpty() ) continue;

      if ( r.Inside( p ) ) {

        if ( m_mover_rect != i ) {
          _SetMouseOverRect( i );
          UpdateMinimap();
        }
        return;

      }

    }
  }

  if ( m_mover_rect != -1 ) {
    _SetMouseOverRect( -1 );
    UpdateMinimap();
  }
}


void MapCtrl::_SetMouseOverRect( int index )
{
  m_mover_rect = index;
  if ( m_mover_rect != -1 ) SetCursor( wxCursor( wxCURSOR_SIZING ) );
  else SetCursor( wxCursor( wxCURSOR_ARROW ) );
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

