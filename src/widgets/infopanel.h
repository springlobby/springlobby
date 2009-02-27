#ifndef SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
#define SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED

#include <wx/scrolwin.h>

class Widget;
class wxGridSizer;
class wxBoxSizer;
class wxButton;
class wxCommandEvent;
class wxTextUrlEvent;
class ServerMessageBox;

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

        ServerMessageBox* m_changelog;

        wxButton* m_download;
        wxButton* m_chg_log;
        wxButton* m_update;
        wxButton* m_remove;
        wxButton* m_pics;

        void SetButtonStates();

        void OnDownload( wxCommandEvent& evt );
        void OnPics( wxCommandEvent& evt );
        void OnChangeLog( wxCommandEvent& evt );
        void OnRemove( wxCommandEvent& evt );
        void OnUpdate( wxCommandEvent& evt );

        enum {
            BUT_DOWNLOAD,
            BUT_CHG_LOG,
            BUT_UPDATE,
            BUT_REMOVE,
            BUT_PICS,
            CTL_DESC
        };

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
