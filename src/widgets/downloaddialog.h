#ifndef SPRINGLOBBY_WIDGETDOWNLOADDIALOG_H_INCLUDED
#define SPRINGLOBBY_WIDGETDOWNLOADDIALOG_H_INCLUDED

#include <wx/dialog.h>


class WidgetDownloadDialog : public wxDialog
{
    public:
        WidgetDownloadDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~WidgetDownloadDialog();
    protected:
    private:
};

#endif // SPRINGLOBBY_WIDGETDOWNLOADDIALOG_H_INCLUDED
