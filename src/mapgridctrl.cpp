/* Copyright (C) 2008 The SpringLobby Team. All rights reserved. */
/* Author: Tobi Vollebregt */

#include "mapgridctrl.h"

#include "utils.h"
#include <wx/dcclient.h>
#include <wx/image.h>


/// Size of the map previews.  This should be same as size of map previews in
/// battle list and as prefetch size in SpringUnitSync for performance reasons.
#define MINIMAP_SIZE 98


BEGIN_EVENT_TABLE( MapGridCtrl, wxPanel )
	EVT_PAINT( MapGridCtrl::OnPaint )
	EVT_SIZE( MapGridCtrl::OnResize )
	EVT_MOTION( MapGridCtrl::OnMouseMove )
	EVT_LEFT_DOWN( MapGridCtrl::OnLeftDown )
	EVT_LEFT_UP( MapGridCtrl::OnLeftUp )
  EVT_COMMAND( wxID_ANY, UnitSyncGetMapImageAsyncCompletedEvt, MapGridCtrl::OnGetMapImageAsyncCompleted )
END_EVENT_TABLE()


MapGridCtrl::MapGridCtrl( wxWindow* parent, int size, IBattle* battle, Ui& ui )
	: wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size), wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE )
	, m_ui( ui )
	, m_battle( battle )
	, m_size( 0, 0 )
	, m_pos( 0, 0 )
	, m_async_minimap_fetches( 0 )
{
}


MapGridCtrl::~MapGridCtrl()
{

}


void MapGridCtrl::SetBattle( IBattle* battle )
{
	m_battle = battle;
}


void MapGridCtrl::LoadMaps()
{
	wxArrayString maps = usync().GetMapList();
	int count = maps.size();

	wxLogDebugFunc( TowxString( maps.size() ) );

	m_maps.clear();
	m_grid.clear();

	for (int i = 0; i < count; ++i ) {
		try {
			m_maps[maps[i]] = usync().GetMapEx( maps[i] );
		}
		catch (...) {}
	}

	const int width = int(sqrt( m_maps.size() ));

	m_size.x = width;
	m_size.y = (m_maps.size() + width - 1) / width;

	std::map<wxString, MapData>::iterator it = m_maps.begin();
	if (it == m_maps.end()) return;

	for (int y = 0; y < m_size.y; ++y) {
		for (int x = 0; x < m_size.x; ++x) {
			m_grid.push_back( &it->second );
			++it;
			if (it == m_maps.end()) return;
		}
	}
}


void MapGridCtrl::CheckInBounds()
{
	const int size = MINIMAP_SIZE + 1;

	int width, height;
	GetClientSize( &width, &height );

	// this ensures always at least a quarter of the client area contains minimaps
	m_pos.x = std::max( -width  / 2, std::min( size * m_size.x - width  / 2, m_pos.x ) );
	m_pos.y = std::max( -height / 2, std::min( size * m_size.y - height / 2, m_pos.y ) );
}


void MapGridCtrl::DrawMap( wxDC& dc, MapData& map, int x, int y )
{
	switch ( map.state ) {
		case MapState_NoMinimap:
			if (m_async_minimap_fetches < 2) {
				usync().GetMinimapAsync( map.name, MINIMAP_SIZE, MINIMAP_SIZE, this );
				map.state = MapState_GetMinimap;
				++m_async_minimap_fetches;
			}
			break;
		case MapState_GetMinimap:
			// do nothing, waiting for async fetch of minimap
			break;
		case MapState_GotMinimap:
			x += (MINIMAP_SIZE - map.minimap.GetWidth()) / 2;
			y += (MINIMAP_SIZE - map.minimap.GetHeight()) / 2;
			dc.DrawBitmap( map.minimap, x, y, false );
			break;
	}
}


void MapGridCtrl::OnPaint( wxPaintEvent& event )
{
	if ( m_maps.size() == 0 ) LoadMaps();

	wxPaintDC dc( this );

	int width, height;
	GetClientSize( &width, &height );

	const int size = MINIMAP_SIZE + 1;

	// simple tile mapping algorithm
	int start_scrn_x = -(m_pos.x % size);
	int start_scrn_y = -(m_pos.y % size);
	int start_grid_x = m_pos.x / size;
	int start_grid_y = m_pos.y / size;
	int end_grid_x   = std::min(start_grid_x + (width  + 2 * size - 1) / size, m_size.x);
	int end_grid_y   = std::min(start_grid_y + (height + 2 * size - 1) / size, m_size.y);

	if ( start_grid_x < 0 ) {
		start_scrn_x += size * -start_grid_x;
		start_grid_x = 0;
	}

	if ( start_grid_y < 0 ) {
		start_scrn_y += size * -start_grid_y;
		start_grid_y = 0;
	}

	for (int y = start_grid_y, scrn_y = start_scrn_y; y < end_grid_y; ++y, scrn_y += size) {
		for (int x = start_grid_x, scrn_x = start_scrn_x; x < end_grid_x; ++x, scrn_x += size) {
			const int idx = y * m_size.x + x;
			if ( idx >= 0 && idx < int(m_grid.size()) ) {
				DrawMap( dc, *m_grid[idx], scrn_x, scrn_y );
			}
		}
	}
}


void MapGridCtrl::OnResize( wxSizeEvent& event )
{
	CheckInBounds();
}


void MapGridCtrl::OnMouseMove( wxMouseEvent& event )
{
	if ( !event.Dragging() ) return;

	m_pos -= (event.GetPosition() - m_last_mouse_pos);
	m_last_mouse_pos = event.GetPosition();

	CheckInBounds();
	Refresh();
}


void MapGridCtrl::OnLeftDown( wxMouseEvent& event )
{
	SetCursor( wxCursor( wxCURSOR_HAND ) );
	m_last_mouse_pos = event.GetPosition();
}


void MapGridCtrl::OnLeftUp( wxMouseEvent& event )
{
	SetCursor( wxCursor( wxCURSOR_ARROW ) );
}


void MapGridCtrl::OnGetMapImageAsyncCompleted( wxCommandEvent& event )
{
	wxString mapname = event.GetString();

	wxLogDebugFunc( mapname );

	m_maps[mapname].minimap = usync().GetMinimap( mapname, MINIMAP_SIZE, MINIMAP_SIZE );
	m_maps[mapname].state = MapState_GotMinimap;

	--m_async_minimap_fetches;

	Refresh(); // TODO: use RefreshRect ?
}
