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
class wxTextCtrl;
class ImageViewerPanel;
class wxHtmlWindow;
class SLNotebook;

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
        wxBoxSizer* m_left_sizer;
        wxBoxSizer* m_right_sizer;
        wxBoxSizer* m_left_button_sizer;
//        wxBoxSizer* m_right_button_sizer;
        wxGridSizer* m_grid_sizer;
//        wxBoxSizer* m_desc_sizer;
//        wxBoxSizer* m_screeny_sizer;
//        wxBoxSizer* m_chglog_sizer;
        wxBoxSizer* m_variable_info_sizer;

        wxBoxSizer* m_busy_notice;

        wxTextCtrl* m_changelog;
        ImageViewerPanel* m_imageviewer;
        wxHtmlWindow* m_desc;

        SLNotebook* m_ext_info;
        wxButton* m_update;
        wxButton* m_remove;
        wxButton* m_download;


        void SetButtonStates();

        void OnDownload( wxCommandEvent& evt );
        void OnPics( wxCommandEvent& evt );
        void OnChangeLog( wxCommandEvent& evt );
        void OnDescription( wxCommandEvent& evt );
        void OnRemove( wxCommandEvent& evt );
        void OnUpdate( wxCommandEvent& evt );

        enum {
            BUT_DOWNLOAD,
            BUT_CHG_LOG,
            BUT_UPDATE,
            BUT_REMOVE,
            BUT_PICS,
            CTL_DESC,
            BUT_DESC
        };

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_WIDGETINFOPANEL_H_INCLUDED
