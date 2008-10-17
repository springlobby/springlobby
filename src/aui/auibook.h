///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/auibook.h
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////



#ifndef _WX_AUINOTEBOOK_H_
#define _WX_AUINOTEBOOK_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>

#if wxUSE_AUI

#include <wx/aui/framemanager.h>
#include <wx/aui/dockart.h>
#include <wx/aui/floatpane.h>
#include <wx/control.h>


namespace SL_Extern {
class SLAuiNotebook;


enum SLAuiNotebookOption
{
    wxAUI_NB_TOP                 = 1 << 0,
    wxAUI_NB_LEFT                = 1 << 1,  // not implemented yet
    wxAUI_NB_RIGHT               = 1 << 2,  // not implemented yet
    wxAUI_NB_BOTTOM              = 1 << 3,
    wxAUI_NB_TAB_SPLIT           = 1 << 4,
    wxAUI_NB_TAB_MOVE            = 1 << 5,
    wxAUI_NB_TAB_EXTERNAL_MOVE   = 1 << 6,
    wxAUI_NB_TAB_FIXED_WIDTH     = 1 << 7,
    wxAUI_NB_SCROLL_BUTTONS      = 1 << 8,
    wxAUI_NB_WINDOWLIST_BUTTON   = 1 << 9,
    wxAUI_NB_CLOSE_BUTTON        = 1 << 10,
    wxAUI_NB_CLOSE_ON_ACTIVE_TAB = 1 << 11,
    wxAUI_NB_CLOSE_ON_ALL_TABS   = 1 << 12,
    wxAUI_NB_MIDDLE_CLICK_CLOSE  = 1 << 13,

    wxAUI_NB_DEFAULT_STYLE = wxAUI_NB_TOP |
                             wxAUI_NB_TAB_SPLIT |
                             wxAUI_NB_TAB_MOVE |
                             wxAUI_NB_SCROLL_BUTTONS |
                             wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                             wxAUI_NB_MIDDLE_CLICK_CLOSE
};




// aui notebook event class

class SLAuiNotebookEvent : public wxNotifyEvent
{
public:
    SLAuiNotebookEvent(wxEventType command_type = wxEVT_NULL,
                       int win_id = 0)
          : wxNotifyEvent(command_type, win_id)
    {
        old_selection = -1;
        selection = -1;
        drag_source = NULL;
    }
#ifndef SWIG
    SLAuiNotebookEvent(const SLAuiNotebookEvent& c) : wxNotifyEvent(c)
    {
        old_selection = c.old_selection;
        selection = c.selection;
        drag_source = c.drag_source;
    }
#endif
    wxEvent *Clone() const { return new SLAuiNotebookEvent(*this); }

    void SetSelection(int s) { selection = s; m_commandInt = s; }
    int GetSelection() const { return selection; }

    void SetOldSelection(int s) { old_selection = s; }
    int GetOldSelection() const { return old_selection; }

    void SetDragSource(SLAuiNotebook* s) { drag_source = s; }
    SLAuiNotebook* GetDragSource() const { return drag_source; }

public:
    int old_selection;
    int selection;
    SLAuiNotebook* drag_source;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(SLAuiNotebookEvent)
#endif
};


class  SLAuiNotebookPage
{
public:
    wxWindow* window;     // page's associated window
    wxString caption;     // caption displayed on the tab
    wxBitmap bitmap;      // tab's bitmap
    wxRect rect;          // tab's hit rectangle
    bool active;          // true if the page is currently active
};

class  SLAuiTabContainerButton
{
public:

    int id;               // button's id
    int cur_state;        // current state (normal, hover, pressed, etc.)
    int location;         // buttons location (wxLEFT, wxRIGHT, or wxCENTER)
    wxBitmap bitmap;      // button's hover bitmap
    wxBitmap dis_bitmap;  // button's disabled bitmap
    wxRect rect;          // button's hit rectangle
};


#ifndef SWIG
WX_DECLARE_USER_EXPORTED_OBJARRAY(SLAuiNotebookPage, SLAuiNotebookPageArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(SLAuiTabContainerButton, SLAuiTabContainerButtonArray, WXDLLIMPEXP_AUI);
#endif


// tab art class

class  SLAuiTabArt
{
public:

    SLAuiTabArt() { }
    virtual ~SLAuiTabArt() { }

    virtual SLAuiTabArt* Clone() = 0;
    virtual void SetFlags(unsigned int flags) = 0;

    virtual void SetSizingInfo(const wxSize& tab_ctrl_size,
                               size_t tab_count) = 0;

    virtual void SetNormalFont(const wxFont& font) = 0;
    virtual void SetSelectedFont(const wxFont& font) = 0;
    virtual void SetMeasuringFont(const wxFont& font) = 0;

    virtual void DrawBackground(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& rect) = 0;

    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const SLAuiNotebookPage& pane,
                         const wxRect& in_rect,
                         int close_button_state,
                         wxRect* out_tab_rect,
                         wxRect* out_button_rect,
                         int* x_extent) = 0;

    virtual void DrawButton(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& in_rect,
                         int bitmap_id,
                         int button_state,
                         int orientation,
                         wxRect* out_rect) = 0;

    virtual wxSize GetTabSize(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxString& caption,
                         const wxBitmap& bitmap,
                         bool active,
                         int close_button_state,
                         int* x_extent) = 0;

    virtual int ShowDropDown(
                         wxWindow* wnd,
                         const SLAuiNotebookPageArray& items,
                         int active_idx) = 0;

    virtual int GetIndentSize() = 0;

    virtual int GetBestTabCtrlSize(
                         wxWindow* wnd,
                         const SLAuiNotebookPageArray& pages,
                         const wxSize& required_bmp_size) = 0;
};


class  SLAuiDefaultTabArt : public SLAuiTabArt
{

public:

    SLAuiDefaultTabArt();
    virtual ~SLAuiDefaultTabArt();

    SLAuiTabArt* Clone();
    void SetFlags(unsigned int flags);
    void SetSizingInfo(const wxSize& tab_ctrl_size,
                       size_t tab_count);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const SLAuiNotebookPage& pane,
                 const wxRect& in_rect,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& in_rect,
                 int bitmap_id,
                 int button_state,
                 int orientation,
                 wxRect* out_rect);

    int GetIndentSize();

    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int close_button_state,
                 int* x_extent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const SLAuiNotebookPageArray& items,
                 int active_idx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const SLAuiNotebookPageArray& pages,
                 const wxSize& required_bmp_size);

protected:

    wxFont m_normal_font;
    wxFont m_selected_font;
    wxFont m_measuring_font;
    wxColour m_base_colour;
    wxPen m_base_colour_pen;
    wxPen m_border_pen;
    wxBrush m_base_colour_brush;
    wxBitmap m_active_close_bmp;
    wxBitmap m_disabled_close_bmp;
    wxBitmap m_active_left_bmp;
    wxBitmap m_disabled_left_bmp;
    wxBitmap m_active_right_bmp;
    wxBitmap m_disabled_right_bmp;
    wxBitmap m_active_windowlist_bmp;
    wxBitmap m_disabled_windowlist_bmp;

    int m_fixed_tab_width;
    int m_tab_ctrl_height;
    unsigned int m_flags;
};


class  SLAuiSimpleTabArt : public SLAuiTabArt
{

public:

    SLAuiSimpleTabArt();
    virtual ~SLAuiSimpleTabArt();

    SLAuiTabArt* Clone();
    void SetFlags(unsigned int flags);

    void SetSizingInfo(const wxSize& tab_ctrl_size,
                       size_t tab_count);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const SLAuiNotebookPage& pane,
                 const wxRect& in_rect,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& in_rect,
                 int bitmap_id,
                 int button_state,
                 int orientation,
                 wxRect* out_rect);

    int GetIndentSize();

    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int close_button_state,
                 int* x_extent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const SLAuiNotebookPageArray& items,
                 int active_idx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const SLAuiNotebookPageArray& pages,
                 const wxSize& required_bmp_size);

protected:

    wxFont m_normal_font;
    wxFont m_selected_font;
    wxFont m_measuring_font;
    wxPen m_normal_bkpen;
    wxPen m_selected_bkpen;
    wxBrush m_normal_bkbrush;
    wxBrush m_selected_bkbrush;
    wxBrush m_bkbrush;
    wxBitmap m_active_close_bmp;
    wxBitmap m_disabled_close_bmp;
    wxBitmap m_active_left_bmp;
    wxBitmap m_disabled_left_bmp;
    wxBitmap m_active_right_bmp;
    wxBitmap m_disabled_right_bmp;
    wxBitmap m_active_windowlist_bmp;
    wxBitmap m_disabled_windowlist_bmp;

    int m_fixed_tab_width;
    unsigned int m_flags;
};


class  SLAuiTabContainer
{
public:

    SLAuiTabContainer();
    virtual ~SLAuiTabContainer();

    void SetArtProvider(SLAuiTabArt* art);
    SLAuiTabArt* GetArtProvider() const;

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    bool AddPage(wxWindow* page, const SLAuiNotebookPage& info);
    bool InsertPage(wxWindow* page, const SLAuiNotebookPage& info, size_t idx);
    bool MovePage(wxWindow* page, size_t new_idx);
    bool RemovePage(wxWindow* page);
    bool SetActivePage(wxWindow* page);
    bool SetActivePage(size_t page);
    void SetNoneActive();
    int GetActivePage() const;
    bool TabHitTest(int x, int y, wxWindow** hit) const;
    bool ButtonHitTest(int x, int y, SLAuiTabContainerButton** hit) const;
    wxWindow* GetWindowFromIdx(size_t idx) const;
    int GetIdxFromWindow(wxWindow* page) const;
    size_t GetPageCount() const;
    SLAuiNotebookPage& GetPage(size_t idx);
    const SLAuiNotebookPage& GetPage(size_t idx) const;
    SLAuiNotebookPageArray& GetPages();
    void SetNormalFont(const wxFont& normal_font);
    void SetSelectedFont(const wxFont& selected_font);
    void SetMeasuringFont(const wxFont& measuring_font);
    void DoShowHide();
    void SetRect(const wxRect& rect);

    void RemoveButton(int id);
    void AddButton(int id,
                   int location,
                   const wxBitmap& normal_bitmap = wxNullBitmap,
                   const wxBitmap& disabled_bitmap = wxNullBitmap);

    size_t GetTabOffset() const;
    void SetTabOffset(size_t offset);

    // Is the tab visible?
    bool IsTabVisible(int tabPage, int tabOffset, wxDC* dc, wxWindow* wnd);

    // Make the tab visible if it wasn't already
    void MakeTabVisible(int tabPage, wxWindow* win);

protected:

    virtual void Render(wxDC* dc, wxWindow* wnd);

protected:

    SLAuiTabArt* m_art;
    SLAuiNotebookPageArray m_pages;
    SLAuiTabContainerButtonArray m_buttons;
    SLAuiTabContainerButtonArray m_tab_close_buttons;
    wxRect m_rect;
    size_t m_tab_offset;
    unsigned int m_flags;
};



class  SLAuiTabCtrl : public wxControl,
                                     public SLAuiTabContainer
{
public:

    SLAuiTabCtrl(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0);

    ~SLAuiTabCtrl();

#if wxABI_VERSION >= 20805
    bool IsDragging() const { return m_is_dragging; }
#endif

protected:

    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
#if wxABI_VERSION >= 20805
    void OnMiddleDown(wxMouseEvent& evt);
    void OnMiddleUp(wxMouseEvent& evt);
    void OnRightDown(wxMouseEvent& evt);
    void OnRightUp(wxMouseEvent& evt);
    void OnLeftDClick(wxMouseEvent& evt);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnChar(wxKeyEvent& event);
#endif
    void OnMotion(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnButton(SLAuiNotebookEvent& evt);


protected:

    wxPoint m_click_pt;
    wxWindow* m_click_tab;
    bool m_is_dragging;
    SLAuiTabContainerButton* m_hover_button;
    SLAuiTabContainerButton* m_pressed_button;

#ifndef SWIG
    DECLARE_CLASS(SLAuiTabCtrl)
    DECLARE_EVENT_TABLE()
#endif
};




class  SLAuiNotebook : public wxControl
{

public:

    SLAuiNotebook();

    SLAuiNotebook(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE);

    virtual ~SLAuiNotebook();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    void SetWindowStyleFlag(long style);
    void SetArtProvider(SLAuiTabArt* art);
    SLAuiTabArt* GetArtProvider() const;

    virtual void SetUniformBitmapSize(const wxSize& size);
    virtual void SetTabCtrlHeight(int height);

    bool AddPage(wxWindow* page,
                 const wxString& caption,
                 bool select = false,
                 const wxBitmap& bitmap = wxNullBitmap);

    bool InsertPage(size_t page_idx,
                    wxWindow* page,
                    const wxString& caption,
                    bool select = false,
                    const wxBitmap& bitmap = wxNullBitmap);

    bool DeletePage(size_t page);
    bool RemovePage(size_t page);

    size_t GetPageCount() const;
    wxWindow* GetPage(size_t page_idx) const;
    int GetPageIndex(wxWindow* page_wnd) const;

    bool SetPageText(size_t page, const wxString& text);
    wxString GetPageText(size_t page_idx) const;

    bool SetPageBitmap(size_t page, const wxBitmap& bitmap);
    wxBitmap GetPageBitmap(size_t page_idx) const;

    size_t SetSelection(size_t new_page);
    int GetSelection() const;

    virtual void Split(size_t page, int direction);

#if wxABI_VERSION >= 20801
    const wxAuiManager& GetAuiManager() const { return m_mgr; }
#endif

#if wxABI_VERSION >= 20805
    // Sets the normal font
    void SetNormalFont(const wxFont& font);

    // Sets the selected tab font
    void SetSelectedFont(const wxFont& font);

    // Sets the measuring font
    void SetMeasuringFont(const wxFont& font);

    // Sets the tab font
    virtual bool SetFont(const wxFont& font);

    // Gets the tab control height
    int GetTabCtrlHeight() const;

    // Gets the height of the notebook for a given page height
    int GetHeightForPageHeight(int pageHeight);

    // Advances the selection, generation page selection events
    void AdvanceSelection(bool forward = true);

    // Shows the window menu
    bool ShowWindowMenu();
#endif

protected:

    // these can be overridden
    virtual void UpdateTabCtrlHeight();
    virtual int CalculateTabCtrlHeight();
    virtual wxSize CalculateNewSplitSize();

protected:

    void DoSizing();
    void InitNotebook(long style);
    SLAuiTabCtrl* GetTabCtrlFromPoint(const wxPoint& pt);
    wxWindow* GetTabFrameFromTabCtrl(wxWindow* tab_ctrl);
    SLAuiTabCtrl* GetActiveTabCtrl();
    bool FindTab(wxWindow* page, SLAuiTabCtrl** ctrl, int* idx);
    void RemoveEmptyTabFrames();
    void UpdateHintWindowSize();

protected:

    void OnChildFocus(wxChildFocusEvent& evt);
    void OnRender(wxAuiManagerEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnTabClicked(wxCommandEvent& evt);
    void OnTabBeginDrag(wxCommandEvent& evt);
    void OnTabDragMotion(wxCommandEvent& evt);
    void OnTabEndDrag(wxCommandEvent& evt);
    void OnTabButton(wxCommandEvent& evt);
#if wxABI_VERSION >= 20805
    void OnTabMiddleDown(wxCommandEvent& evt);
    void OnTabMiddleUp(wxCommandEvent& evt);
    void OnTabRightDown(wxCommandEvent& evt);
    void OnTabRightUp(wxCommandEvent& evt);
    void OnNavigationKey(wxNavigationKeyEvent& event);
    void OnTabBgDClick(wxCommandEvent& evt);
#endif

protected:

    wxAuiManager m_mgr;
    SLAuiTabContainer m_tabs;
    int m_curpage;
    int m_tab_id_counter;
    wxWindow* m_dummy_wnd;

    wxSize m_requested_bmp_size;
    int m_requested_tabctrl_height;
    wxFont m_selected_font;
    wxFont m_normal_font;
    int m_tab_ctrl_height;

    int m_last_drag_x;
    unsigned int m_flags;

#ifndef SWIG
    DECLARE_CLASS(SLAuiNotebook)
    DECLARE_EVENT_TABLE()
#endif
};




// wx event machinery

#ifndef SWIG

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, 0)
#if wxABI_VERSION >= 20805
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, 0)
#endif
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*SLAuiNotebookEventFunction)(SLAuiNotebookEvent&);

#define SLAuiNotebookEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(SLAuiNotebookEventFunction, &func)

#define EVT_AUINOTEBOOK_PAGE_CLOSE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CLOSED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BUTTON(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BUTTON, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BEGIN_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_END_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_MOTION(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_ALLOW_DND(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, winid, SLAuiNotebookEventHandler(fn))

#if wxABI_VERSION >= 20805
#define EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_MIDDLE_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_RIGHT_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_DONE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, winid, SLAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BG_DCLICK(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, winid, SLAuiNotebookEventHandler(fn))
#endif

#else

// wxpython/swig event work
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BUTTON;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_END_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP;

%pythoncode {
    EVT_AUINOTEBOOK_PAGE_CLOSE = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 1 )
    EVT_AUINOTEBOOK_PAGE_CLOSED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 1 )
    EVT_AUINOTEBOOK_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 1 )
    EVT_AUINOTEBOOK_BEGIN_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 1 )
    EVT_AUINOTEBOOK_END_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 1 )
    EVT_AUINOTEBOOK_DRAG_MOTION = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 1 )
    EVT_AUINOTEBOOK_ALLOW_DND = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, 1 )
    EVT_AUINOTEBOOK_DRAG_DONE = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, 1 )
    EVT_AUINOTEBOOK_BG_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, 1 )
    EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, 1 )
    EVT_AUINOTEBOOK_TAB_MIDDLE_UP  = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP , 1 )
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, 1 )
    EVT_AUINOTEBOOK_TAB_RIGHT_UP = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, 1 )
}
#endif

}//end namespace SL_EXTERN

#endif  // wxUSE_AUI
#endif  // _WX_AUINOTEBOOK_H_
