#include "groupuserdialog.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/tokenzr.h>

#include "ui.h"
#include "nicklistctrl.h"
#include "userlistctrl.h"
#include "userlist.h"
#include "server.h"
#include "useractions.h"
#include "settings.h"
#include "user.h"
#include "settings++/custom_dialogs.h"

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
    m_all_users = new UserListctrl( this, _T("AllUsersGroup"), false );
    m_group_users = new UserListctrl( this, _T("GroupUsersGroup"), false );

    //populate lists
    typedef UserListctrl::UserData UserData;

    if ( ui().IsConnected() )
    {
        const UserList& userlist = ui().GetServer().GetUserList();
        for ( unsigned int i = 0; i < userlist.GetNumUsers(); ++i)
        {
            wxString name = userlist.GetUser( i ).GetNick();
            wxString country = userlist.GetUser( i ).GetCountry();
            if ( !useractions().IsKnown( name ) && !ui().IsThisMe( name ) )
                m_all_users->AddUser( UserData( name, country ) );
        }
    }

    wxSortedArrayString groupuser = sett().GetPeopleList( m_groupname );
    unsigned int num = groupuser.GetCount();
    for ( unsigned int i = 0; i < num; ++i)
    {
        wxString name = groupuser[i];
        wxString country = wxEmptyString;
        if ( ui().IsConnected() ) {
            const UserList& userlist = ui().GetServer().GetUserList();
            if ( userlist.UserExists( name ) )
                country = userlist.GetUser( groupuser[i] ).GetCountry();
        }

        m_group_users->AddUser( UserData( name, country ) );
    }

    m_add_users = new wxButton( this, ID_BUTTON_ADD, _("Add user") );
    m_delete_users = new wxButton( this, ID_BUTTON_DELETE, _("Remove selected") );
    wxStaticText* m_username_label = new wxStaticText( this, -1, _("you can also enter a ';' seperated list of usernames:") );
    m_usernames_ctrl = new wxTextCtrl ( this, -1, _("") );

    leftCol->Add( m_group_users, 10, wxALL|wxEXPAND, 10 );
    leftCol->Add( m_delete_users, 0, wxALL|wxEXPAND, 10 );
    rightCol->Add( m_all_users, 10, wxALL|wxEXPAND, 10 );

    if ( !ui().IsConnected() )
    {
        wxStaticText* warning = new wxStaticText( this, -1, _("not connected to server, no user to display") );
        warning->SetForegroundColour( sett().GetChatColorError() );
        rightCol->Add( warning, 0, wxALL|wxEXPAND, 10 );
    }

    rightCol->Add( m_username_label , 0, wxALL|wxEXPAND, 10 );
    rightCol->Add( m_usernames_ctrl , 0, wxALL|wxEXPAND, 10 );
    rightCol->Add( m_add_users, 0, wxALL|wxEXPAND, 10 );
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
    wxArrayString group_user = m_group_users->GetUserNicks( );
    unsigned int num = group_user.GetCount();
    for ( unsigned int i = 0; i < num ; ++i){
        wxString name = group_user[i];
        if ( !useractions().IsKnown( name ) );
            useractions().AddUserToGroup( m_groupname, name );
    }

    this->Show( 0 );
}

void GroupUserDialog::OnCancel ( wxCommandEvent& event )
{
    this->Show( 0 );
}

void GroupUserDialog::OnAdd ( wxCommandEvent& event )
{
    const UserListctrl::UserDataMap sel_users ( m_all_users->GetSelectedUserData( ));
    m_all_users->SelectNone();
    m_all_users->RemoveUsers( sel_users );
    m_group_users->AddUser( sel_users );

    wxArrayString faulty_nicknames;
    wxString list = m_usernames_ctrl->GetValue();
    if ( !list.IsEmpty() )
    {
        wxStringTokenizer usersnames( list, _T( ";" ) );
        while ( usersnames.HasMoreTokens() )
        {
            wxString name = usersnames.GetNextToken();
            name  = name.Trim(true).Trim(false);//trim left and right whitespace
            wxString country = wxEmptyString;
            if ( ui().IsConnected() ) {
                const UserList& userlist = ui().GetServer().GetUserList();
                if ( userlist.UserExists( name ) )
                    country = userlist.GetUser( name ).GetCountry();
            }
            if ( IsValidNickname( name ) )
                m_group_users->AddUser( UserListctrl::UserData( name, country ) );
            else if ( !name.IsEmpty() )
                faulty_nicknames.Add( name );
        }
        wxString new_textctrl_content = wxEmptyString;
        if ( faulty_nicknames.GetCount() > 0 )
        {
            customMessageBox( SL_MAIN_ICON, _("At least one username contained non-ascii or whitespace character.\nPlease correct and try again."),
                                _("Invalid usernames") );
            unsigned int i = 0;
            for ( ; i < faulty_nicknames.GetCount() ; ++i )
                new_textctrl_content << faulty_nicknames[i] + _T(";");
            if (i > 0)
                new_textctrl_content = new_textctrl_content.BeforeLast( *wxT(";") );
        }
        m_usernames_ctrl->SetValue( new_textctrl_content );
    }

}

void GroupUserDialog::OnDelete ( wxCommandEvent& event )
{
    const UserListctrl::UserDataMap sel_users = m_group_users->GetSelectedUserData( );
    m_group_users->SelectNone();
    m_group_users->RemoveUsers( sel_users );
    m_all_users->AddUser( sel_users );
}


