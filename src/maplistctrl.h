#ifndef SPRINGLOBBY_HEADERGUARD_MAPLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPLISTCTRL_H

#include "iunitsync.h"
#include <wx/bitmap.h>
#include <wx/panel.h>

class IBattle;
class Ui;


class MapListCtrl : public wxPanel
{
	public:
		MapListCtrl( wxWindow* parent, int size, IBattle* battle, Ui& ui );
		~MapListCtrl();

		void SetBattle( IBattle* battle );

		void OnPaint( wxPaintEvent& event );
		void OnResize( wxSizeEvent& event );

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

		void LoadMaps();
		void DrawMap( wxDC& dc, MapData& map, int x, int y );

		Ui& m_ui;
		IBattle* m_battle;

		std::map< wxString, MapData > m_maps;
		std::vector< std::vector<MapData*> > m_bins;

		DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPLISTCTRL_H
