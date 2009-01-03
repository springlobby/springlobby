#ifndef SPRINGLOBBY_WIDGETINFODIALOG_H_INCLUDED
#define SPRINGLOBBY_WIDGETINFODIALOG_H_INCLUDED

#include <wx/dialog.h>
class Widget;
class WidgetInfoPanel;
class wxBoxSizer;

class WidgetInfoDialog : public wxDialog
{
    public:
        WidgetInfoDialog(Widget& widget,wxWindow* parent, wxWindowID id, const wxString& title,
            long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE );
        virtual ~WidgetInfoDialog();

    protected:
        WidgetInfoPanel* m_panel;
        wxBoxSizer* m_main_sizer;
};

#endif // SPRINGLOBBY_WIDGETINFODIALOG_H_INCLUDED
