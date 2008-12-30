#ifndef SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
#define SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED

#include <wx/scrolwin.h>

class WidgetDownloadListctrl;
class wxBoxSizer;
class wxButton;
class wxTextCtrl;
class wxStaticText;

class WidgetDownloadPanel : public wxScrolledWindow
{
    public:
        WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL );
        virtual ~WidgetDownloadPanel();

    protected:
        bool PopulateList();

        WidgetDownloadListctrl* m_list;

        wxBoxSizer* m_main_sizer;
};

#endif // SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
