/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <lslunitsync/unitsync.h>
class Ui;

class MapGridCtrl : public wxPanel
{
public:
	/// this event is raised each time when a new map is selected
	static const wxEventType MapSelectedEvt;
	/// this event is raised after the loading of map infos finished
	static const wxEventType LoadingCompletedEvt;

	enum SortKey {
		SortKey_Name,
		SortKey_TidalStrength,
		SortKey_Gravity,
		SortKey_MaxMetal,
		SortKey_ExtractorRadius,
		SortKey_MinWind,
		SortKey_MaxWind,
		SortKey_Wind,	// minWind + maxWind
		SortKey_Area,	// width * height
		SortKey_AspectRatio, // max(width/height, height/width)
		SortKey_PosCount
	};

	MapGridCtrl(wxWindow* parent, wxSize size = wxDefaultSize, wxWindowID id = -1);
	~MapGridCtrl();

	void Clear();
	void AddMap(const wxString& mapname);


	/* ===== sorting ===== */
	void Sort(SortKey vertical, SortKey horizontal, bool vertical_direction = false, bool horizontal_direction = false);

	/* ===== filtering ===== */
	template <class Predicate>
	int Filter(Predicate pred)
	{
		std::vector<wxString> maps;

		m_grid.clear();
		m_mouseover_map = NULL; // can't be sure pointer will stay valid
		m_selected_map = NULL;

		for (MapMap::const_iterator it = m_maps.begin(); it != m_maps.end(); ++it) {
			if (pred(it->second)) {
				AddMap(it->first);
			}
		}
		return m_maps.size();
	}

	LSL::UnitsyncMap* GetSelectedMap() const
	{
		return m_selected_map;
	}

private:
	void OnPaint(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);

	void OnMouseMove(wxMouseEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);

	void OnRefresh(wxCommandEvent& event);
	void CheckInBounds();

	enum MapState {
		MapState_NoMinimap,
		MapState_GetMinimap,
		MapState_GotMinimap
	};

	struct MapData : LSL::UnitsyncMap
	{
		MapData()
		    : state(MapState_NoMinimap)
		    , priority(0)
		{
		}
		void operator=(const LSL::UnitsyncMap& other)
		{
			LSL::UnitsyncMap::operator=(other);
		}

		wxBitmap minimap;
		MapState state;
		unsigned priority; //the higher the earlier data will be fetched, is increased by Draw()
	};

	typedef std::map<wxString, MapData> MapMap;

	// wrapper around the Compare*() methods below to allow changing sort direction
	template <class Compare>
	class _Compare2
	{
	public:
		_Compare2(bool direction, Compare cmp)
		    : m_cmp(cmp)
		    , m_direction(direction ? -1 : 1)
		{
		}
		bool operator()(const MapData* a, const MapData* b)
		{
			return (m_direction * m_cmp(a, b)) < 0;
		}

	private:
		Compare m_cmp;
		int m_direction;
	};

	// allow a _Compare2 to be constructed with implicit template arguments
	template <class Compare>
	_Compare2<Compare> _Compare(bool direction, Compare cmp)
	{
		return _Compare2<Compare>(direction, cmp);
	}

	// comparison methods returning -1 if a < b, 1 if a > b and 0 if !(a < b) && !(a > b)
	static int CompareName(const MapData* a, const MapData* b);
	static int CompareTidalStrength(const MapData* a, const MapData* b);
	static int CompareGravity(const MapData* a, const MapData* b);
	static int CompareMaxMetal(const MapData* a, const MapData* b);
	static int CompareExtractorRadius(const MapData* a, const MapData* b);
	static int CompareMinWind(const MapData* a, const MapData* b);
	static int CompareMaxWind(const MapData* a, const MapData* b);
	static int CompareWind(const MapData* a, const MapData* b);
	static int CompareArea(const MapData* a, const MapData* b);
	static int CompareAspectRatio(const MapData* a, const MapData* b);
	static int ComparePosCount(const MapData* a, const MapData* b);
	template <class Compare>
	void _Sort(int dimension, Compare cmp);

private:
	void OnGetMapImageAsyncCompleted(const std::string& _mapname);
	void OnGetMapExAsyncCompleted(const std::string& _mapname);
	void UpdateGridSize();
	void UpdateAsyncFetches();
	void FetchMapInfo(const wxString& mapname);
	void FetchMinimap(MapData& map);
	void DrawMap(wxDC& dc, MapData& map, int x, int y);
	void DrawBackground(wxDC& dc);
	void SetMinimap(MapData& mapdata, const wxBitmap& minimap);
	void SelectMap(MapData* map);
	bool IsInGrid(const wxString& mapname);
	MapData* GetMaxPriorityMap(std::list<MapData*>& maps);

	LSL::UnitSyncAsyncOps m_async_image;
	LSL::UnitSyncAsyncOps m_async_ex;

	int m_async_ops_count;

	const bool m_selection_follows_mouse;

	/// Set of maps which are queued to be fetched asynchronously.
	std::list<MapData*> m_pending_mapinfos;
	std::list<MapData*> m_pending_mapimages;

	MapMap m_maps; //list of all maps
	std::vector<MapData*> m_grid;
	wxSize m_size;

	wxPoint m_pos;
	wxPoint m_first_mouse_pos;
	wxPoint m_last_mouse_pos;
	bool m_in_mouse_drag;

	/// Map which is currently under the mouse pointer.
	MapData* m_mouseover_map;
	/// Map which was last clicked.
	MapData* m_selected_map;

	/// solid color background
	wxImage m_img_background;
	/// map_select_1_png, alpha channel of this is attached to minimap before
	/// blending it over m_img_background
	wxImage m_img_minimap_alpha;
	/// map_select_2_png, this is alpha blended on top of the minimap
	wxImage m_img_foreground;
	/// this is displayed for maps whose minimap has not yet been loaded
	wxBitmap m_img_minimap_loading;

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
