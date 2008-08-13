#ifndef GROUPUSERDIALOG_H
#define GROUPUSERDIALOG_H

#include <wx/dialog.h>


class NickListCtrl;
class wxButton;
class wxBoxSizer;

class GroupUserDialog : public wxDialog
{
    public:
        GroupUserDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = _T("dialogBox") );
        virtual ~GroupUserDialog();
    protected:
        NickListCtrl* m_all_users;
        NickListCtrl* m_group_users;
        wxButton* m_delete_users;
        wxButton* m_add_users;
        wxBoxSizer* m_main_sizer;

        void OnOk( wxCommandEvent& event );
		void OnCancel( wxCommandEvent& event );
        void OnAdd( wxCommandEvent& event );
		void OnDelete( wxCommandEvent& event );

        enum {
            ID_BUTTON_ADD = wxID_HIGHEST,
            ID_BUTTON_DELETE
        };

        DECLARE_EVENT_TABLE();

};

#endif // GROUPUSERDIALOG_H
