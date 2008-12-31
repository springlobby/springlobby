#ifndef SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
#define SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED

#include <wx/scrolwin.h>

class Widget;

class WidgetInfoPanel : public wxScrolledWindow
{
    public:
        WidgetInfoPanel( Widget& widget, wxWindow* parent, wxWindowID id, const wxString& title = _T(""), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL );
        virtual ~WidgetInfoPanel();

    protected:
        Widget& m_widget;

        bool GetFileInfos();
        bool GetImageInfos();
        bool DownloadImages();

};

#endif // SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
