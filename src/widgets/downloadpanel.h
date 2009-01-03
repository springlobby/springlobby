#ifndef SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
#define SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED

#include <wx/scrolwin.h>

class WidgetDownloadListctrl;
class WidgetInfoPanel;
class wxBoxSizer;
class wxButton;
class wxTextCtrl;
class wxStaticText;
class wxListEvent;

class WidgetDownloadPanel : public wxScrolledWindow
{
    public:
        WidgetDownloadPanel(wxWindow* parent, wxWindowID id, const wxString& title = _T(""), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxHSCROLL | wxVSCROLL );
        virtual ~WidgetDownloadPanel();

    protected:
        bool PopulateList();
        void OnSelect( wxListEvent& event );

        WidgetDownloadListctrl* m_list;
        WidgetInfoPanel* m_info_panel;

        wxBoxSizer* m_main_sizer;

        enum {
            ID_LIST,
            ID_PANEL
        };

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
