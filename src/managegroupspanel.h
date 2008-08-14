#ifndef SPRINGLOBBY_HEADERGUARD_MANAGEGROUPSPANEL_H
#define SPRINGLOBBY_HEADERGUARD_MANAGEGROUPSPANEL_H

#include <wx/scrolwin.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;
class wxSizer;
class wxCommandEvent;
class GroupUserDialog;

class ManageGroupsPanel: public wxScrolledWindow
{
    public:
         ManageGroupsPanel( wxWindow* parent );
         ~ManageGroupsPanel();

    protected:
        wxSizer* GetGroupSizer( const wxString& group );
        wxBoxSizer* m_main_sizer;
        wxBoxSizer* m_groups_sizer;
        wxTextCtrl* m_newgroup;
        GroupUserDialog* m_user_dialog;

        enum {
          ID_COLOR_BUTTON = wxID_HIGHEST - 600,
          ID_ADD_BUTTON = ID_COLOR_BUTTON -1,
          ID_DEL_BUTTON = ID_COLOR_BUTTON -2,
          ID_USER_BUTTON = ID_COLOR_BUTTON -3

        };

        void OnColorButton( wxCommandEvent& event );
        void OnAddButton( wxCommandEvent& event );
        void OnUserButton( wxCommandEvent& event );
        void OnDeleteButton( wxCommandEvent& event );
        void OnCheckBox( wxCommandEvent& event );
        void ReloadGroupSizer();

        void SetupControls();

        DECLARE_EVENT_TABLE()
};

#endif
