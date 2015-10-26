/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <cmath>
#include <stdexcept>
#include <wx/log.h>
#include <wx/app.h>
#include <wx/toplevel.h>

#define HAVE_WX
#include <lslutils/conversion.h>
#include <lslutils/misc.h>
#include <lslunitsync/image.h>
#include <lslunitsync/data.h>

#include "utils/conversion.h"
#include "uiutils.h"
#include "mapctrl.h"
#include "user.h"
#include "ui.h"
#include "iserver.h"
#include "ibattle.h"
#include "settings.h"
#include "iconimagelist.h"
#include "hosting/addbotdialog.h"
#include "log.h"
#include "contentmanager.h"
#include "contentdownloadrequest.h"
#include "exception.h"

#include "images/close.xpm"
#include "images/close_hi.xpm"
#include "images/start_ally.xpm"
#include "images/start_enemy.xpm"
#include "images/start_unused.xpm"
#include "images/player.xpm"
#include "images/bot.xpm"
#include "images/no1_icon.png.h"
#include "images/no2_icon.png.h"
#include "images/up_down.xpm"
#include "images/upsel_down.xpm"
#include "images/up_downsel.xpm"
#include "images/not_found_icon.xpm"
#include "images/download_map.xpm"
#include "images/reload_map.xpm"
#include "utils/lslconversion.h"

const int USER_BOX_EXPANDED_HEIGHT = 70;
const int USER_BOX_EXPANDED_WIDTH = 75;

/** How much padding to place around the icon of a user box.
 */
const int USER_BOX_ICON_PADDING = 2;


/** Width of the e.g. player or bot images that mark a player/bot's
 * chosen start position.
 */
const int USER_BOX_ICON_WIDTH = 16;

/** Height of the e.g. player or bot images that mark a player/bot's
 * chosen start position.
 */
const int USER_BOX_ICON_HEIGHT = 16;

//int div is indeed wanted here
const int USER_BOX_ICON_HALFWIDTH = (USER_BOX_ICON_WIDTH / 2);
//const int USER_BOX_ICON_HALFHEIGHT = (USER_BOX_ICON_HEIGHT / 2);


const wxSize user_box_icon_size(USER_BOX_ICON_WIDTH + 2 * USER_BOX_ICON_PADDING,
				USER_BOX_ICON_HEIGHT + 2 * USER_BOX_ICON_PADDING);

const wxSize user_box_expanded_size(USER_BOX_EXPANDED_WIDTH, USER_BOX_EXPANDED_HEIGHT);

BEGIN_EVENT_TABLE(MapCtrl, wxPanel)
EVT_PAINT(MapCtrl::OnPaint)
EVT_SIZE(MapCtrl::OnResize)
EVT_MOTION(MapCtrl::OnMouseMove)
EVT_ENTER_WINDOW(MapCtrl::OnMouseEnter)
EVT_MOUSEWHEEL(MapCtrl::OnMouseWheel)
EVT_LEFT_DOWN(MapCtrl::OnLeftDown)
EVT_LEFT_UP(MapCtrl::OnLeftUp)
EVT_RIGHT_UP(MapCtrl::OnRightUp)
EVT_COMMAND(wxID_ANY, REFRESH_EVENT, MapCtrl::OnRefresh)
END_EVENT_TABLE()

/* Something to do with start box sizes. */
const int boxsize = 8;
const int minboxsize = 40;

static inline void WriteInt24(unsigned char* p, int i)
{
	p[0] = i & 0xFF;
	p[1] = (i >> 8) & 0xFF;
	p[2] = (i >> 16) & 0xFF;
}

static inline int ReadInt24(const unsigned char* p)
{
	return p[0] | (p[1] << 8) | (p[2] << 16);
}

MapCtrl::MapCtrl(wxWindow* parent, int size, IBattle* battle, bool readonly, bool draw_start_types, bool singleplayer)
    : wxPanel(parent, -1, wxDefaultPosition, wxSize(size, size), wxSIMPLE_BORDER | wxFULL_REPAINT_ON_RESIZE)
    , m_async(boost::bind(&MapCtrl::OnGetMapImageAsyncCompleted, this, _1))
    , m_minimap(0)
    , m_metalmap(0)
    , m_heightmap(0)
    , m_battle(battle)
    , m_mapname("")
    , m_draw_start_types(draw_start_types)
    , m_ro(readonly)
    , m_sp(singleplayer)
    , m_mover_rect(-2)
    , m_mdown_rect(0)
    , m_rect_area(Main)
    , m_last_rect_area(Main)
    , m_mdown_area(Main)
    , m_maction(None)
    , m_mdown_x(0)
    , m_mdown_y(0)
    , m_lastsize(-1, -1)
    , m_close_img(NULL)
    , m_close_hi_img(NULL)
    , m_start_ally(NULL)
    , m_start_enemy(NULL)
    , m_start_unused(NULL)
    , m_player_img(NULL)
    , m_bot_img(NULL)
    , m_nfound_img(NULL)
    , m_reload_img(NULL)
    , m_dl_img(NULL)
    , m_user_expanded(NULL)
    , m_current_infomap(IM_Minimap)
    , m_mutex()
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(*wxLIGHT_GREY);
	m_close_img = new wxBitmap(close_xpm);
	m_close_hi_img = new wxBitmap(close_hi_xpm);
	m_tmp_brect.ally = -1;
}


MapCtrl::~MapCtrl()
{
	FreeMinimap();
	delete m_close_img;
	delete m_close_hi_img;
	delete m_start_ally;
	delete m_start_enemy;
	delete m_start_unused;
	delete m_nfound_img;
	delete m_reload_img;
	delete m_dl_img;
	delete m_player_img;
	delete m_bot_img;
}


void MapCtrl::SetBattle(IBattle* battle)
{
	m_battle = battle;
	UpdateMinimap();
}


wxRect
MapCtrl::GetDrawableRect() const
{
	int w(0), h(0);
	GetClientSize(&w, &h);
	return wxRect(0, 0, w, h);
}

wxRect MapCtrl::GetMinimapRect() const
{
	if (m_minimap == 0)
		return wxRect();

	int cwidth, cheight, top = 0, left = 0;
	GetClientSize(&cwidth, &cheight);

	if (m_minimap->GetWidth() < cwidth) {
		left = (cwidth - m_minimap->GetWidth()) / 2;
	} else {
		top = (cheight - m_minimap->GetHeight()) / 2;
	}
	return wxRect(left, top, m_minimap->GetWidth(), m_minimap->GetHeight());
}


wxRect MapCtrl::GetStartRect(int index) const
{
	BattleStartRect sr = m_battle->GetStartRect(index);
	if (!sr.IsOk())
		return wxRect();
	return GetStartRect(sr);
}


wxRect MapCtrl::GetStartRect(const BattleStartRect& sr) const
{
	int x1, y1, x2, y2;
	if (!sr.exist || sr.todelete)
		return wxRect();
	wxRect mr = GetMinimapRect();

	x1 = int((mr.x + sr.left * mr.width / 200.0) + 0.5);
	y1 = int((mr.y + sr.top * mr.height / 200.0) + 0.5);
	x2 = int((mr.x + sr.right * mr.width / 200.0) + 0.5);
	y2 = int((mr.y + sr.bottom * mr.height / 200.0) + 0.5);

	return wxRect(x1, y1, x2 - x1, y2 - y1);
}


void MapCtrl::Accumulate(wxImage& image) const
{
	if (!image.IsOk())
		return;

	// Even tho GetData() returns a non-const pointer,
	// it does not unshare the reference counted data,
	// so we have to make a copy of the image..
	image = image.Copy();

	const int w = image.GetWidth();
	const int h = image.GetHeight();
	unsigned char* data = image.GetData();

	// sum in vertical direction
	unsigned char* p = data;
	for (int x = 0; x < w; ++x, p += 3) {
		WriteInt24(p, p[0] + p[1] + p[2]);
	}
	for (int y = 1; y < h; ++y) {
		const unsigned char* prev = data + 3 * ((y - 1) * w);
		unsigned char* curr = data + 3 * (y * w);
		for (int x = 0; x < w; ++x, prev += 3, curr += 3) {
			WriteInt24(curr, ReadInt24(prev) + curr[0] + curr[1] + curr[2]);
		}
	}

	// sum in horizontal direction
	for (int x = 1; x < w; ++x) {
		for (int y = 0; y < h; ++y) {
			p = data + 3 * (y * w + x);
			WriteInt24(p, ReadInt24(p) + ReadInt24(p - 3));
		}
	}
}


double MapCtrl::GetStartRectMetalFraction(int index) const
{
	BattleStartRect sr = m_battle->GetStartRect(index);
	if (!sr.IsOk())
		return 0.0;
	return GetStartRectMetalFraction(sr);
}


double MapCtrl::GetStartRectMetalFraction(const BattleStartRect& sr) const
{
	// todo: this really is *logic*, not rendering code, so it
	// should go in some other layer sometime (SpringUnitSync?).
	if (!m_metalmap_cumulative.IsOk())
		return 0.0;

	int x1, y1, x2, y2, w, h;

	w = m_metalmap_cumulative.GetWidth();
	h = m_metalmap_cumulative.GetHeight();
	x1 = std::max(0, std::min(w - 1, int((sr.left * w / 200.0) + 0.5)));
	y1 = std::max(0, std::min(h - 1, int((sr.top * h / 200.0) + 0.5)));
	x2 = std::max(0, std::min(w - 1, int((sr.right * w / 200.0) + 0.5)));
	y2 = std::max(0, std::min(h - 1, int((sr.bottom * h / 200.0) + 0.5)));

	const unsigned char* metalmap = m_metalmap_cumulative.GetData();
	const int lefttop = ReadInt24(&metalmap[3 * (y1 * w + x1)]);
	const int righttop = ReadInt24(&metalmap[3 * (y1 * w + x2)]);
	const int leftbot = ReadInt24(&metalmap[3 * (y2 * w + x1)]);
	const int rightbot = ReadInt24(&metalmap[3 * (y2 * w + x2)]);
	// in 2d cumulative distribution total is value at bottom right corner
	const int total = ReadInt24(&metalmap[3 * (w * h - 1)]);

	return (double)(lefttop + rightbot - righttop - leftbot) / total;
}


unsigned int MapCtrl::GetNewRectIndex() const
{
	ASSERT_LOGIC(m_battle, "getting a rectangle index not in a battle");
	return m_battle->GetNextFreeRectIdx();
}


BattleStartRect MapCtrl::GetBattleRect(int x1, int y1, int x2, int y2, int ally) const
{
	BattleStartRect br;
	wxRect mr = GetMinimapRect();

	br.ally = ally;
	br.left = int((200.0 * (x1 - mr.x) / mr.width) + 0.5);
	br.top = int((200.0 * (y1 - mr.y) / mr.height) + 0.5);
	br.right = int((200.0 * (x2 - mr.x) / mr.width) + 0.5);
	br.bottom = int((200.0 * (y2 - mr.y) / mr.height) + 0.5);
	br.exist = true;
	br.toadd = false;
	br.toresize = true;

	int max_sz = 200;

	if (br.left < 0)
		br.left = 0;
	if (br.left > max_sz)
		br.left = max_sz;
	if (br.top < 0)
		br.top = 0;
	if (br.top > max_sz)
		br.top = max_sz;
	if (br.right < 0)
		br.right = 0;
	if (br.right > max_sz)
		br.right = max_sz;
	if (br.bottom < 0)
		br.bottom = 0;
	if (br.bottom > max_sz)
		br.bottom = max_sz;

	return br;
}


void MapCtrl::SetMouseOverRect(int index)
{
	int oldindex = m_mover_rect;
	m_mover_rect = index;
	_SetCursor();

	if ((index != oldindex) || (m_rect_area != m_last_rect_area))
		UpdateMinimap();
	m_last_rect_area = m_rect_area;

	Refresh();
}


void MapCtrl::_SetCursor()
{
	if (!m_minimap) {
		if (m_rect_area != Main)
			SetCursor(wxCursor(wxCURSOR_HAND));
		else
			SetCursor(wxCursor(wxCURSOR_ARROW));
		return;
	}

	if (m_battle != 0) {
		const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
								     .getSingleValue("startpostype", LSL::Enum::EngineOption));
		if (longval != IBattle::ST_Choose) {
			SetCursor(wxCursor(wxCURSOR_ARROW));
			return;
		} else {
			if (!m_ro)
				SetCursor(wxCursor(wxCURSOR_CROSS));
			else
				SetCursor(wxCursor(wxCURSOR_ARROW));
			return;
		}
	}

	if (!m_ro) {
		if (m_mover_rect >= 0) {
			if (m_rect_area == UpLeft)
				SetCursor(wxCursor(wxCURSOR_SIZENWSE));
			else if (m_rect_area == UpRight)
				SetCursor(wxCursor(wxCURSOR_ARROW));
			else if (m_rect_area == DownLeft)
				SetCursor(wxCursor(wxCURSOR_SIZENESW));
			else if (m_rect_area == DownRight)
				SetCursor(wxCursor(wxCURSOR_SIZENWSE));
			else
				SetCursor(wxCursor(wxCURSOR_SIZING));
		} else if (m_mover_rect == -1) {
			SetCursor(wxCursor(wxCURSOR_CROSS));
		} else {
			SetCursor(wxCursor(wxCURSOR_ARROW));
		}
	} else {
		if (m_mover_rect >= 0)
			SetCursor(wxCursor(wxCURSOR_HAND));
		else
			SetCursor(wxCursor(wxCURSOR_ARROW));
	}
}


void MapCtrl::RelocateUsers()
{
	if (m_battle == 0)
		return;

	for (unsigned int i = 0; i < m_battle->GetNumUsers(); i++) {
		try {
			User& user = m_battle->GetUser(i);
			UserPosition& pos = user.BattleStatus().pos;
			pos = m_battle->GetFreePosition();
			if (user.BattleStatus().pos.x == -1)
				m_battle->KickPlayer(user);
		} catch (...) {
		}
	}
}


void MapCtrl::GetClosestStartPos(int fromx, int fromy, int& index, int& x, int& y, int& range)
{
	if (m_battle == 0)
		return;
	LSL::UnitsyncMap map = m_battle->LoadMap();

	int newrange;
	range = -1;
	index = -1;

	for (int i = 0; i < int(map.info.positions.size()); i++) {
		double dx = fromx - map.info.positions[i].x;
		double dy = fromy - map.info.positions[i].y;
		newrange = (int)sqrt(dx * dx + dy * dy);
		if ((range == -1) || (range > newrange)) {
			range = newrange;
			index = i;
		}
	}

	if (index != -1) {
		x = map.info.positions[index].x;
		y = map.info.positions[index].y;
	}
}


int MapCtrl::LoadMinimap()
{
	slLogDebugFunc("");
	if (m_battle == 0)
		return -1;
	if (m_minimap)
		return -1;
	if (!m_battle->MapExists(false))
		return -1;

	const std::string map = m_battle->GetHostMapName();

	try {
		int w, h;
		GetClientSize(&w, &h);
		if (w * h == 0) {
			m_mapname = "";
			m_lastsize = wxSize(-1, -1);
			return -2;
		}

		// start chain of asynchronous map image fetches
		// first minimap, then metalmap and heightmap
		m_async.GetMinimap(map, w, h);

		m_mapname = map;
		m_lastsize = wxSize(w, h);
	} catch (...) {
		FreeMinimap();
		return -3;
	}
	return 0;
}


void MapCtrl::FreeMinimap()
{
	delete m_minimap;
	m_minimap = 0;
	delete m_metalmap;
	m_metalmap = 0;
	delete m_heightmap;
	m_heightmap = 0;
	m_mapname = "";
}


void MapCtrl::UpdateMinimap()
{
	assert(wxThread::IsMain());
	_SetCursor();
	if (m_battle == nullptr)
		return;
	int w, h;
	GetClientSize(&w, &h);

	m_mutex.Lock();
	const bool just_resize = (m_lastsize != wxSize(-1, -1) && m_lastsize != wxSize(w, h));
	if ((m_mapname != m_battle->GetHostMapName()) || just_resize) {
		FreeMinimap();
		int loaded_ok = LoadMinimap();

		if (!just_resize && loaded_ok == 0) // if a new map is loaded, reset start positions
		{
			const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
										 .getSingleValue("startpostype", LSL::Enum::EngineOption));
			if (longval == IBattle::ST_Pick)
				RelocateUsers();
		}
	}
	m_mutex.Unlock();
	Refresh();
}


void MapCtrl::RequireImages()
{
	if (!m_start_ally)
		m_start_ally = new wxBitmap(start_ally_xpm);
	if (!m_start_enemy)
		m_start_enemy = new wxBitmap(start_enemy_xpm);
	if (!m_start_unused)
		m_start_unused = new wxBitmap(start_unused_xpm);
	if (!m_player_img)
		m_player_img = new wxBitmap(player_xpm);
	if (!m_bot_img)
		m_bot_img = new wxBitmap(bot_xpm);
}


void MapCtrl::DrawStartRect(wxDC& dc, int index, wxRect& sr, const wxColour& col, bool mouseover, int alphalevel, bool forceInsideMinimap)
{
	int x1 = sr.x;
	int y1 = sr.y;
	int x2 = sr.x + sr.width;
	int y2 = sr.y + sr.height;
	wxRect mr = GetMinimapRect();
	if (forceInsideMinimap) {
		x1 = std::max(mr.x, x1);
		y1 = std::max(mr.y, y1);
		x2 = std::min(mr.x + mr.width, x2);
		y2 = std::min(mr.y + mr.height, y2);
	}
	sr.x = x1;
	sr.y = y1;
	sr.width = x2 - x1;
	sr.height = y2 - y1;
	if (sr.width < 0) {
		sr.width = 0;
	}
	if (sr.height < 0) {
		sr.height = 0;
	}
	if (sr.width * sr.height <= 0)
		return;

	dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxTRANSPARENT));

	wxImage img(sr.width, sr.height);
	wxColour light;
	light.Set(((col.Red() + 100) > 200) ? 200 : (col.Red() + 100), ((col.Green() + 100) > 200) ? 200 : (col.Green() + 100), ((col.Blue() + 100) > 200) ? 200 : (col.Blue() + 100));
	img.SetRGB(wxRect(0, 0, sr.width, sr.height), light.Red(), light.Green(), light.Blue());
	img.InitAlpha();
	unsigned char* alpha = img.GetAlpha();
	for (int y = 0; y < sr.height; y++) {
		int a;
		if ((y % 3) == 0)
			a = alphalevel;
		else
			a = ((alphalevel - 40) < 0) ? 0 : (alphalevel - 40);
		for (int x = 0; x < sr.width; x++) {
			alpha[(y * sr.width) + x] = a;
		}
	}
	wxBitmap bmpimg(img);
	dc.DrawBitmap(bmpimg, sr.x, sr.y, false);

	/*  wxFont f( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL|wxFONTFLAG_ANTIALIASED, wxFONTWEIGHT_LIGHT );
      dc.SetFont( f );*/
	if (index != -1) {
		int twidth, theight, tx, ty;
		wxString strIndex = wxString::Format(_T("%d"), index + 1);
		dc.GetTextExtent(strIndex, &twidth, &theight);
		dc.SetTextForeground(col);
		tx = sr.x + sr.width / 2 - twidth / 2;
		ty = sr.y + sr.height / 2 - theight / 2 - 1;
		DrawOutlinedText(dc, strIndex, tx, ty, wxColour(50, 50, 50), *wxWHITE);
		//dc.DrawText( wxString::Format( _T("%d"), index+1), sr.x + sr.width / 2 - twidth / 2, sr.y + sr.height / 2 - theight / 2 - 1 );

		const double metal = GetStartRectMetalFraction(index);
		if (metal != 0.0) {
			wxString strMetal = wxString::Format(_("Metal: %.1f%%"),
							     std::isnan(metal) ? 0.0 : metal * 100.0);
			dc.GetTextExtent(strMetal, &twidth, &theight);
			// don't cramp it in rect, but only display it if it actually fits
			if (sr.height >= 6 * theight && sr.width > twidth) {
				tx = sr.x + sr.width / 2 - twidth / 2;
				ty = sr.y + sr.height / 2 + theight / 2 + theight - 1;
				DrawOutlinedText(dc, strMetal, tx, ty, wxColour(50, 50, 50), *wxWHITE);
			}
		}
	}

	dc.SetPen(wxPen(col));

	if (mouseover && !m_ro) {

		dc.DrawRectangle(sr.x + 1, sr.y + 1, sr.width - 2, sr.height - 2);
		dc.SetPen(wxPen(*wxBLACK));
		dc.DrawRectangle(sr.x, sr.y, sr.width, sr.height);

		dc.SetBrush(wxBrush(*wxBLACK, wxSOLID));
		dc.DrawRectangle(sr.x, sr.y, boxsize, boxsize);
		dc.DrawRectangle(sr.x + sr.width - boxsize, sr.y + sr.height - boxsize, boxsize, boxsize);
		//dc.DrawRectangle( sr.x + sr.width - boxsize, sr.y + 1, boxsize, boxsize );
		//dc.DrawRectangle( sr.x, sr.y + sr.height - boxsize, boxsize, boxsize );

		if (m_rect_area != UpRight)
			dc.DrawBitmap(*m_close_img, sr.x + sr.width - 16, sr.y + 2, true);
		else
			dc.DrawBitmap(*m_close_hi_img, sr.x + sr.width - 16, sr.y + 2, true);

	} else {
		dc.DrawRectangle(sr.x, sr.y, sr.width, sr.height);
	}
}


wxRect MapCtrl::GetRefreshRect() const
{
	int width, height;
	GetClientSize(&width, &height);
	return wxRect(5, height - 27, 72, 22);
}


wxRect MapCtrl::GetDownloadRect() const
{
	int width, height;
	GetClientSize(&width, &height);
	return wxRect(5, height - 52, 87, 22);
}


void MapCtrl::DrawBackground(wxDC& dc)
{
	int width, height;
	GetClientSize(&width, &height);

	dc.SetPen(wxPen(*wxLIGHT_GREY));
	dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));

	if (m_battle == 0) {
		dc.DrawRectangle(0, 0, width, height);
		return;
	}

	wxBitmap* img = 0;
	switch (m_current_infomap) {
		case IM_Minimap:
			img = m_minimap;
			break;
		case IM_Metalmap:
			img = m_metalmap;
			break;
		case IM_Heightmap:
			img = m_heightmap;
			break;
		default:
			ASSERT_LOGIC(false, "missing InfoMap IM_* enumeration constant in switch");
			break;
	}

	// Draw minimap.
	if (!img) {

		// Draw background.
		dc.DrawRectangle(0, 0, width, height);

		if (m_sp)
			return;

		if (!m_nfound_img)
			m_nfound_img = new wxBitmap(not_found_icon_xpm);
		if (!m_reload_img)
			m_reload_img = new wxBitmap(reload_map_xpm);
		if (!m_dl_img)
			m_dl_img = new wxBitmap(download_map_xpm);

		dc.DrawBitmap(*m_nfound_img, width / 2 - 32, height / 2 - 32, true);
		if ((m_rect_area == Refreshing) && (m_mdown_area != Refreshing))
			dc.DrawBitmap(*m_reload_img, 6, height - 28, true);
		else
			dc.DrawBitmap(*m_reload_img, 5, height - 27, true);
		if ((m_rect_area == Download) && (m_mdown_area != Download))
			dc.DrawBitmap(*m_dl_img, 6, height - 53, true);
		else
			dc.DrawBitmap(*m_dl_img, 5, height - 52, true);

		dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		if (m_rect_area == Refreshing)
			DrawOutlinedText(dc, _("Refresh"), 28, height - 25, wxColour(50, 50, 50), *wxWHITE);
		else
			DrawOutlinedText(dc, _("Refresh"), 28, height - 25, *wxWHITE, wxColour(50, 50, 50));
		if (m_rect_area == Download)
			DrawOutlinedText(dc, _("Download"), 28, height - 50, wxColour(50, 50, 50), *wxWHITE);
		else
			DrawOutlinedText(dc, _("Download"), 28, height - 50, *wxWHITE, wxColour(50, 50, 50));

		//Draw "%MAPNAME% not found!" label on minimap
		wxString noMapFoundString = wxString::Format("Map \"%s\" not found!", m_battle->GetHostMapName());
		const int APPROXIMATE_FONT_SIZE = 6;
		int labelOrigin = (width / 2) - ((noMapFoundString.Length() / 2) * APPROXIMATE_FONT_SIZE);
		//If string do not fits to minimap's canvas reduce font size and reformate string
		if (labelOrigin < 0) {
			dc.SetFont(wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

			const wxString strangeLabel[3] = {"Map", m_battle->GetHostMapName(), "not found!"};
			for (int labelIndex = 0; labelIndex < 3; labelIndex++) {
				labelOrigin = (width / 2) - ((strangeLabel[labelIndex].Length() / 2) * (APPROXIMATE_FONT_SIZE - 1));
				DrawOutlinedText(dc, strangeLabel[labelIndex], labelOrigin, 5 + (labelIndex * APPROXIMATE_FONT_SIZE * 2), wxColour(50, 50, 50), *wxWHITE);
			}
		} else {
			DrawOutlinedText(dc, noMapFoundString, labelOrigin, 10, wxColour(50, 50, 50), *wxWHITE);
		}
	} else {

		wxRect r = GetMinimapRect();

		// Draw background where the minimap is not drawn(to avoid flickering).
		if (r.y > 0) {
			dc.DrawRectangle(0, 0, width, (height - r.height) / 2);
			dc.DrawRectangle(0, r.y + r.height, width, (height - r.height) / 2 + 1);
		}
		if (r.x > 0) {
			dc.DrawRectangle(0, 0, (width - r.width) / 2, height);
			dc.DrawRectangle(r.x + r.width, 0, (width - r.width) / 2 + 1, height);
		}

		// Draw minimap
		dc.DrawBitmap(*img, r.x, r.y, false);
	}
}


void MapCtrl::DrawStartRects(wxDC& dc)
{
	for (int i = 0; i <= int(m_battle->GetLastRectIdx()); i++) {
		wxRect sr = GetStartRect(i);
		if (sr.IsEmpty())
			continue;
		wxColour col;
		if (i == m_battle->GetMe().BattleStatus().ally) {
			col.Set(0, 200, 0);
		} else {
			col.Set(200, 0, 0);
		}
		DrawStartRect(dc, i, sr, col, m_mover_rect == i);
	}
}


void MapCtrl::DrawStartPositions(wxDC& dc)
{
	wxRect mr = GetMinimapRect();
	m_map = m_battle->LoadMap();
	RequireImages();
	const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
							     .getSingleValue("startpostype", LSL::Enum::EngineOption));
	if (longval == IBattle::ST_Fixed) {

		wxFont f(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
		dc.SetFont(f);
		for (int i = 0; i < int(m_map.info.positions.size()); i++) {
			int x = (int)((double)((double)m_map.info.positions[i].x / (double)m_map.info.width) * (double)mr.width) - 8;
			int y = (int)((double)(m_map.info.positions[i].y / (double)m_map.info.height) * (double)mr.height) - 8;
			dc.DrawBitmap(*m_start_ally, x + mr.x, y + mr.y, true);
			wxCoord w, h;
			dc.GetTextExtent(wxString::Format(_T("%d"), i + 1), &w, &h);
			dc.DrawText(wxString::Format(_T("%d"), i + 1), x + mr.x + (8 - w / 2), y + mr.y + (8 - h / 2));
		}
	} else {
		// Draw startpositions
		for (int i = 0; i < int(m_map.info.positions.size()); i++) {
			int x = (int)((double)((double)m_map.info.positions[i].x / (double)m_map.info.width) * (double)mr.width) - 8;
			int y = (int)((double)(m_map.info.positions[i].y / (double)m_map.info.height) * (double)mr.height) - 8;
			dc.DrawBitmap(*m_start_unused, x + mr.x, y + mr.y, true);
		}
	}
}

wxRealPoint
MapCtrl::GetUserMapPositionAsReal(const User& user) const
{
	return wxRealPoint(static_cast<double>(user.BattleStatus().pos.x) / static_cast<double>(m_map.info.width),
			   static_cast<double>(user.BattleStatus().pos.y) / static_cast<double>(m_map.info.height));
}

wxPoint
MapCtrl::GetTranslatedScaledUserMapPosition(const User& user) const
{
	wxRealPoint rmp(GetUserMapPositionAsReal(user));
	wxRect mr(GetMinimapRect());

	return wxPoint(mr.x + static_cast<int>(rmp.x * mr.width),
		       mr.y + static_cast<int>(rmp.y * mr.height));
}

/** Try to fit @p what inside @p container.
 *
 * @return a wxPoint that can be used with wxRect::Offset.
 */
inline wxPoint
FitInside(const wxRect& what, const wxRect& container)
{
	ASSERT_LOGIC(what.width <= container.width && what.height <= container.height,
		     "Can't fit rect inside target container");

	wxPoint offset(0, 0);

	if (what.x < container.x)
		offset.x += container.x - what.x;
	else if (what.GetRight() > container.GetRight())
		offset.x += container.GetRight() - what.GetRight();

	if (what.y < container.y)
		offset.y += container.y - what.y;
	else if (what.GetBottom() > container.GetBottom())
		offset.y += container.GetBottom() - what.GetBottom();

	return offset;
}

wxRect MapCtrl::GetUserRect(const User& user, bool selected)
{
	ASSERT_LOGIC(m_battle != 0, "Bot == 0");
	m_map = m_battle->LoadMap();

	wxPoint absolute_position(GetTranslatedScaledUserMapPosition(user));
	wxPoint box_start(absolute_position.x - USER_BOX_ICON_HALFWIDTH - USER_BOX_ICON_PADDING,
			  absolute_position.y - USER_BOX_ICON_HALFWIDTH - USER_BOX_ICON_PADDING);
	wxRect user_box(box_start, (selected && m_sp) ? user_box_expanded_size : user_box_icon_size);
	wxRect cram_into_box(GetDrawableRect());
	wxPoint offset(::FitInside(user_box, cram_into_box));
	user_box.Offset(offset);

	return user_box;
}


MapCtrl::RectangleArea MapCtrl::GetUserRectArea(const wxRect& userrect, int x, int y) const
{
	x = x - userrect.x;
	y = y - userrect.y;
	wxRect close = GetUserCloseRect();

	if (close.Contains(x, y))
		return Close;
	wxRect side = GetUserSideRect();
	if (side.Contains(x, y))
		return Side;
	wxRect AllyUp = GetUserUpAllyButtonRect();
	if (AllyUp.Contains(x, y))
		return UpAllyButton;
	wxRect AllyDown = GetUserDownAllyButtonRect();
	if (AllyDown.Contains(x, y))
		return DownAllyButton;
	wxRect handicap = GetUserHandicapRect();
	if (handicap.Contains(x, y))
		return Handicap;
	wxRect HandicapUp = GetUserUpHandicapButtonRect();
	if (HandicapUp.Contains(x, y))
		return UpHandicapButton;
	wxRect HandicapDown = GetUserDownHandicapButtonRect();
	if (HandicapDown.Contains(x, y))
		return DownHandicapButton;

	wxRect bot(0, 0, USER_BOX_ICON_WIDTH, USER_BOX_ICON_HEIGHT);
	if (bot.Contains(x, y))
		return Move;

	return Main;
}


void MapCtrl::DrawOutlinedText(wxDC& dc, const wxString& str, int x, int y, const wxColour& outline, const wxColour& font) const
{
	dc.SetTextForeground(outline);
	dc.DrawText(str, x, y);
	dc.DrawText(str, x, y + 1);
	dc.DrawText(str, x + 1, y);
	dc.DrawText(str, x + 2, y);
	dc.DrawText(str, x, y + 2); // Hack to get font outline
	dc.DrawText(str, x + 2, y + 2);
	dc.SetTextForeground(font);
	dc.DrawText(str, x + 1, y + 1);
}

void MapCtrl::DrawUser(wxDC& dc, User& user, bool selected, bool /*unused*/)
{
	wxBitmap* img;
	if (!user.BattleStatus().IsBot())
		img = m_player_img;
	else
		img = m_bot_img;

	wxFont f(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
	dc.SetFont(f);
	dc.SetTextForeground(*wxWHITE);

	wxRect r = GetUserRect(user, selected);
	wxColour col = lslTowxColour(user.BattleStatus().colour);

	if (selected) {
		/* Drawing the whole user-battle-settings-box deal */
		DrawStartRect(dc, -1, r, ColourDelta(col, -40), false, 180, false); /* box background */

		/* Side selector */
		wxRect siderect = GetUserSideRect();
		DrawOutlinedText(dc, _("side:"), r.x + 3, r.y + siderect.y - 1, wxColour(50, 50, 50), *wxWHITE);

		if (m_rect_area == Side) {
			wxRect tmp(r.x + siderect.x - 1, r.y + siderect.y - 1, siderect.width + 2, siderect.height + 2);
			DrawStartRect(dc, -1, tmp, col, false);
		}

		wxBitmap bmp = icons().GetBitmap(icons().GetSideIcon(m_battle->GetHostGameName(), user.BattleStatus().side));
		dc.DrawBitmap(bmp, r.x + siderect.x, r.y + siderect.y, true);

		/* Draw the Ally Number numeric select */
		wxRect updownallyrect = GetUserUpAllyButtonRect();
		DrawOutlinedText(dc, wxString::Format(_("ally:   %d"), user.BattleStatus().ally + 1), r.x + 3, r.y + updownallyrect.y, wxColour(50, 50, 50), *wxWHITE);
		//dc.DrawText( wxString::Format( _("ally: %d"), bot.BattleStatus().ally + 1 ), r.x+4, r.y+40 );

		if (m_rect_area == UpAllyButton)
			dc.DrawBitmap(wxBitmap(upsel_down_xpm), r.x + updownallyrect.x, r.y + updownallyrect.y, true);
		else if (m_rect_area == DownAllyButton)
			dc.DrawBitmap(wxBitmap(up_downsel_xpm), r.x + updownallyrect.x, r.y + updownallyrect.y, true);
		else
			dc.DrawBitmap(wxBitmap(up_down_xpm), r.x + updownallyrect.x, r.y + updownallyrect.y, true);

		/* Draw the Handicap numeric select */
		wxRect updownhandicaprect = GetUserUpHandicapButtonRect();
		wxFont b(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
		dc.SetFont(b);
		DrawOutlinedText(dc, wxString::Format(_("bonus: %d%%"), user.BattleStatus().handicap), r.x + 3, r.y + updownhandicaprect.y + 2, wxColour(50, 50, 50), *wxWHITE);
		dc.SetFont(f);

		if (m_rect_area == UpHandicapButton)
			dc.DrawBitmap(wxBitmap(upsel_down_xpm), r.x + updownhandicaprect.x, r.y + updownhandicaprect.y, true);
		else if (m_rect_area == DownHandicapButton)
			dc.DrawBitmap(wxBitmap(up_downsel_xpm), r.x + updownhandicaprect.x, r.y + updownhandicaprect.y, true);
		else
			dc.DrawBitmap(wxBitmap(up_down_xpm), r.x + updownhandicaprect.x, r.y + updownhandicaprect.y, true);

		dc.SetPen(wxPen(col));
		dc.SetBrush(wxBrush(col, wxSOLID));

		dc.DrawRectangle(r.x + 1, r.y + 1, r.width - 2, 18);

		/* Draw the little 'X' (close button) in the corner... */
		if (&user != &(m_battle->GetMe())) {
			wxRect closerect = GetUserCloseRect();
			if (m_rect_area == Close)
				dc.DrawBitmap(*m_close_hi_img, r.x + closerect.x, r.y + closerect.y, true);
			else
				dc.DrawBitmap(*m_close_img, r.x + closerect.x, r.y + closerect.y, true);
		}
		dc.DrawBitmap(*img, r.x + 2, r.y + 2, true);
	} else {
		/* Just drawing an icon */
		dc.SetPen(wxPen(ColourDelta(col, -40)));
		dc.SetBrush(wxBrush(col, wxSOLID));
		dc.DrawRectangle(r.x, r.y, r.width, r.height);
		dc.DrawBitmap(*img, r.x + USER_BOX_ICON_PADDING, r.y + USER_BOX_ICON_PADDING, true);

		int w, h;
		wxString allystr = wxString::Format(_T("%d"), user.BattleStatus().ally + 1);
		dc.GetTextExtent(allystr, &w, &h);

		DrawOutlinedText(dc, allystr, r.width - w - 3 + r.x, r.height - h - 1 + r.y, wxColour(50, 50, 50), *wxWHITE);
	}
}


void MapCtrl::DrawUserPositions(wxDC& dc)
{
	slLogDebugFunc("");
	if (m_battle == 0)
		return;
	if (!m_battle->MapExists(false))
		return;

	wxRect mr = GetMinimapRect();
	m_map = m_battle->LoadMap();
	RequireImages();

	for (int i = 0; i < int(m_map.info.positions.size()); i++) {

		int x = (int)((double)((double)m_map.info.positions[i].x / (double)m_map.info.width) * (double)mr.width) - 8;
		int y = (int)((double)(m_map.info.positions[i].y / (double)m_map.info.height) * (double)mr.height) - 8;

		User* bot = 0;
		for (unsigned int bi = 0; bi < m_battle->GetNumUsers(); bi++) {
			User& tbot = m_battle->GetUser(bi);
			if (&tbot == 0)
				continue;
			if ((tbot.BattleStatus().pos.x == m_map.info.positions[i].x) && (tbot.BattleStatus().pos.y == m_map.info.positions[i].y)) {
				bot = &tbot;
				break;
			}
		}

		if (bot != 0)
			continue;

		dc.DrawBitmap(*m_start_ally, x + mr.x, y + mr.y, true);
	}
	int previousteam = -1;
	for (unsigned int i = 0; i < m_battle->GetNumUsers(); i++) {
		User& usr = m_battle->GetUser(i);
		if (&usr == 0)
			continue;
		if (usr.BattleStatus().spectator)
			continue;
		int currentteam = usr.BattleStatus().team;
		if (currentteam == previousteam)
			continue;
		else
			previousteam = currentteam;

		bool expanded = m_user_expanded;
		if (!m_sp)
			expanded = false;
		DrawUser(dc, usr, (m_maction != Moved) && expanded, (m_maction == Moved) && expanded);
	}
}


void MapCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	slLogDebugFunc("");
	wxPaintDC dc(this);

	DrawBackground(dc);

	if (m_battle == 0)
		return;

	if (!m_minimap)
		return;
	const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
							     .getSingleValue("startpostype", LSL::Enum::EngineOption));


	if (m_draw_start_types) {
		if (longval == IBattle::ST_Choose) {
			DrawStartRects(dc);
			// Draw add rect.
			if (m_tmp_brect.ally != -1) {
				wxRect tmp = GetStartRect(m_tmp_brect);
				DrawStartRect(dc, m_tmp_brect.ally, tmp, *wxWHITE, false);
			}
		} else if (longval == IBattle::ST_Pick) {
			DrawUserPositions(dc);
		} else {
			DrawStartPositions(dc);
		}
	}
}


void MapCtrl::OnResize(wxSizeEvent& /*unused*/)
{
	UpdateMinimap();
}

void MapCtrl::OnMouseEnter(wxMouseEvent& /*event*/)
{
	// workaround: scroll events only if element is focused on MSW
	// workaround2: only focus current if window is active
	if (((wxTopLevelWindow*)wxTheApp->GetTopWindow())->IsActive())
		SetFocus();
}

void MapCtrl::OnMouseMove(wxMouseEvent& event)
{
	wxPoint p = event.GetPosition();
	if (m_battle == 0)
		return;
	if (p == wxDefaultPosition)
		return;
	const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
							     .getSingleValue("startpostype", LSL::Enum::EngineOption));

	if (longval == IBattle::ST_Pick) {

		if (!m_battle->MapExists(false))
			return;
		if (m_maction == Moved) {
			User& user = *m_user_expanded;
			try {
				ASSERT_LOGIC(&user != 0, "MapCtrl::OnMouseMove(): user = 0");
			} catch (...) {
				return;
			}
			m_map = m_battle->LoadMap();

			wxRect mr = GetMinimapRect();
			wxRect r = GetUserRect(user, false);

			user.BattleStatus().pos.x = (int)(((double)(event.GetX() - mr.x) / (double)mr.width) * (double)m_map.info.width);
			user.BattleStatus().pos.y = (int)(((double)(event.GetY() - mr.y) / (double)mr.height) * (double)m_map.info.height);
			user.BattleStatus().pos.x = LSL::Util::Clamp(user.BattleStatus().pos.x, 0, m_map.info.width);
			user.BattleStatus().pos.y = LSL::Util::Clamp(user.BattleStatus().pos.y, 0, m_map.info.height);

			int x, y, index, range;
			GetClosestStartPos(user.BattleStatus().pos.x, user.BattleStatus().pos.y, index, x, y, range);
			if (index != -1) {
				if (range < (10.0 / (double)mr.width) * (double)m_map.info.width) {
					user.BattleStatus().pos.x = x;
					user.BattleStatus().pos.y = y;
				}
			}
			RefreshRect(r.Union(GetUserRect(user, false)), false);
			return;
		}

		if (!m_user_expanded || !m_battle->UserExists(m_user_expanded->GetNick()))
			m_user_expanded = 0;

		if (m_user_expanded) {
			User& user = *m_user_expanded;
			try {
				ASSERT_LOGIC(&user != 0, "MapCtrl::OnMouseMove(): user = 0");
			} catch (...) {
				return;
			}
			wxRect r = GetUserRect(user, true);
			if (r.Contains(event.GetX(), event.GetY())) {
				RectangleArea last = m_rect_area;
				m_rect_area = GetUserRectArea(r, event.GetX(), event.GetY());
				if (last != m_rect_area)
					RefreshRect(r, false);
			} else {
				m_user_expanded = 0;
				RefreshRect(r, false);
			}
		} else {
			for (unsigned int i = 0; i < m_battle->GetNumUsers(); i++) {
				User& user = m_battle->GetUser(i);
				if (&user == 0)
					continue;
				wxRect r = GetUserRect(user, false);
				if (r.Contains(event.GetX(), event.GetY())) {
					m_rect_area = GetUserRectArea(r, event.GetX(), event.GetY());
					m_user_expanded = &user;
					RefreshRect(GetUserRect(user, true), false);
					break;
				}
			}
		}
		return;
	}

	if (!m_minimap) {
		wxRect r = GetRefreshRect();
		wxRect d = GetDownloadRect();
		RectangleArea old = m_rect_area;
		if (r.Contains(event.GetX(), event.GetY())) {
			m_rect_area = Refreshing;
		} else if (d.Contains(event.GetX(), event.GetY())) {
			m_rect_area = Download;
		} else {
			m_rect_area = Main;
		}
		_SetCursor();
		if (m_rect_area != old) {
			Refresh();
		}
		return;
	}

	if (!m_draw_start_types)
		return;

	if (longval != IBattle::ST_Choose)
		return;

	if (m_maction == Add) { // We are currently adding a rect.

		wxRect oldr = GetStartRect(m_tmp_brect);

		m_tmp_brect = GetBattleRect(m_mdown_x < event.GetX() ? m_mdown_x : event.GetX(),
					    m_mdown_y < event.GetY() ? m_mdown_y : event.GetY(),
					    m_mdown_x > event.GetX() ? m_mdown_x : event.GetX(),
					    m_mdown_y > event.GetY() ? m_mdown_y : event.GetY(),
					    m_tmp_brect.ally);

		wxRect newr = GetStartRect(m_tmp_brect);
		if (newr != oldr)
			RefreshRect(newr.Union(oldr), false);
		return;

	} else if (m_maction == ResizeDownRight) {

		wxRect sr = GetStartRect(m_mdown_rect);

		wxRect nsr(sr.x, sr.y, event.GetX() - sr.x, event.GetY() - sr.y);
		if (nsr.width < minboxsize)
			nsr.SetWidth(minboxsize);
		if (nsr.height < minboxsize)
			nsr.SetHeight(minboxsize);
		BattleStartRect bsr = GetBattleRect(nsr.x, nsr.y, nsr.x + nsr.width, nsr.y + nsr.height, m_mdown_rect);
		m_battle->AddStartRect(m_mdown_rect, bsr.left, bsr.top, bsr.right, bsr.bottom);
		m_battle->StartRectAdded(m_mdown_rect);
		m_battle->ResizeStartRect(m_mdown_rect);

		// ugly fix.
		nsr = GetStartRect(bsr);

		if (sr != nsr)
			RefreshRect(sr.Union(nsr), false);
		return;

	} else if (m_maction == ResizeUpLeft) {

		wxRect sr = GetStartRect(m_mdown_rect);

		wxRect nsr(event.GetX(), event.GetY(), (sr.x - event.GetX()) + sr.width, (sr.y - event.GetY()) + sr.height);
		if (nsr.width < minboxsize) {
			nsr.SetLeft(event.GetX() - minboxsize + nsr.width);
			nsr.SetWidth(minboxsize);
		}
		if (nsr.height < minboxsize) {
			nsr.SetTop(event.GetY() - minboxsize + nsr.height);
			nsr.SetHeight(minboxsize);
		}
		BattleStartRect bsr = GetBattleRect(nsr.x, nsr.y, nsr.x + nsr.width, nsr.y + nsr.height, m_mdown_rect);
		m_battle->AddStartRect(m_mdown_rect, bsr.left, bsr.top, bsr.right, bsr.bottom);
		m_battle->StartRectAdded(m_mdown_rect);
		m_battle->ResizeStartRect(m_mdown_rect);

		// ugly fix.
		nsr = GetStartRect(bsr);

		if (sr != nsr)
			RefreshRect(sr.Union(nsr), false);
		return;
	}

	// Make sure point is inside minimap
	if (GetMinimapRect().Contains(p)) {

		// Check if point is in a startrect.
		for (int i = m_battle->GetLastRectIdx(); i >= 0; i--) {

			wxRect r = GetStartRect(i);
			if (r.IsEmpty())
				continue;

			if (r.Contains(p)) {

				if (!m_ro) {
					if ((wxRect(r.x + r.width - m_close_img->GetWidth(), r.y + 1, m_close_img->GetWidth(), m_close_img->GetWidth())).Contains(p))
						m_rect_area = UpRight;
					else if ((wxRect(r.x, r.y, boxsize, boxsize)).Contains(p))
						m_rect_area = UpLeft;
					else if ((wxRect(r.x + r.width - boxsize, r.y + r.height - boxsize, boxsize, boxsize)).Contains(p))
						m_rect_area = DownRight;
					//else if ( (wxRect( r.x, r.y + r.height - boxsize, boxsize, boxsize )).Contains( p ) ) m_rect_area = DownLeft;
					else
						m_rect_area = Main;
				}
				SetMouseOverRect(i);

				return;
			}
		}
		if (m_mover_rect != -1)
			SetMouseOverRect(-1);

	} else {
		if (m_mover_rect != -2)
			SetMouseOverRect(-2);
	}
}


void MapCtrl::OnLeftDown(wxMouseEvent& event)
{
	if (m_battle == 0)
		return;

	const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
							     .getSingleValue("startpostype", LSL::Enum::EngineOption));

	if (!m_ro) {
		if (longval == IBattle::ST_Pick) {
			if (m_user_expanded)
				m_mdown_area = m_rect_area;
			else
				return;
			if (m_mdown_area == Move)
				m_maction = Moved;
			else
				m_maction = None;
			User& user = *m_user_expanded;
			try {
				ASSERT_LOGIC(&user != 0, "MapCtrl::OnLeftDown(): user = 0");
			} catch (...) {
				return;
			}
			RefreshRect(GetUserRect(user, true), false);
			return;
		}
	}

	if (!m_minimap) {
		if (m_rect_area != Main) {
			m_mdown_area = m_rect_area;
			Refresh();
		}
		return;
	}

	if (!m_draw_start_types)
		return;

	if (longval != IBattle::ST_Choose)
		return;
	if (!m_ro) {
		// In edit mode
		if (m_mover_rect >= 0) { // We are over an existing rect.

			m_mdown_area = m_rect_area;
			m_mdown_rect = m_mover_rect;
			m_mdown_x = event.GetX();
			m_mdown_y = event.GetY();

			if (m_mdown_area == UpLeft) {
				m_maction = ResizeUpLeft;
			} else if (m_mdown_area == UpRight) {
				m_maction = Delete;
			} else if (m_mdown_area == DownLeft) {
				//        m_maction = ResizeDownLeft;
			} else if (m_mdown_area == DownRight) {
				m_maction = ResizeDownRight;
			} else if (m_mdown_area == Main) {
				m_maction = Moved;
			}

		} else if (m_mover_rect == -1) { // We are over empty minimap area.

			m_maction = Add; // Add new start rect.
			m_mdown_x = event.GetX();
			m_mdown_y = event.GetY();
			m_tmp_brect.ally = GetNewRectIndex();

		} else if (m_mover_rect == -2) { // We are outside the minimap.
		}
	} else {
		// Readonly.
		if (m_mover_rect >= 0) {
			// Join ally rect that user clicked on
			m_battle->GetMe().BattleStatus().ally = m_mover_rect;
		}
		m_maction = None;
	}
}


void MapCtrl::OnLeftUp(wxMouseEvent& event)
{
	if (m_battle == 0)
		return;

	const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
							     .getSingleValue("startpostype", LSL::Enum::EngineOption));
	if (longval == IBattle::ST_Pick) {
		if (!m_user_expanded)
			return;
		if (m_rect_area != m_mdown_area)
			return;
		User& user = *m_user_expanded;
		try {
			ASSERT_LOGIC(&user != 0, "MapCtrl::OnLeftUp(): user == 0");
		} catch (...) {
			return;
		}
		if ((m_mdown_area == Move) && (m_maction == Moved)) {
			m_battle->UserPositionChanged(user);
		} else if (m_mdown_area == UpAllyButton) {
			user.BattleStatus().ally = (user.BattleStatus().ally + 1) % SPRING_MAX_ALLIES;
			RefreshRect(GetUserRect(user, true), false);

		} else if (m_mdown_area == DownAllyButton) {
			user.BattleStatus().ally = (user.BattleStatus().ally - 1) >= 0 ? (user.BattleStatus().ally - 1) : (SPRING_MAX_ALLIES - 1);
			RefreshRect(GetUserRect(user, true), false);

		} else if (m_mdown_area == UpHandicapButton) {
			if (user.BattleStatus().handicap == 100)
				user.BattleStatus().handicap = 0;
			else
				user.BattleStatus().handicap = (user.BattleStatus().handicap + 5);
			RefreshRect(GetUserRect(user, true), false);

		} else if (m_mdown_area == DownHandicapButton) {
			user.BattleStatus().handicap = (user.BattleStatus().handicap - 5) >= 0 ? (user.BattleStatus().handicap - 5) : 100;
			RefreshRect(GetUserRect(user, true), false);

		} else if (m_mdown_area == Side) {
			try {
				const auto sides = LSL::usync().GetSides(m_battle->GetHostGameName());
				const unsigned int sidecount = sides.size();
				if (sidecount > 0)
					user.BattleStatus().side = (user.BattleStatus().side + 1) % sidecount;
				else
					user.BattleStatus().side = 0;
			} catch (...) {
			}
			RefreshRect(GetUserRect(user, true), false);

		} else if (m_mdown_area == Close && m_user_expanded != &m_battle->GetMe()) {
			wxRect r = GetUserRect(user, true);
			m_battle->KickPlayer(user);
			RefreshRect(r, false);
			m_user_expanded = 0;
		}
		m_mdown_area = Main;
		m_maction = None;
		return;
	}

	if (!m_minimap) {
		if (m_mdown_area == m_rect_area) {
			if (m_mdown_area == Refreshing) {
				//				LSL::usync().AddReloadEvent();
				m_battle->Update(stdprintf("%d_mapname", LSL::Enum::PrivateOptions));
				UpdateMinimap();
			} else if (m_mdown_area == Download) {
				ContentDownloadRequest req;
				req.MapRequired(m_battle->GetHostMapName(), m_battle->GetHostMapHash());
				try{
					ContentManager::Instance()->DownloadContent(req);
				}catch(Exception& e) {
					wxLogError(_("Failed to download map: ") + e.Reason());
				}
			}
		}
		m_mdown_area = Main;
		Refresh();
		return;
	}

	if (!m_draw_start_types)
		return;

	if (longval != IBattle::ST_Choose)
		return;

	if (m_maction == Add) {

		m_tmp_brect.ally = -1;
		if ((abs(m_mdown_x - event.GetX()) >= minboxsize) && (abs(m_mdown_y - event.GetY()) >= minboxsize)) {
			BattleStartRect r = GetBattleRect(m_mdown_x < event.GetX() ? m_mdown_x : event.GetX(), m_mdown_y < event.GetY() ? m_mdown_y : event.GetY(), m_mdown_x > event.GetX() ? m_mdown_x : event.GetX(), m_mdown_y > event.GetY() ? m_mdown_y : event.GetY());
			m_battle->AddStartRect(GetNewRectIndex(), r.left, r.top, r.right, r.bottom);
		} else {
			BattleStartRect r = GetBattleRect(m_mdown_x, m_mdown_y, m_mdown_x + minboxsize, m_mdown_y + minboxsize);
			m_battle->AddStartRect(GetNewRectIndex(), r.left, r.top, r.right, r.bottom);
		}
		m_battle->SendHostInfo(IBattle::HI_StartRects);
		UpdateMinimap();
	} else if (m_maction == Delete) {

		if ((m_mdown_area == m_rect_area) && (m_mover_rect == m_mdown_rect)) {
			m_battle->RemoveStartRect(m_mdown_rect);
			UpdateMinimap();
			m_battle->SendHostInfo(IBattle::HI_StartRects);
		}

	} else if ((m_maction == ResizeDownRight) || (m_maction == ResizeUpLeft)) {
		m_battle->ResizeStartRect(m_mdown_rect);
		m_battle->SendHostInfo(IBattle::HI_StartRects);
	}

	m_maction = None;
}
void MapCtrl::OnRightUp(wxMouseEvent& event)
{
	wxPoint p = event.GetPosition();
	if (m_battle == 0)
		return;
	if (p == wxDefaultPosition)
		return;

	const long longval = LSL::Util::FromIntString(m_battle->CustomBattleOptions()
							     .getSingleValue("startpostype", LSL::Enum::EngineOption));
	if (longval == IBattle::ST_Pick) {

		if (!m_battle->MapExists(false))
			return;

		if (m_maction != Moved) {
			m_map = m_battle->LoadMap();
			wxRect mr = GetMinimapRect();

			int mclx = (int)(((double)(event.GetX() - mr.x) / (double)mr.width) * (double)m_map.info.width);
			int mcly = (int)(((double)(event.GetY() - mr.y) / (double)mr.height) * (double)m_map.info.height);
			mclx = LSL::Util::Clamp(mclx, 0, m_map.info.width);
			mcly = LSL::Util::Clamp(mcly, 0, m_map.info.height);

			int x, y, index = -1, range;
			GetClosestStartPos(mclx, mcly, index, x, y, range);
			if (index != -1) {
				if (range < (10.0 / (double)mr.width) * (double)m_map.info.width) {
					AddBotDialog dlg(this, m_battle[0], true);
					if (dlg.ShowModal() == wxID_OK) {
						UserBattleStatus bs;
						bs.owner = m_battle->GetMe().GetNick();
						bs.aishortname = STD_STRING(dlg.GetAIShortName());
						bs.airawname = STD_STRING(dlg.GetAiRawName());
						bs.aiversion = STD_STRING(dlg.GetAIVersion());
						bs.aitype = dlg.GetAIType();
						bs.team = m_battle->GetFreeTeam();
						bs.ally = m_battle->GetFreeAlly();
						bs.colour = m_battle->GetNewColour();
						User& bot = m_battle->OnBotAdded(STD_STRING(dlg.GetNick()), bs);
						ASSERT_LOGIC(&bot != 0, "bot == 0");
						bot.BattleStatus().pos.x = x;
						bot.BattleStatus().pos.y = y;
						RefreshRect(GetUserRect(bot, false), false);
					}
				}
			}
			return;
		}
	}
}

void MapCtrl::OnMouseWheel(wxMouseEvent& event)
{
	if (m_metalmap) {
		int idx = (int)m_current_infomap;
		if (event.m_wheelRotation > 0) {
			++idx;
			if (idx >= IM_Count)
				idx = IM_Minimap;
		} else {
			--idx;
			if (idx < IM_Minimap)
				idx = IM_Count - 1;
		}
		m_current_infomap = (InfoMap)idx;
		Refresh();
	}
}


void MapCtrl::OnGetMapImageAsyncCompleted(const std::string& mapname)
{
	wxLogDebug(wxString::Format(_T("Map loaded (async) complete: %s"), TowxString(mapname).c_str()));
	m_mutex.Lock();

	if (m_mapname.empty() || mapname.empty() || mapname != m_mapname) {
		m_mutex.Unlock();
		return;
	}

	const int w = m_lastsize.GetWidth();
	const int h = m_lastsize.GetHeight();

	if (m_minimap == NULL) {
		m_minimap = new wxBitmap(LSL::usync().GetMinimap(m_mapname, w, h).wxbitmap());
		// this ensures metalmap and heightmap aren't loaded in battlelist
		if (m_draw_start_types) {
			m_async.GetMetalmap(m_mapname, w, h);
		}
	} else if (m_metalmap == NULL) {
		m_metalmap = new wxBitmap(LSL::usync().GetMetalmap(m_mapname, w, h).wxbitmap());
		// singleplayer mode doesn't allow startboxes anyway
		m_metalmap_cumulative = LSL::usync().GetMetalmap(m_mapname, w, h).wximage();
		Accumulate(m_metalmap_cumulative);
		m_async.GetHeightmap(m_mapname, w, h);
	} else if (m_heightmap == NULL) {
		m_heightmap = new wxBitmap(LSL::usync().GetHeightmap(m_mapname, w, h).wxbitmap());
	}

	// never ever call a gui function here, it will crash! (in 1/100 cases)
	wxCommandEvent evt(REFRESH_EVENT, GetId());
	evt.SetEventObject(this);
	wxPostEvent(this, evt);

	m_mutex.Unlock();
}

void MapCtrl::OnRefresh(wxCommandEvent& /*event*/)
{
	assert(wxThread::IsMain());
	Refresh();
}
