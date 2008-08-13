#include "groupuserdialog.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>

#include "ui.h"
#include "nicklistctrl.h"
#include "userlist.h"
#include "server.h"
#include "useractions.h"
#include "settings.h"

BEGIN_EVENT_TABLE( GroupUserDialog, wxDialog )

	EVT_BUTTON( wxID_OK,     GroupUserDialog::OnOk )
	EVT_BUTTON( wxID_CANCEL, GroupUserDialog::OnCancel )
	EVT_BUTTON( ID_BUTTON_ADD, GroupUserDialog::OnAdd )
	EVT_BUTTON( ID_BUTTON_DELETE, GroupUserDialog::OnDelete)

END_EVENT_TABLE()

GroupUserDialog::GroupUserDialog(wxWindow* parent, wxWindowID id, const wxString& title,const wxString& group,
    const wxPoint& pos , const wxSize& size, long style, const wxString& name )
    : wxDialog (parent,  id, title, pos, size, style, name),
    m_groupname (group)

{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    wxBoxSizer* top_sizer = new wxBoxSizer ( wxHORIZONTAL );
    wxBoxSizer* leftCol = new wxBoxSizer ( wxVERTICAL );
    wxBoxSizer* rightCol = new wxBoxSizer ( wxVERTICAL );
    m_all_users = new NickListCtrl( this, true, 0, false, _T("AllUsersGroup") );
    m_group_users = new NickListCtrl( this, true, 0, false, _T("GroupUsersGroup") );

    //populate lists
    const UserList& userlist = ui().GetServer().GetUserList();
    for ( unsigned int i = 0; i < userlist.GetNumUsers(); ++i)
    {
        m_all_users->AddUser( userlist.GetUser( i ) );
    }

    wxSortedArrayString groupuser = sett().GetPeopleList( m_groupname );
    for ( unsigned int i = 0; i < groupuser.GetCount(); ++i)
    {
        m_group_users->AddUser( userlist.GetUser( groupuser[i] ) );
    }

    leftCol->Add( m_group_users, 10, wxALL|wxEXPAND, 10 );
    rightCol->Add( m_all_users, 10, wxALL|wxEXPAND, 10 );
    top_sizer->Add( leftCol,1, wxALL|wxEXPAND, 10 );
    top_sizer->Add( rightCol,1, wxALL|wxEXPAND, 10 );
    m_main_sizer->Add( top_sizer,1, wxALL|wxEXPAND, 10 );
    m_main_sizer->Add( CreateStdDialogButtonSizer(wxOK|wxCANCEL),0, wxALL|wxEXPAND, 10 );
    SetSizer( m_main_sizer );
    Layout();

}

GroupUserDialog::~GroupUserDialog()
{
    //dtor
}

void GroupUserDialog::OnOk ( wxCommandEvent& event )
{
    EndModal(0);
}

void GroupUserDialog::OnAdd ( wxCommandEvent& event )
{
    EndModal(0);
}

void GroupUserDialog::OnDelete ( wxCommandEvent& event )
{

}

void GroupUserDialog::OnCancel ( wxCommandEvent& event )
{

}
