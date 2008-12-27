/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H

#include "iunitsync.h"
#include <wx/bitmap.h>
#include <wx/panel.h>

class Ui;


class MapGridCtrl : public wxPanel
{
	public:

		enum SortKey
		{
			SortKey_Name,
			SortKey_Area,
			SortKey_PosCount,
		};

		MapGridCtrl( wxWindow* parent, Ui& ui, wxSize size = wxDefaultSize );
		~MapGridCtrl();

		void LoadMaps();
		void Sort( SortKey vertical, SortKey horizontal );

		void OnPaint( wxPaintEvent& event );
		void OnResize( wxSizeEvent& event );

		void OnMouseMove( wxMouseEvent& event );
		void OnLeftDown( wxMouseEvent& event );
		void OnLeftUp( wxMouseEvent& event );

		void OnGetMapImageAsyncCompleted( wxCommandEvent& event );

	protected:

		enum MapState
		{
			MapState_NoMinimap,
			MapState_GetMinimap,
			MapState_GotMinimap
		};

		struct MapData : UnitSyncMap
		{
			MapData() : state( MapState_NoMinimap ) {}
			void operator=( const UnitSyncMap& other ) { UnitSyncMap::operator=( other ); }

			wxBitmap minimap;
			MapState state;
		};

		static bool CompareName( const MapData* a, const MapData* b );
		static bool CompareArea( const MapData* a, const MapData* b );
		static bool ComparePosCount( const MapData* a, const MapData* b );
		template< class Compare > void _Sort( int dimension, Compare cmp );

		void CheckInBounds();
		void DrawMap( wxDC& dc, MapData& map, int x, int y );

		Ui& m_ui;

		std::map< wxString, MapData > m_maps;
		std::vector< MapData* > m_grid;
		wxSize m_size;

		wxPoint m_pos;
		wxPoint m_last_mouse_pos;
		bool m_in_mouse_drag;

		/// Number of async minimap fetches still running on behalf of this control.
		/// This number is limited so the control can adapt (faster) to changes in
		/// the set of visible maps.  (it fetches only visible maps)
		int m_async_minimap_fetches;

		/// Map which is currently under the mouse pointer.
		MapData* m_mouseover_map;

		DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
