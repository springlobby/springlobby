#ifndef SPRINGLOBBY_WIDGETINFODIALOG_H_INCLUDED
#define SPRINGLOBBY_WIDGETINFODIALOG_H_INCLUDED

#include <wx/dialog.h>
class Widget;

class WidgetInfoDialog : public wxDialog
{
    public:
        WidgetInfoDialog(Widget& widget);
        virtual ~WidgetInfoDialog();
    protected:
    private:
};

#endif // SPRINGLOBBY_WIDGETINFODIALOG_H_INCLUDED
