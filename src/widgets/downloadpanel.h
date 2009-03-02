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
class wxSplitterWindow;
class wxSplitterEvent;

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

        wxSplitterWindow* m_splitter;
        wxBoxSizer* m_main_sizer;

        void OnSashChanged( wxSplitterEvent& evt );

        enum {
            ID_LIST,
            ID_PANEL
        };

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_WIDGETDOWNLOADPANEL_H_INCLUDED
