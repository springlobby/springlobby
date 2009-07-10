/* Copyright (C) 2008 The SpringLobby Team. All rights reserved. */
/* Author: Tobi Vollebregt */

#include "mapgridctrl.h"

#include "settings.h"
#include "uiutils.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include <wx/dcbuffer.h>
#include <wx/geometry.h>
#include <wx/settings.h>
#include <algorithm>
#include <wx/log.h>

#include "images/map_select_1.png.h"
#include "images/map_select_2.png.h"

/// Size of the map previews.  This should be same as size of map previews in
/// battle list and as prefetch size in SpringUnitSync for performance reasons.
#define MINIMAP_SIZE 98

/// Margin between the map previews, in pixels.
#define MINIMAP_MARGIN 1

/// Maximum minimap fetches in WorkerThread queue
/// (these will still be executed after control is destroyed)
#define MAX_MINIMAP_FETCHES 2
/// Maximum mapinfo fetches in WorkerThread queue
#define MAX_MAPINFO_FETCHES 5


BEGIN_EVENT_TABLE( MapGridCtrl, wxPanel )
	EVT_PAINT( MapGridCtrl::OnPaint )
	EVT_SIZE( MapGridCtrl::OnResize )
	EVT_MOTION( MapGridCtrl::OnMouseMove )
	EVT_LEFT_DOWN( MapGridCtrl::OnLeftDown )
	EVT_LEFT_UP( MapGridCtrl::OnLeftUp )
	EVT_COMMAND( 2, UnitSyncAsyncOperationCompletedEvt, MapGridCtrl::OnGetMapImageAsyncCompleted )
	EVT_COMMAND( 3, UnitSyncAsyncOperationCompletedEvt, MapGridCtrl::OnGetMapExAsyncCompleted )
END_EVENT_TABLE()


const wxEventType MapGridCtrl::MapSelectedEvt = wxNewEventType();
const wxEventType MapGridCtrl::LoadingCompletedEvt = wxNewEventType();


MapGridCtrl::MapGridCtrl( wxWindow* parent, wxSize size, wxWindowID id )
	: wxPanel( parent, id, wxDefaultPosition, size, wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE )
	, m_async( this )
	, m_selection_follows_mouse( sett().GetMapSelectorFollowsMouse() )
	, m_size( 0, 0 )
	, m_pos( 0, 0 )
	, m_in_mouse_drag( false )
	, m_async_mapinfo_fetches( 0 )
	, m_async_minimap_fetches( 0 )
	, m_mouseover_map( NULL )
	, m_selected_map( NULL )
{
	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
	SetBackgroundColour( *wxLIGHT_GREY );

	m_img_background.Create( MINIMAP_SIZE, MINIMAP_SIZE, false /*don't clear*/ );
	wxRect rect( 0, 0, MINIMAP_SIZE, MINIMAP_SIZE );
	wxColor color( GetBackgroundColour() );
	m_img_background.SetRGB( rect, color.Red(), color.Green(), color.Blue() );

	m_img_minimap_alpha = charArr2wxImage( map_select_1_png, sizeof(map_select_1_png) );
	m_img_foreground    = charArr2wxImage( map_select_2_png, sizeof(map_select_2_png) );

	ASSERT_EXCEPTION( m_img_minimap_alpha.HasAlpha(), _T("map_select_1_png must have an alpha channel") );
	ASSERT_EXCEPTION( m_img_foreground.HasAlpha(),    _T("map_select_2_png must have an alpha channel") );

	m_img_minimap_loading = wxBitmap( BlendImage( m_img_foreground, m_img_background, false ) );
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
}


void MapGridCtrl::Clear()
{
	m_maps_unused.insert( m_maps.begin(), m_maps.end() );
	m_maps_unused.insert( m_maps_filtered.begin(), m_maps_filtered.end() );
	m_pending_maps.clear();
	m_maps.clear();
	m_maps_filtered.clear();
	m_grid.clear();
	m_mouseover_map = NULL; // can't be sure pointer will stay valid
	m_selected_map = NULL;
}


void MapGridCtrl::AddMap( const wxString& mapname )
{
	// no duplicates (would crash because of dangling MapData pointers in m_grid)
	if ( m_maps.find( mapname ) != m_maps.end() ) return;

	// check if we still have it in m_maps_unused..
	MapMap::iterator it = m_maps_unused.find( mapname );
	if ( it != m_maps_unused.end() ) {
		m_maps.insert( *it );
		m_grid.push_back( &m_maps[mapname] );
		m_maps_unused.erase( it );
		UpdateGridSize();
		return;
	}

	// if not, get it from unitsync
	FetchMapInfo( mapname );
}


void MapGridCtrl::AddMap( const UnitSyncMap& map )
{
	// no duplicates (would crash because of dangling MapData pointers in m_grid)
	if ( m_maps.find( map.name ) != m_maps.end() ) return;
	// don't want map to exist in both m_maps and m_maps_unused.
	m_maps_unused.erase( map.name );

	m_maps[map.name] = map;
	m_grid.push_back( &m_maps[map.name] );
	UpdateGridSize();
}


void MapGridCtrl::UpdateGridSize()
{
	// recalculate grid size (keep it approximately square)
	const int width = int(sqrt( double(m_maps.size()) ) + 0.5);
	m_size.x = width;
	m_size.y = (m_maps.size() + width - 1) / width;
	CheckInBounds();
}


void MapGridCtrl::CheckInBounds()
{
	const int size = MINIMAP_SIZE + MINIMAP_MARGIN;

	int width, height;
	GetClientSize( &width, &height );

	// this ensures always at least a quarter of the client area contains minimaps
	//m_pos.x = std::max( -width  / 2, std::min( size * m_size.x - width  / 2, m_pos.x ) );
	//m_pos.y = std::max( -height / 2, std::min( size * m_size.y - height / 2, m_pos.y ) );

	// this ensures the maps are centered when they fit on the screen,
	// otherwise it ensures the entire client area contains minimaps.
	if ( width / size >= m_size.x )
		m_pos.x = -(width - m_size.x * size) / 2;
	else
		m_pos.x = std::max( -1, std::min( size * m_size.x - width, m_pos.x ) );

	if ( height / size >= m_size.y )
		m_pos.y = -(height - m_size.y * size) / 2;
	else
		m_pos.y = std::max( -1, std::min( size * m_size.y - height, m_pos.y ) );
}


void MapGridCtrl::UpdateAsyncFetches()
{
	while ( m_async_mapinfo_fetches < MAX_MAPINFO_FETCHES && !m_pending_maps.empty() ) {
		wxString mapname = m_pending_maps.back();
		m_pending_maps.pop_back();
		FetchMapInfo( mapname );
	}

	// no minimap fetches until all mapinfo fetches are finished
	if ( m_async_mapinfo_fetches == 0 && m_async_minimap_fetches == 0 ) {
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
}


void MapGridCtrl::FetchMapInfo( const wxString& mapname )
{
	if ( m_async_mapinfo_fetches < MAX_MAPINFO_FETCHES ) {
		m_async.GetMapEx( mapname );
		++m_async_mapinfo_fetches;
	}
	else {
		m_pending_maps.push_back( mapname );
	}
}


void MapGridCtrl::FetchMinimap( MapData& map )
{
	// must be finished fetching mapinfos
	if ( m_async_mapinfo_fetches == 0 && m_async_minimap_fetches < MAX_MINIMAP_FETCHES ) {
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
			// fall through, both when starting fetch and when waiting
			// for it to finish, we want to show temporary image
		case MapState_GetMinimap:
			// draw temporary image while waiting for async fetch of minimap
			dc.DrawBitmap( m_img_minimap_loading, x, y );
			break;
		case MapState_GotMinimap:
			x += (MINIMAP_SIZE - map.minimap.GetWidth()) / 2;
			y += (MINIMAP_SIZE - map.minimap.GetHeight()) / 2;
			dc.DrawBitmap( map.minimap, x, y, false );
			// draw selection
			if ( &map == m_selected_map ) {
				dc.SetPen( wxPen( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) ) );
				dc.SetBrush( wxBrush( wxColour(0, 0, 0), wxTRANSPARENT ) );
				dc.DrawRectangle( x - 1, y - 1, map.minimap.GetWidth() + 2, map.minimap.GetHeight() + 2 );
			}
			break;
        default:
            wxLogError( _T("unknonw map.state in MapGridCtrl::DrawMap") );
            break;
	}
}


void MapGridCtrl::DrawBackground( wxDC& dc )
{
	int width, height;
	GetClientSize( &width, &height );

	dc.SetPen( wxPen( *wxLIGHT_GREY ) );
	dc.SetBrush( wxBrush( *wxLIGHT_GREY, wxSOLID ) );

	dc.DrawRectangle( 0, 0, width, height );
}


void MapGridCtrl::OnPaint( wxPaintEvent& /*unused*/ )
{
	// This line must come first, to avoid an endless succession of paint messages.
	// OnPaint handlers must always create a wxPaintDC.

	wxAutoBufferedPaintDC dc( this );

	DrawBackground( dc );

	if ( m_maps.empty() ) return;

	int width, height;
	GetClientSize( &width, &height );

	const int size = MINIMAP_SIZE + MINIMAP_MARGIN;

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
    SetCursor( wxCursor( wxCURSOR_HAND ) );
	if ( m_in_mouse_drag ) {
		// Fix for for not receiving LeftUp event when left button
		// is released outside the control (happens on Windows.)
		if ( !event.LeftIsDown() ) {
			OnLeftUp( event );
			return;
		}

		m_pos -= (event.GetPosition() - m_last_mouse_pos);
		m_last_mouse_pos = event.GetPosition();

		CheckInBounds();
		Refresh();
	}
	else {
		const wxPoint pos_unscaled = event.GetPosition() + m_pos;
		const wxPoint pos = wxPoint2DInt(pos_unscaled) / (MINIMAP_SIZE + MINIMAP_MARGIN);
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

		if ( m_mouseover_map != old_mouseover_map ) {
			if ( m_selection_follows_mouse && m_mouseover_map != NULL ) {
				SelectMap( m_mouseover_map );
			}
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
		SelectMap( m_mouseover_map );
	}
}


void MapGridCtrl::SelectMap( MapData* map )
{
	m_selected_map = map;

	if ( m_selected_map != NULL ) {
		wxLogMessage( _T("MapGridCtrl: Selected map: ") + m_selected_map->name );

		wxCommandEvent evt( MapSelectedEvt, GetId() );
		evt.SetEventObject( this );
		evt.SetString( m_selected_map->name );
		wxPostEvent( this, evt );
	}

	Refresh();
}


void MapGridCtrl::SetMinimap( MapMap& maps, const wxString& mapname, const wxBitmap& minimap )
{
	MapMap::iterator it = maps.find( mapname );

	if ( it != maps.end() ) {
		it->second.minimap = minimap;
		it->second.state = MapState_GotMinimap;
	}
}


void MapGridCtrl::OnGetMapImageAsyncCompleted( wxCommandEvent& event )
{
	wxString mapname = event.GetString();

	wxLogDebugFunc( mapname );

	// if mapname is empty, some error occurred in usync().GetMinimap...
	if ( !mapname.empty() ) {
		wxImage minimap( usync().GetMinimap( mapname, MINIMAP_SIZE, MINIMAP_SIZE ) );

		const int w = minimap.GetWidth();
		const int h = minimap.GetHeight();
		wxImage background( BorderInvariantResizeImage( m_img_background, w, h ) );
		wxImage minimap_alpha( BorderInvariantResizeImage( m_img_minimap_alpha, w, h ) );
		wxImage foreground( BorderInvariantResizeImage( m_img_foreground, w, h ) );

		minimap.SetAlpha( minimap_alpha.GetAlpha(), true /* "static data" */ );
		minimap = BlendImage( minimap, background, false );
		minimap = BlendImage( foreground, minimap, false );

		// set the minimap in all MapMaps
		wxBitmap minimap_bmp( minimap );
		SetMinimap( m_maps, mapname, minimap_bmp );
		SetMinimap( m_maps_unused, mapname, minimap_bmp );
		SetMinimap( m_maps_filtered, mapname, minimap_bmp );

		Refresh(); // TODO: use RefreshRect ?
	}

	--m_async_minimap_fetches;
	UpdateAsyncFetches();
}


void MapGridCtrl::OnGetMapExAsyncCompleted( wxCommandEvent& event )
{
	wxString mapname = event.GetString();

	wxLogDebugFunc( mapname );

	// if mapname is empty, some error occurred in usync().GetMapEx...
	if ( !mapname.empty() ) {
		try {
			AddMap( usync().GetMapEx( mapname ) );
		}
		catch (...) {}

		Refresh();
	}

	--m_async_mapinfo_fetches;
	UpdateAsyncFetches();

	// UpdateAsyncFetches didn't start a new one, so we finished
	// and can raise the LoadingCompletedEvt
	if ( m_async_mapinfo_fetches == 0 ) {
		wxCommandEvent evt( LoadingCompletedEvt, GetId() );
		evt.SetEventObject( this );
		wxPostEvent( this, evt );
	}
}
