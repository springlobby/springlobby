#ifndef SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
#define SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED

#include <wx/scrolwin.h>

class Widget;
class wxGridSizer;
class wxBoxSizer;

class WidgetInfoPanel : public wxScrolledWindow
{
    public:
        WidgetInfoPanel( Widget& widget, wxWindow* parent, wxWindowID id, const wxString& title = _T(""), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL );
        virtual ~WidgetInfoPanel();

        void Create();

    protected:
        Widget& m_widget;
        wxBoxSizer* m_main_sizer;
        wxBoxSizer* m_top_sizer;
        wxBoxSizer* m_button_sizer;
        wxGridSizer* m_grid_sizer;

        wxBoxSizer* m_busy_notice;

        bool GetFileInfos();
        bool GetImageInfos();
        bool DownloadImages();

};

#endif // SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
