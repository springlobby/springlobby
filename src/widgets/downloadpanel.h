#ifndef SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
#define SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED

#include <wx/scrolwin.h>


class WidgetDownloadPanel : public wxScrolledWindow
{
    public:
        WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL );
        virtual ~WidgetDownloadPanel();
    protected:
    private:
};

#endif // SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
