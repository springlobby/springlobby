/* Copyright (C) 2008 The SpringLobby Team. All rights reserved. */
/* Author: Tobi Vollebregt */

#include "mapgridctrl.h"

#include "uiutils.h"
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


const wxEventType MapGridCtrl::MapSelectedEvt = wxNewEventType();


MapGridCtrl::MapGridCtrl( wxWindow* parent, Ui& ui, wxSize size, wxWindowID id )
	: wxPanel( parent, id, wxDefaultPosition, size, wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE )
	, m_ui( ui )
	, m_async( this )
	, m_size( 0, 0 )
	, m_pos( 0, 0 )
	, m_in_mouse_drag( false )
	, m_async_minimap_fetches( 0 )
	, m_mouseover_map( NULL )
	, m_selected_map( NULL )
{
	UpdateToolTip();
}


MapGridCtrl::~MapGridCtrl()
{

}


#define CMP( var ) \
	if ( a->info.var < b->info.var ) return -1; \
	if ( a->info.var > b->info.var ) return 1; \
	return 0

#define CMP2( a, b ) \
	if ( (a) < (b) ) return -1; \
	if ( (a) > (b) ) return 1; \
	return 0

inline int MapGridCtrl::CompareName( const MapData* a, const MapData* b )
{
	return a->name.CmpNoCase( b->name );
}
inline int MapGridCtrl::CompareTidalStrength( const MapData* a, const MapData* b )
{
	CMP( tidalStrength );
}
inline int MapGridCtrl::CompareGravity( const MapData* a, const MapData* b )
{
	CMP( gravity );
}
inline int MapGridCtrl::CompareMaxMetal( const MapData* a, const MapData* b )
{
	CMP( maxMetal );
}
inline int MapGridCtrl::CompareExtractorRadius( const MapData* a, const MapData* b )
{
	CMP( extractorRadius );
}
inline int MapGridCtrl::CompareMinWind( const MapData* a, const MapData* b )
{
	CMP( minWind );
}
inline int MapGridCtrl::CompareMaxWind( const MapData* a, const MapData* b )
{
	CMP( maxWind );
}
inline int MapGridCtrl::CompareWind( const MapData* a, const MapData* b )
{
	CMP2( (a->info.minWind + a->info.maxWind), (b->info.minWind + b->info.maxWind) );
}
inline int MapGridCtrl::CompareArea( const MapData* a, const MapData* b )
{
	CMP2( (a->info.width * a->info.height), (b->info.width * b->info.height) );
}
static inline double AspectRatio( const MapInfo& x )
{
	const int max = std::max( x.width, x.height );
	const int min = std::min( x.width, x.height );
	return (double) max / (min != 0 ? min : 1);
}
inline int MapGridCtrl::CompareAspectRatio( const MapData* a, const MapData* b )
{
	CMP2( AspectRatio( a->info ), AspectRatio( b->info ) );
}
inline int MapGridCtrl::ComparePosCount( const MapData* a, const MapData* b )
{
	CMP( posCount );
}

#undef CMP2
#undef CMP

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


void MapGridCtrl::Sort( SortKey vertical, SortKey horizontal, bool vertical_direction, bool horizontal_direction )
{
	if ( m_maps.empty() ) return;

	// Always start by sorting on name, to get duplicate maps together.
	SortKey keys[3] = { SortKey_Name, vertical, horizontal };
	bool dirs[3] = { false, vertical_direction, horizontal_direction };

	// This looks like common antipattern 'loop switch sequence', however here
	// it's the best way I found to prevent duplication of the switch statement,
	// which will probably require most (all?) changes and possibly grow big.

	for (int i = 0; i < 3; ++i) {
		// Do nothing if current sortkey is same as previous one.
		if ( i > 0 && keys[i] == keys[i - 1] && dirs[i] == dirs[i - 1] ) continue;
		// Sort dimension i on sortkey keys[i].
		const bool d = dirs[i];
		switch ( keys[i] ) {
			case SortKey_Name:            _Sort( i, _Compare( d, CompareName ) ); break;
			case SortKey_TidalStrength:   _Sort( i, _Compare( d, CompareTidalStrength ) ); break;
			case SortKey_Gravity:         _Sort( i, _Compare( d, CompareGravity ) ); break;
			case SortKey_MaxMetal:        _Sort( i, _Compare( d, CompareMaxMetal ) ); break;
			case SortKey_ExtractorRadius: _Sort( i, _Compare( d, CompareExtractorRadius ) ); break;
			case SortKey_MinWind:         _Sort( i, _Compare( d, CompareMinWind ) ); break;
			case SortKey_MaxWind:         _Sort( i, _Compare( d, CompareMaxWind ) ); break;
			case SortKey_Wind:            _Sort( i, _Compare( d, CompareWind ) ); break;
			case SortKey_Area:            _Sort( i, _Compare( d, CompareArea ) ); break;
			case SortKey_AspectRatio:     _Sort( i, _Compare( d, CompareAspectRatio ) ); break;
			case SortKey_PosCount:        _Sort( i, _Compare( d, ComparePosCount ) ); break;
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


void MapGridCtrl::UpdateToolTip()
{
	if ( m_mouseover_map != NULL ) {
		const MapInfo& info = m_mouseover_map->info;
		const wxString endl = _T("\n");
		wxString tooltip = RefineMapname( m_mouseover_map->name );

		tooltip += _T(" (") + TowxString(info.width / 512) + _T("x") + TowxString(info.height / 512) + _T(")");
		if ( !info.author.empty() ) tooltip += _(" by ") + info.author;
		tooltip += endl + endl;

		if ( !info.description.empty() ) tooltip += info.description + endl;

		// TODO: maybe use something else then tabs?  May be pain to get right when translating..
		tooltip += _("Tidal strength\t: ") + TowxString(info.tidalStrength) + endl;
		tooltip += _("Gravity\t\t: ") + TowxString(info.gravity) + endl;
		tooltip += _("Metal scale\t: ") + TowxString(info.maxMetal) + endl;
		tooltip += _("Extractor radius\t: ") + TowxString(info.extractorRadius) + endl;
		tooltip += _("Wind\t\t\t: ") + TowxString(info.minWind) + _T(" - ") + TowxString(info.maxWind) + endl;
		tooltip += _("Start positions\t: ") + TowxString(info.posCount);

		SetToolTip( tooltip );
	}
	else {
		SetToolTip( _("Drag to move around the grid of map previews.\n"
		              "Click to select a map and close this window.") );
	}
}


void MapGridCtrl::UpdateAsyncFetches()
{
	if ( m_async_minimap_fetches != 0 ) return;

	for (int y = 0; y < m_size.y; ++y) {
		for (int x = 0; x < m_size.x; ++x) {
			const int idx = y * m_size.x + x;
			if ( idx >= int(m_grid.size()) ) break;
			if ( m_grid[idx]->state == MapState_NoMinimap ) {
				FetchMinimap( *m_grid[idx] );
				return;
			}
		}
	}
}


void MapGridCtrl::FetchMinimap( MapData& map )
{
	if ( m_async_minimap_fetches < 2 ) {
		m_async.GetMinimap( map.name, MINIMAP_SIZE, MINIMAP_SIZE );
		map.state = MapState_GetMinimap;
		++m_async_minimap_fetches;
	}
}


void MapGridCtrl::DrawMap( wxDC& dc, MapData& map, int x, int y )
{
	switch ( map.state ) {
		case MapState_NoMinimap:
			FetchMinimap( map );
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
		const wxPoint pos_unscaled = event.GetPosition() + m_pos - wxPoint(5, 5);
		const wxPoint pos = wxPoint2DInt(pos_unscaled) / MINIMAP_SIZE;
		const int idx = pos.y * m_size.x + pos.x;
		MapData* old_mouseover_map = m_mouseover_map;

		// use pos_unscaled for tests against 0 because negative values lower
		// than MINIMAP_SIZE get rounded up to 0 when diviving by MINIMAP_SIZE..
		if ( pos_unscaled.x >= 0 && pos.x < m_size.x && pos_unscaled.y >= 0 && pos.y <= m_size.y && idx < (int)m_grid.size() ) {
			m_mouseover_map = m_grid[idx];
		}
		else {
			m_mouseover_map = NULL;
		}

		if ( old_mouseover_map != m_mouseover_map ) {
			UpdateToolTip();
		}
	}
}


void MapGridCtrl::OnLeftDown( wxMouseEvent& event )
{
	SetCursor( wxCursor( wxCURSOR_HAND ) );
	m_first_mouse_pos = event.GetPosition();
	m_last_mouse_pos = event.GetPosition();
	m_in_mouse_drag = true;
}


void MapGridCtrl::OnLeftUp( wxMouseEvent& event )
{
	SetCursor( wxCursor( wxCURSOR_ARROW ) );
	m_in_mouse_drag = false;

	if ( wxPoint2DInt(event.GetPosition() - m_first_mouse_pos).GetVectorLength() <= 3 ) {
		m_selected_map = m_mouseover_map;

		if ( m_selected_map != NULL ) {
			wxLogMessage( _T("MapGridCtrl: Selected map: ") + m_selected_map->name );

			wxCommandEvent evt( MapSelectedEvt, GetId() );
			evt.SetEventObject( this );
			evt.SetString( m_selected_map->name );
			wxPostEvent( this, evt );
		}
	}
}


void MapGridCtrl::OnGetMapImageAsyncCompleted( wxCommandEvent& event )
{
	wxString mapname = event.GetString();

	wxLogDebugFunc( mapname );

	m_maps[mapname].minimap = usync().GetMinimap( mapname, MINIMAP_SIZE, MINIMAP_SIZE );
	m_maps[mapname].state = MapState_GotMinimap;

	--m_async_minimap_fetches;
	UpdateAsyncFetches();

	Refresh(); // TODO: use RefreshRect ?
}
