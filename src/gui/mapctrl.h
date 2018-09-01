/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPCTRL_H

#include <lslunitsync/unitsync.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/thread.h>
#include "ibattle.h"
class wxPanel;
class wxBitmap;
class wxDC;

struct BattleStartRect;

namespace LSL
{
struct UnitsyncMap;
}

class MapCtrl : public wxPanel
{

	enum RectangleArea {
		Main = -1,
		UpLeft,
		UpRight,
		DownRight,
		DownLeft,
		UpAllyButton,
		DownAllyButton,
		Side,
		UpHandicapButton,
		DownHandicapButton,
		Handicap,
		Close,
		Move,
		Download,
		Refreshing
	};

	enum MouseAction {
		None,
		Add,
		Delete,
		Moved,
		ResizeUpLeft,
		ResizeUpRight,
		ResizeDownLeft,
		ResizeDownRight
	};

	enum UserRectOrientation {
		TopLeft,
		BottomLeft,
		TopRight,
		BottomRight
	};


public:
	MapCtrl(wxWindow* parent, int size, IBattle* battle, bool readonly, bool draw_start_types, bool singleplayer);
	~MapCtrl();

	void SetBattle(IBattle* battle);

	void UpdateMinimap();

	void OnPaint(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);

	void OnMouseEnter(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
	void OnRightUp(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);

	void OnGetMapImageAsyncCompleted(const std::string& mapname);

	void OnRefresh(wxCommandEvent& event);

	void SetReadOnly(bool readonly)
	{
		m_ro = readonly;
	}


private:
	int LoadMinimap();
	void FreeMinimap();

	BattleStartRect GetBattleRect(int x1, int y1, int x2, int y2, int ally = -1) const;

	/** Get the rect occupied by the minimap.
     *
     * @see GetDrawableRect
     */
	wxRect GetMinimapRect() const;

	/** Get the widget-drawable area as a wxRect.  This is similar to
     * GetMinimapRect, but includes the area not filled by the minimap.
     *
     * @return The wxRect corresponding {0, 0, ClientWidth, ClientHeight}.
     *
     * @see GetMinimapRect, wxWindow::GetClientSize
     */
	wxRect GetDrawableRect() const;

	wxRect GetStartRect(int index) const;
	wxRect GetStartRect(const BattleStartRect& sr) const;
	void Accumulate(wxImage& image) const;
	double GetStartRectMetalFraction(int index) const;
	double GetStartRectMetalFraction(const BattleStartRect& sr) const;

	void DrawOutlinedText(wxDC& dc, const wxString& str, int x, int y, const wxColour& outline, const wxColour& font) const;

	/** Get the relative (range: [0.0,1.0]) x- and y- coordinates of
     * the user's start position.
     */
	wxRealPoint GetUserMapPositionAsReal(const User& user) const;

	/** Get an absolute (relative to the client's [*this* MapCtrl
     * widget's] drawable area) user map position.
     *
     * The returned point is as would be used with wxDC methods.
     */
	wxPoint GetTranslatedScaledUserMapPosition(const User& user) const;

	wxRect GetUserRect(const User& user, bool selected);
	RectangleArea GetUserRectArea(const wxRect& userrect, int x, int y) const;

	wxRect GetUserSideRect() const
	{
		return wxRect(37, 20, 16, 16);
	}
	wxRect GetUserHandicapRect() const
	{
		return wxRect(40, 55, 16, 16);
	}
	wxRect GetUserCloseRect() const
	{
		return wxRect(59, 4, 14, 14);
	}
	wxRect GetUserUpAllyButtonRect() const
	{
		return wxRect(61, 35, 12, 8);
	}
	wxRect GetUserDownAllyButtonRect() const
	{
		return wxRect(61, 43, 12, 8);
	}
	wxRect GetUserUpHandicapButtonRect() const
	{
		return wxRect(61, 52, 12, 8);
	}
	wxRect GetUserDownHandicapButtonRect() const
	{
		return wxRect(61, 60, 12, 8);
	}

	wxRect GetRefreshRect() const;
	wxRect GetDownloadRect() const;

	unsigned int GetNewRectIndex() const;

	void RequireImages();

	void RelocateUsers();

	void GetClosestStartPos(int fromx, int fromy, int& index, int& x, int& y, int& range);

	void DrawUser(wxDC& dc, User& user, bool selected, bool moving);
	void DrawUserPositions(wxDC& dc);
	void DrawBackground(wxDC& dc);
	void DrawStartRects(wxDC& dc);
	void DrawStartPositions(wxDC& dc);
	void DrawStartRect(wxDC& dc, int index, wxRect& sr, const wxColour& col, bool mouseover, int alphalevel = 70, bool forceInsideMinimap = true);

	void SetMouseOverRect(int index);

	void _SetCursor();

	LSL::UnitSyncAsyncOps m_async;

	wxBitmap* m_minimap;
	wxBitmap* m_metalmap;
	wxBitmap* m_heightmap;
	wxImage m_metalmap_cumulative;

	IBattle* m_battle;

	std::string m_mapname;

	bool m_draw_start_types;
	bool m_ro;
	bool m_sp;

	int m_mover_rect;
	int m_mdown_rect;

	RectangleArea m_rect_area;
	RectangleArea m_last_rect_area;

	RectangleArea m_mdown_area;
	BattleStartRect m_tmp_brect;

	MouseAction m_maction;
	int m_mdown_x;
	int m_mdown_y;

	wxSize m_lastsize;

	wxBitmap* m_close_img;
	wxBitmap* m_close_hi_img;

	wxBitmap* m_start_ally;
	wxBitmap* m_start_enemy;
	wxBitmap* m_start_unused;

	wxBitmap* m_player_img;
	wxBitmap* m_bot_img;

	wxBitmap* m_nfound_img;
	wxBitmap* m_reload_img;
	wxBitmap* m_dl_img;

	LSL::UnitsyncMap m_map;

	User* m_user_expanded;

	enum InfoMap {
		IM_Minimap,  // must be first one
		IM_Metalmap, // entries must be consecutively numbered (without gaps)
		IM_Heightmap,
		IM_Count // must be last one
	} m_current_infomap;

	wxMutex m_mutex;

	DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPCTRL_H
