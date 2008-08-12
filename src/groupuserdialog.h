#ifndef GROUPUSERDIALOG_H
#define GROUPUSERDIALOG_H

#include <wx/dialog.h>


class GroupUserDialog : public wxDialog
{
    public:
        GroupUserDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = _T("dialogBox") );
        virtual ~GroupUserDialog();
    protected:
    private:
};

#endif // GROUPUSERDIALOG_H
