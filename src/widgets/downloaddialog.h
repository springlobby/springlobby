#ifndef SPRINGLOBBY_WIDGETDOWNLOADDIALOG_H_INCLUDED
#define SPRINGLOBBY_WIDGETDOWNLOADDIALOG_H_INCLUDED

#include <wx/dialog.h>
#include "../gui/windowattributespickle.h"
class wxBoxSizer;
class WidgetDownloadPanel;

class WidgetDownloadDialog : public wxDialog, public WindowAttributesPickle
{
    public:
        WidgetDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~WidgetDownloadDialog();

        bool Show( bool show = true );

    protected:
        wxBoxSizer* m_main_sizer;
        WidgetDownloadPanel* m_widgets_panel;
};

#endif // SPRINGLOBBY_WIDGETDOWNLOADDIALOG_H_INCLUDED
