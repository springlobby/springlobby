#include "groupuserdialog.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>

#include "ui.h"
#include "nicklistctrl.h"

BEGIN_EVENT_TABLE( GroupUserDialog, wxDialog )

	EVT_BUTTON( wxID_OK,     GroupUserDialog::OnOk )
	EVT_BUTTON( wxID_CANCEL, GroupUserDialog::OnCancel )
	EVT_BUTTON( ID_BUTTON_ADD, GroupUserDialog::OnAdd )
	EVT_BUTTON( ID_BUTTON_DELETE, GroupUserDialog::OnDelete)

END_EVENT_TABLE()

GroupUserDialog::GroupUserDialog(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size,
    long style, const wxString& name )
{
    m_main_sizer = new wxBoxSizer ( wxHORIZONTAL );
    wxBoxSizer* leftCol = new wxBoxSizer ( wxVERTICAL );
    wxBoxSizer* rightCol = new wxBoxSizer ( wxVERTICAL );
    m_all_users = new NickListCtrl( this, true, 0, false, _T("AllUsersGroup") );
    m_group_users = new NickListCtrl( this, true, 0, false, _T("AllUsersGroup") );

}

GroupUserDialog::~GroupUserDialog()
{
    //dtor
}

void GroupUserDialog::OnOk ( wxCommandEvent& event )
{

}

void GroupUserDialog::OnAdd ( wxCommandEvent& event )
{

}

void GroupUserDialog::OnDelete ( wxCommandEvent& event )
{

}

void GroupUserDialog::OnCancel ( wxCommandEvent& event )
{

}
