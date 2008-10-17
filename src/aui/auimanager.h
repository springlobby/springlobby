#ifndef SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
#define SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H

#include <wx/aui/aui.h>
#include <wx/timer.h>

class AuiManagerContainer
{
  public:
    AuiManagerContainer() {}

    wxAuiManager* manager;

};

AuiManagerContainer& GetAui();

class SLTipWindow;

class SLAuiNotebook : public wxAuiNotebook
{
    public:
        SLAuiNotebook();

        SLAuiNotebook(wxWindow* parent,
                      wxWindowID id = wxID_ANY,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxAUI_NB_DEFAULT_STYLE);

        virtual ~SLAuiNotebook();

    protected:
    /** Tooltip related **/
        #if wxUSE_TIPWINDOW
            //! some wx implementations do not support this yet
            SLTipWindow* m_tipwindow;
            SLTipWindow** controlPointer;
        #endif

        static const unsigned int m_tooltip_delay    = 10;
        static const unsigned int m_tooltip_duration = 2000;
        wxPoint m_last_mouse_pos;
        //! starts timer, sets tooltiptext
        virtual void OnMouseMotion(wxMouseEvent& event);
        //! this event is triggered when delay timer (set in mousemotion) ended
        virtual void OnTimer(wxTimerEvent& event);
        //! used to display tooltips for a certain amount of time
        wxTimer m_tiptimer;
        //! always set to the currrently displayed tooltip text
        wxString m_tiptext;
    /********************************/

//        enum {
//            ID_TIMER = 9723
//        };


        DECLARE_EVENT_TABLE()
};


class SLArtProvider : public wxAuiTabArt
{

public:

    SLArtProvider();
    virtual ~SLArtProvider();

    wxAuiTabArt* Clone();
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
                 const wxAuiNotebookPage& pane,
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
                 const wxAuiNotebookPageArray& items,
                 int active_idx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
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


#endif // SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
