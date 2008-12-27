/* Copyright (C) 2008 The SpringLobby Team. All rights reserved. */
/* Author: Tobi Vollebregt */

#include "mapgridctrl.h"

#include "utils.h"
#include <wx/dcclient.h>
#include <wx/image.h>
#include <algorithm>

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


MapGridCtrl::MapGridCtrl( wxWindow* parent, Ui& ui, wxSize size )
	: wxPanel( parent, -1, wxDefaultPosition, size, wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE )
	, m_ui( ui )
	, m_size( 0, 0 )
	, m_pos( 0, 0 )
	, m_in_mouse_drag( false )
	, m_async_minimap_fetches( 0 )
{
}


MapGridCtrl::~MapGridCtrl()
{

}


inline bool MapGridCtrl::CompareName( const MapData* a, const MapData* b )
{
	return a->name.CmpNoCase( b->name ) < 0;
}

inline bool MapGridCtrl::CompareArea( const MapData* a, const MapData* b )
{
	return (a->info.width * a->info.height) < (b->info.width * b->info.height);
}

inline bool MapGridCtrl::ComparePosCount( const MapData* a, const MapData* b )
{
	return a->info.posCount < b->info.posCount;
}

template< class Compare > inline void MapGridCtrl::_Sort( int dimension, Compare cmp )
{
	if ( dimension <= 1 ) {
		// vertical sort (ie. sort entire dataset)
		std::stable_sort( m_grid.begin(), m_grid.end(), cmp );
	}
	else /*if ( dimension == 2 )*/ {
		// horizontal sort (ie. sort each row individually)
		for ( int y = 0; y < m_size.y; ++y ) {
			const int idx1 = y * m_size.x;
			const int idx2 = std::min( int(m_grid.size()), idx1 + m_size.x );
			std::stable_sort( m_grid.begin() + idx1, m_grid.begin() + idx2, cmp );
		}
	}
}


void MapGridCtrl::Sort( SortKey vertical, SortKey horizontal )
{
	if ( m_maps.empty() ) return;

	// Always start by sorting on name, to get duplicate maps together.
	SortKey keys[3] = { SortKey_Name, vertical, horizontal };

	// This looks like common antipattern 'loop switch sequence', however here
	// it's the best way I found to prevent duplication of the switch statement,
	// which will probably require most (all?) changes and possibly grow big.

	for (int i = 0; i < 3; ++i) {
		// Do nothing if current sortkey is same as previous one.
		if ( i > 0 && keys[i] == keys[i - 1] ) continue;
		// Sort dimension i on sortkey keys[i].
		switch ( keys[i] ) {
			case SortKey_Name:     _Sort( i, CompareName ); break;
			case SortKey_Area:     _Sort( i, CompareArea ); break;
			case SortKey_PosCount: _Sort( i, ComparePosCount ); break;
			default:
				ASSERT_EXCEPTION( false, _T("unimplemented SortKey in MapGridCtrl::Sort") );
				break;
		}
	}

	Refresh();
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

	for (int y = 0; y < m_size.y && it != m_maps.end(); ++y) {
		for (int x = 0; x < m_size.x && it != m_maps.end(); ++x) {
			m_grid.push_back( &it->second );
			++it;
		}
	}

	Sort( SortKey_Name, SortKey_Name );
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
	if ( m_maps.empty() ) return;

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
			if ( idx >= int(m_grid.size()) ) break;
			DrawMap( dc, *m_grid[idx], scrn_x, scrn_y );
		}
	}
}


void MapGridCtrl::OnResize( wxSizeEvent& event )
{
	CheckInBounds();
}


void MapGridCtrl::OnMouseMove( wxMouseEvent& event )
{
	if ( m_in_mouse_drag ) {
		m_pos -= (event.GetPosition() - m_last_mouse_pos);
		m_last_mouse_pos = event.GetPosition();

		CheckInBounds();
		Refresh();
	}
	else {
		// the (5, 5) is some random offset to correct error in assumption
		// that event.GetPosition() is relative to client area of control.
		const wxPoint pos = wxPoint2DInt (event.GetPosition() + m_pos - wxPoint(5, 5)) / MINIMAP_SIZE;
		const int idx = pos.y * m_size.x + pos.x;

		if ( pos.x >= 0 && pos.x < m_size.x && pos.y >= 0 && pos.y <= m_size.y && idx < (int)m_grid.size() ) {
			m_mouseover_map = m_grid[idx];
			SetToolTip( m_mouseover_map->name + _T("\n") + TowxString(m_mouseover_map->info.width / 512) + _T("x") + TowxString(m_mouseover_map->info.height / 512) );
		}
		else {
			m_mouseover_map = NULL;
			SetToolTip( _T("") );
		}
	}
}


void MapGridCtrl::OnLeftDown( wxMouseEvent& event )
{
	SetCursor( wxCursor( wxCURSOR_HAND ) );
	m_last_mouse_pos = event.GetPosition();
	m_in_mouse_drag = true;
}


void MapGridCtrl::OnLeftUp( wxMouseEvent& event )
{
	SetCursor( wxCursor( wxCURSOR_ARROW ) );
	m_in_mouse_drag = false;
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
