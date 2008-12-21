/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

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
  EVT_COMMAND( wxID_ANY, UnitSyncGetMapImageAsyncCompletedEvt, MapGridCtrl::OnGetMapImageAsyncCompleted )
END_EVENT_TABLE()


MapGridCtrl::MapGridCtrl( wxWindow* parent, int size, IBattle* battle, Ui& ui )
	: wxPanel( parent, -1, wxDefaultPosition, wxSize(size, size), wxSIMPLE_BORDER|wxFULL_REPAINT_ON_RESIZE )
	, m_ui( ui )
	, m_battle( battle )
{
}


MapGridCtrl::~MapGridCtrl()
{

}


void MapGridCtrl::LoadMaps()
{
	wxArrayString maps = usync().GetMapList();
	int count = maps.size();

	wxLogDebugFunc( TowxString( maps.size() ) );

	m_maps.clear();

	for (int i = 0; i < count; ++i ) {
		try {
			m_maps[maps[i]] = usync().GetMapEx( maps[i] );
		}
		catch (...) {}
	}
}


void MapGridCtrl::SetBattle( IBattle* battle )
{
	m_battle = battle;
}


void MapGridCtrl::DrawMap( wxDC& dc, MapData& map, int x, int y )
{
	switch ( map.state ) {
		case MapState_NoMinimap:
			usync().GetMinimapAsync( map.name, MINIMAP_SIZE, MINIMAP_SIZE, this );
			map.state = MapState_GetMinimap;
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
	wxLogDebugFunc( _T("") );

	if ( m_maps.size() == 0 ) LoadMaps();

	wxPaintDC dc( this );

	int width, height;
	GetClientSize( &width, &height );

	std::map<wxString, MapData>::iterator it = m_maps.begin();
	if (it == m_maps.end()) return;

	const int size = MINIMAP_SIZE + 1;

	for (int y = (height % size) / 2; y <= height - size; y += size) {
		for (int x = (width % size) / 2; x <= width - size; x += size) {
			DrawMap( dc, it->second, x, y );
			++it;
			if (it == m_maps.end()) return;
		}
	}
}


void MapGridCtrl::OnResize( wxSizeEvent& event )
{
}


void MapGridCtrl::OnGetMapImageAsyncCompleted( wxCommandEvent& event )
{
	wxString mapname = event.GetString();

	wxLogDebugFunc( mapname );

	m_maps[mapname].minimap = usync().GetMinimap( mapname, MINIMAP_SIZE, MINIMAP_SIZE );
	m_maps[mapname].state = MapState_GotMinimap;

	Refresh(); // TODO: use RefreshRect ?
}
