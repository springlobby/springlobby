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

class ManageGroupsPanel: public wxScrolledWindow
{
    public:
         ManageGroupsPanel( wxWindow* parent );
         ~ManageGroupsPanel();

    protected:
        wxSizer* GetGroupSizer( const wxString& group );
        wxBoxSizer* m_main_sizer;
};

#endif
