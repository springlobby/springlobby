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

		static const wxEventType MapSelectedEvt;

		enum SortKey
		{
			SortKey_Name,
			SortKey_TidalStrength,
			SortKey_Gravity,
			SortKey_MaxMetal,
			SortKey_ExtractorRadius,
			SortKey_MinWind,
			SortKey_MaxWind,
			SortKey_Wind,        // minWind + maxWind
			SortKey_Area,        // width * height
			SortKey_AspectRatio, // max(width/height, height/width)
			SortKey_PosCount,
		};

		MapGridCtrl( wxWindow* parent, Ui& ui, wxSize size = wxDefaultSize, wxWindowID id = -1 );
		~MapGridCtrl();

		void LoadMaps();
		void Sort( SortKey vertical, SortKey horizontal );

		UnitSyncMap* GetSelectedMap() const { return m_selected_map; }

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
		static bool CompareTidalStrength( const MapData* a, const MapData* b );
		static bool CompareGravity( const MapData* a, const MapData* b );
		static bool CompareMaxMetal( const MapData* a, const MapData* b );
		static bool CompareExtractorRadius( const MapData* a, const MapData* b );
		static bool CompareMinWind( const MapData* a, const MapData* b );
		static bool CompareMaxWind( const MapData* a, const MapData* b );
		static bool CompareWind( const MapData* a, const MapData* b );
		static bool CompareArea( const MapData* a, const MapData* b );
		static bool CompareAspectRatio( const MapData* a, const MapData* b );
		static bool ComparePosCount( const MapData* a, const MapData* b );
		template< class Compare > void _Sort( int dimension, Compare cmp );

		void CheckInBounds();
		void UpdateToolTip();
		void DrawMap( wxDC& dc, MapData& map, int x, int y );

		Ui& m_ui;
		UnitSyncAsyncOps m_async;

		std::map< wxString, MapData > m_maps;
		std::vector< MapData* > m_grid;
		wxSize m_size;

		wxPoint m_pos;
		wxPoint m_first_mouse_pos;
		wxPoint m_last_mouse_pos;
		bool m_in_mouse_drag;

		/// Number of async minimap fetches still running on behalf of this control.
		/// This number is limited so the control can adapt (faster) to changes in
		/// the set of visible maps.  (it fetches only visible maps)
		int m_async_minimap_fetches;

		/// Map which is currently under the mouse pointer.
		MapData* m_mouseover_map;
		/// Map which was last clicked.
		MapData* m_selected_map;

		DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
