/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <functional>
#include <wx/dcbuffer.h>
#include <wx/geometry.h>
#include <wx/settings.h>
#include <wx/log.h>

#define HAVE_WX
#include "mapgridctrl.h"

#include "settings.h"
#include "uiutils.h"
#include "utils/conversion.h"


#include <algorithm>
#include <lslutils/misc.h>
#include <lslunitsync/image.h>
#include "log.h"

#include "images/map_select_1.png.h"
#include "images/map_select_2.png.h"
/// Size of the map previews.  This should be same as size of map previews in
/// battle list and as prefetch size in SpringUnitSync for performance reasons.
const int MINIMAP_SIZE = 98;

/// Margin between the map previews, in pixels.
const int MINIMAP_MARGIN = 1;

BEGIN_EVENT_TABLE(MapGridCtrl, wxPanel)
EVT_PAINT(MapGridCtrl::OnPaint)
EVT_SIZE(MapGridCtrl::OnResize)
EVT_MOTION(MapGridCtrl::OnMouseMove)
EVT_LEFT_DOWN(MapGridCtrl::OnLeftDown)
EVT_LEFT_UP(MapGridCtrl::OnLeftUp)

EVT_COMMAND(wxID_ANY, REFRESH_EVENT, MapGridCtrl::OnRefresh)
//	EVT_COMMAND( 2, UnitSyncAsyncOperationCompletedEvt, MapGridCtrl::OnGetMapImageAsyncCompleted )
//	EVT_COMMAND( 3, UnitSyncAsyncOperationCompletedEvt, MapGridCtrl::OnGetMapExAsyncCompleted )
END_EVENT_TABLE()


const wxEventType MapGridCtrl::MapSelectedEvt = wxNewEventType();
const wxEventType MapGridCtrl::LoadingCompletedEvt = wxNewEventType();


MapGridCtrl::MapGridCtrl(wxWindow* parent, wxSize size, wxWindowID id)
    : wxPanel(parent, id, wxDefaultPosition, size, wxSIMPLE_BORDER | wxFULL_REPAINT_ON_RESIZE)
    , m_async_image(std::bind(&MapGridCtrl::OnGetMapImageAsyncCompleted, this, std::placeholders::_1))
    , m_async_ex(std::bind(&MapGridCtrl::OnGetMapExAsyncCompleted, this, std::placeholders::_1))
    , m_async_ops_count(0)
    , m_selection_follows_mouse(sett().GetMapSelectorFollowsMouse())
    , m_size(0, 0)
    , m_pos(0, 0)
    , m_in_mouse_drag(false)
    , m_mouseover_map(NULL)
    , m_selected_map(NULL)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(*wxLIGHT_GREY);

	m_img_background.Create(MINIMAP_SIZE, MINIMAP_SIZE, false /*don't clear*/);
	wxRect rect(0, 0, MINIMAP_SIZE, MINIMAP_SIZE);
	wxColour color(GetBackgroundColour());
	m_img_background.SetRGB(rect, color.Red(), color.Green(), color.Blue());

	m_img_minimap_alpha = charArr2wxImage(map_select_1_png, sizeof(map_select_1_png));
	m_img_foreground = charArr2wxImage(map_select_2_png, sizeof(map_select_2_png));

	ASSERT_EXCEPTION(m_img_minimap_alpha.HasAlpha(), _T("map_select_1_png must have an alpha channel"));
	ASSERT_EXCEPTION(m_img_foreground.HasAlpha(), _T("map_select_2_png must have an alpha channel"));

	m_img_minimap_loading = wxBitmap(BlendImage(m_img_foreground, m_img_background, false));
}


MapGridCtrl::~MapGridCtrl()
{
	//m_mutex.Lock(); //FIXME: deadlocks sometimes, WTF?!
	m_async_ex.Disconnect();
	m_async_image.Disconnect();
	Clear();
	m_pending_mapimages.clear();
	m_pending_mapinfos.clear();
	m_async_ops_count = 0;
	m_grid.clear();
	m_maps.clear();
	//m_mutex.Unlock();
}


#define CMP(var)                       \
	if (a->info.var < b->info.var) \
		return -1;             \
	if (a->info.var > b->info.var) \
		return 1;              \
	return 0

#define CMP2(a, b)         \
	if ((a) < (b))     \
		return -1; \
	if ((a) > (b))     \
		return 1;  \
	return 0

inline int MapGridCtrl::CompareName(const MapData* a, const MapData* b)
{
	return LSL::Util::Predicates::CaseInsensitive(a->name).cmp(b->name);
}
inline int MapGridCtrl::CompareTidalStrength(const MapData* a, const MapData* b)
{
	CMP(tidalStrength);
}
inline int MapGridCtrl::CompareGravity(const MapData* a, const MapData* b)
{
	CMP(gravity);
}
inline int MapGridCtrl::CompareMaxMetal(const MapData* a, const MapData* b)
{
	CMP(maxMetal);
}
inline int MapGridCtrl::CompareExtractorRadius(const MapData* a, const MapData* b)
{
	CMP(extractorRadius);
}
inline int MapGridCtrl::CompareMinWind(const MapData* a, const MapData* b)
{
	CMP(minWind);
}
inline int MapGridCtrl::CompareMaxWind(const MapData* a, const MapData* b)
{
	CMP(maxWind);
}
inline int MapGridCtrl::CompareWind(const MapData* a, const MapData* b)
{
	CMP2((a->info.minWind + a->info.maxWind), (b->info.minWind + b->info.maxWind));
}
inline int MapGridCtrl::CompareArea(const MapData* a, const MapData* b)
{
	CMP2((a->info.width * a->info.height), (b->info.width * b->info.height));
}
static inline double AspectRatio(const LSL::MapInfo& x)
{
	const int max = std::max(x.width, x.height);
	const int min = std::min(x.width, x.height);
	return (double)max / (min != 0 ? min : 1);
}
inline int MapGridCtrl::CompareAspectRatio(const MapData* a, const MapData* b)
{
	CMP2(AspectRatio(a->info), AspectRatio(b->info));
}
inline int MapGridCtrl::ComparePosCount(const MapData* a, const MapData* b)
{
	CMP(positions.size());
}

#undef CMP2
#undef CMP

template <class Compare>
inline void MapGridCtrl::_Sort(int dimension, Compare cmp)
{
	if (dimension <= 1) {
		// vertical sort (ie. sort entire dataset)
		std::stable_sort(m_grid.begin(), m_grid.end(), cmp);
	} else /*if ( dimension == 2 )*/ {
		// horizontal sort (ie. sort each row individually)
		for (int y = 0; y < m_size.y; ++y) {
			const int idx1 = y * m_size.x;
			const int idx2 = std::min(int(m_grid.size()), idx1 + m_size.x);
			std::stable_sort(m_grid.begin() + idx1, m_grid.begin() + idx2, cmp);
		}
	}
}


void MapGridCtrl::Sort(SortKey vertical, SortKey horizontal, bool vertical_direction, bool horizontal_direction)
{
	if (m_maps.empty())
		return;

	// Always start by sorting on name, to get duplicate maps together.
	SortKey keys[3] = {SortKey_Name, vertical, horizontal};
	bool dirs[3] = {false, vertical_direction, horizontal_direction};

	// This looks like common antipattern 'loop switch sequence', however here
	// it's the best way I found to prevent duplication of the switch statement,
	// which will probably require most (all?) changes and possibly grow big.

	for (int i = 0; i < 3; ++i) {
		// Do nothing if current sortkey is same as previous one.
		if (i > 0 && keys[i] == keys[i - 1] && dirs[i] == dirs[i - 1])
			continue;
		// Sort dimension i on sortkey keys[i].
		const bool d = dirs[i];
		switch (keys[i]) {
			case SortKey_Name:
				_Sort(i, _Compare(d, CompareName));
				break;
			case SortKey_TidalStrength:
				_Sort(i, _Compare(d, CompareTidalStrength));
				break;
			case SortKey_Gravity:
				_Sort(i, _Compare(d, CompareGravity));
				break;
			case SortKey_MaxMetal:
				_Sort(i, _Compare(d, CompareMaxMetal));
				break;
			case SortKey_ExtractorRadius:
				_Sort(i, _Compare(d, CompareExtractorRadius));
				break;
			case SortKey_MinWind:
				_Sort(i, _Compare(d, CompareMinWind));
				break;
			case SortKey_MaxWind:
				_Sort(i, _Compare(d, CompareMaxWind));
				break;
			case SortKey_Wind:
				_Sort(i, _Compare(d, CompareWind));
				break;
			case SortKey_Area:
				_Sort(i, _Compare(d, CompareArea));
				break;
			case SortKey_AspectRatio:
				_Sort(i, _Compare(d, CompareAspectRatio));
				break;
			case SortKey_PosCount:
				_Sort(i, _Compare(d, ComparePosCount));
				break;
			default:
				ASSERT_EXCEPTION(false, _T("unimplemented SortKey in MapGridCtrl::Sort"));
				break;
		}
	}
}


void MapGridCtrl::Clear()
{
	m_grid.clear();
	m_mouseover_map = NULL; // can't be sure pointer will stay valid
	m_selected_map = NULL;
	m_size.x = 0;
	m_size.y = 0;
}


bool MapGridCtrl::IsInGrid(const wxString& mapname)
{
	for (const MapData* gridentry : m_grid) {
		if (gridentry->name == mapname) {
			return true;
		}
	}
	return false;
}

void MapGridCtrl::AddMap(const wxString& mapname)
{
	assert(wxThread::IsMain());
	assert(!mapname.empty());

	if (!LSL::usync().MapExists(STD_STRING(mapname))) {
		//FIXME: offer download button on image instead?
		wxLogWarning(_("Map %s doesn't exist!"), mapname.wc_str());
		return;
	}

	// no duplicates (would crash because of dangling MapData pointers in m_grid)
	if (m_maps.find(mapname) == m_maps.end()) {
		MapData m;
		m.name = mapname.mb_str();
		m_maps[mapname] = m;
		m_pending_mapinfos.push_back(&m_maps[mapname]);
		m_pending_mapimages.push_back(&m_maps[mapname]);
		UpdateAsyncFetches();
	}

	if (IsInGrid(mapname)) {
		wxLogWarning(_("Map %s already in grid!"), mapname.wc_str());
		return;
	}

	m_grid.push_back(&m_maps[mapname]);
	UpdateGridSize();
}

void MapGridCtrl::UpdateGridSize()
{
	// recalculate grid size (keep it approximately square)
	const int width = int(sqrt(double(m_grid.size())) + 0.5);
	m_size.x = width;
	m_size.y = (m_grid.size() + width - 1) / width;
	CheckInBounds();
}


void MapGridCtrl::CheckInBounds()
{
	const int size = MINIMAP_SIZE + MINIMAP_MARGIN;

	int width, height;
	GetClientSize(&width, &height);

	// this ensures always at least a quarter of the client area contains minimaps
	//m_pos.x = std::max( -width  / 2, std::min( size * m_size.x - width  / 2, m_pos.x ) );
	//m_pos.y = std::max( -height / 2, std::min( size * m_size.y - height / 2, m_pos.y ) );

	// this ensures the maps are centered when they fit on the screen,
	// otherwise it ensures the entire client area contains minimaps.
	if (width / size >= m_size.x)
		m_pos.x = -(width - m_size.x * size) / 2;
	else
		m_pos.x = std::max(-1, std::min(size * m_size.x - width, m_pos.x));

	if (height / size >= m_size.y)
		m_pos.y = -(height - m_size.y * size) / 2;
	else
		m_pos.y = std::max(-1, std::min(size * m_size.y - height, m_pos.y));
}

MapGridCtrl::MapData* MapGridCtrl::GetMaxPriorityMap(std::list<MapData*>& maps)
{
	assert(!maps.empty());
	unsigned max = 0;
	std::list<MapData*>::iterator it;
	std::list<MapData*>::iterator maxpos = maps.begin();
	for (it = maps.begin(); it != maps.end(); ++it) {
		if ((*it)->priority > max) {
			max = (*it)->priority;
			maxpos = it;
		}
	}
	MapData* ret = *maxpos;
	maps.erase(maxpos);
	return ret;
}

void MapGridCtrl::UpdateAsyncFetches()
{
	if (m_async_ops_count > 2)
		return;
	if (!m_pending_mapinfos.empty()) {
		m_async_ops_count++;
		const MapData* m = GetMaxPriorityMap(m_pending_mapinfos);
		m_async_ex.GetMapImageAsync(m->name, LSL::IMAGE_MAP_THUMB, MINIMAP_SIZE, MINIMAP_SIZE);
	} else if (!m_pending_mapimages.empty()) {
		MapData* m = GetMaxPriorityMap(m_pending_mapimages);
		if (m->state != MapState_NoMinimap) //FIXME: this shouldn never happen
			return;
		m_async_ops_count++;

		m->state = MapState_GetMinimap;
		m_async_image.GetMapImageAsync(m->name, LSL::IMAGE_MAP_THUMB, MINIMAP_SIZE, MINIMAP_SIZE);
	} else {
		wxCommandEvent evt(LoadingCompletedEvt, GetId());
		evt.SetEventObject(this);
		wxPostEvent(this, evt);
	}
}


void MapGridCtrl::DrawMap(wxDC& dc, MapData& map, int x, int y)
{
	switch (map.state) {
		case MapState_NoMinimap:
			map.priority = 1;
			UpdateAsyncFetches();
			[[fallthrough]];
		// fall through, both when starting fetch and when waiting
		// for it to finish, we want to show temporary image
		case MapState_GetMinimap:
			// draw temporary image while waiting for async fetch of minimap
			dc.DrawBitmap(m_img_minimap_loading, x, y);
			break;
		case MapState_GotMinimap:
			x += (MINIMAP_SIZE - map.minimap.GetWidth()) / 2;
			y += (MINIMAP_SIZE - map.minimap.GetHeight()) / 2;
			dc.DrawBitmap(map.minimap, x, y, false);
			// draw selection
			if (&map == m_selected_map) {
				dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
				dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxTRANSPARENT));
				dc.DrawRectangle(x - 1, y - 1, map.minimap.GetWidth() + 2, map.minimap.GetHeight() + 2);
			}
			break;
		default:
			wxLogError(_T("unknonw map.state in MapGridCtrl::DrawMap"));
			break;
	}
}


void MapGridCtrl::DrawBackground(wxDC& dc)
{
	int width, height;
	GetClientSize(&width, &height);

	dc.SetPen(wxPen(*wxLIGHT_GREY));
	dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));

	dc.DrawRectangle(0, 0, width, height);
}


void MapGridCtrl::OnPaint(wxPaintEvent& /*unused*/)
{
	// This line must come first, to avoid an endless succession of paint messages.
	// OnPaint handlers must always create a wxPaintDC.

	wxAutoBufferedPaintDC dc(this);

	DrawBackground(dc);

	if (m_maps.empty())
		return;

	int width, height;
	GetClientSize(&width, &height);

	const int size = MINIMAP_SIZE + MINIMAP_MARGIN;

	// simple tile mapping algorithm
	int start_scrn_x = -(m_pos.x % size);
	int start_scrn_y = -(m_pos.y % size);
	int start_grid_x = m_pos.x / size;
	int start_grid_y = m_pos.y / size;
	int end_grid_x = std::min(start_grid_x + (width + 2 * size - 1) / size, m_size.x);
	int end_grid_y = std::min(start_grid_y + (height + 2 * size - 1) / size, m_size.y);

	if (start_grid_x < 0) {
		start_scrn_x += size * -start_grid_x;
		start_grid_x = 0;
	}

	if (start_grid_y < 0) {
		start_scrn_y += size * -start_grid_y;
		start_grid_y = 0;
	}

	for (int y = start_grid_y, scrn_y = start_scrn_y; y < end_grid_y; ++y, scrn_y += size) {
		for (int x = start_grid_x, scrn_x = start_scrn_x; x < end_grid_x; ++x, scrn_x += size) {
			const int idx = y * m_size.x + x;
			if (idx >= int(m_grid.size()))
				break;
			DrawMap(dc, *m_grid[idx], scrn_x, scrn_y);
		}
	}
}


void MapGridCtrl::OnResize(wxSizeEvent&)
{
	CheckInBounds();
}


void MapGridCtrl::OnMouseMove(wxMouseEvent& event)
{
	SetCursor(wxCursor(wxCURSOR_HAND));
	if (m_in_mouse_drag) {
		// Fix for for not receiving LeftUp event when left button
		// is released outside the control (happens on Windows.)
		if (!event.LeftIsDown()) {
			OnLeftUp(event);
			return;
		}

		m_pos -= (event.GetPosition() - m_last_mouse_pos);
		m_last_mouse_pos = event.GetPosition();

		CheckInBounds();
		Refresh();
	} else {
		const wxPoint pos_unscaled = event.GetPosition() + m_pos;
		const wxPoint pos = wxPoint2DInt(pos_unscaled) / (MINIMAP_SIZE + MINIMAP_MARGIN);
		const int idx = pos.y * m_size.x + pos.x;
		MapData* old_mouseover_map = m_mouseover_map;

		// use pos_unscaled for tests against 0 because negative values lower
		// than MINIMAP_SIZE get rounded up to 0 when diviving by MINIMAP_SIZE..
		if (pos_unscaled.x >= 0 && pos.x < m_size.x && pos_unscaled.y >= 0 && pos.y <= m_size.y && idx < (int)m_grid.size()) {
			m_mouseover_map = m_grid[idx];
		} else {
			m_mouseover_map = NULL;
		}

		if (m_mouseover_map != old_mouseover_map) {
			if (m_selection_follows_mouse && m_mouseover_map != NULL) {
				SelectMap(m_mouseover_map);
			}
		}
	}
}


void MapGridCtrl::OnLeftDown(wxMouseEvent& event)
{
	SetCursor(wxCursor(wxCURSOR_HAND));
	m_first_mouse_pos = event.GetPosition();
	m_last_mouse_pos = event.GetPosition();
	m_in_mouse_drag = true;
}


void MapGridCtrl::OnLeftUp(wxMouseEvent& event)
{
	SetCursor(wxCursor(wxCURSOR_ARROW));
	m_in_mouse_drag = false;

	if (wxPoint2DInt(event.GetPosition() - m_first_mouse_pos).GetVectorLength() <= 3) {
		SelectMap(m_mouseover_map);
	}
}


void MapGridCtrl::SelectMap(MapData* map)
{
	if ((map != NULL) && (m_selected_map != map)) {
		m_selected_map = map;
		wxCommandEvent evt(MapSelectedEvt, GetId());
		evt.SetEventObject(this);
		evt.SetString(TowxString(m_selected_map->name));
		wxPostEvent(this, evt);
		Refresh();
	}
}

void MapGridCtrl::OnGetMapImageAsyncCompleted(const std::string& _mapname)
{
	m_mutex.Lock();
	// if mapname is empty, some error occurred in LSL::usync().GetMinimap...
	if (!m_async_ex.Connected() || !m_async_image.Connected() || _mapname.empty()) {
		m_mutex.Unlock();
		return;
	}
	const wxString mapname = TowxString(_mapname);
	wxImage minimap(LSL::usync().GetScaledMapImage(_mapname, LSL::IMAGE_MAP_THUMB, MINIMAP_SIZE, MINIMAP_SIZE).wximage());

	const int w = minimap.GetWidth();
	const int h = minimap.GetHeight();
	wxImage background(BorderInvariantResizeImage(m_img_background, w, h));
	wxImage minimap_alpha(BorderInvariantResizeImage(m_img_minimap_alpha, w, h));
	wxImage foreground(BorderInvariantResizeImage(m_img_foreground, w, h));

	minimap.SetAlpha(minimap_alpha.GetAlpha(), true /* "static data" */);
	minimap = BlendImage(minimap, background, false);
	minimap = BlendImage(foreground, minimap, false);

	// set the minimap in all MapMaps
	m_maps[mapname].minimap = wxBitmap(minimap);
	m_maps[mapname].state = MapState_GotMinimap;
	if (m_async_ops_count > 0) //WTF, why is this needed?
		m_async_ops_count--;

	// never ever call a gui function here, it will crash! (in 1/100 cases)
	wxCommandEvent evt(REFRESH_EVENT, GetId());
	evt.SetEventObject(this);
	wxPostEvent(this, evt);
	m_mutex.Unlock();
}


void MapGridCtrl::OnGetMapExAsyncCompleted(const std::string& _mapname)
{
	m_mutex.Lock();
	// if mapname is empty, some error occurred in LSL::usync().GetMapEx...
	if (!m_async_ex.Connected() || !m_async_image.Connected() || _mapname.empty()) {
		m_mutex.Unlock();
		return;
	}
	const wxString mapname = TowxString(_mapname);
	LSL::UnitsyncMap m = LSL::usync().GetMap(_mapname);
	m_maps[mapname].hash = m.hash;
	m_maps[mapname].info = m.info;
	m_async_ops_count--;
	m_mutex.Unlock();
}

void MapGridCtrl::OnRefresh(wxCommandEvent& /*event*/)
{
	Refresh();
}
