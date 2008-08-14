#include "managegroupspanel.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/colordlg.h>
#include <wx/checkbox.h>
#include "useractions.h"
#include "settings.h"
#include "uiutils.h"
#include "Helper/colorbutton.h"
#include "ui.h"
#include <cmath>
#include "groupuserdialog.h"
#include "settings++/custom_dialogs.h"

BEGIN_EVENT_TABLE( ManageGroupsPanel, wxScrolledWindow )
  EVT_CHECKBOX( wxID_ANY, ManageGroupsPanel::OnCheckBox )
  EVT_BUTTON( ID_COLOR_BUTTON, ManageGroupsPanel::OnColorButton )
  EVT_BUTTON( ID_ADD_BUTTON, ManageGroupsPanel::OnAddButton )
  EVT_BUTTON( ID_DEL_BUTTON, ManageGroupsPanel::OnDeleteButton )
  EVT_BUTTON( ID_USER_BUTTON, ManageGroupsPanel::OnUserButton )
END_EVENT_TABLE()

ManageGroupsPanel::ManageGroupsPanel( wxWindow* parent )
    : wxScrolledWindow( parent, -1 ), m_main_sizer(0),
      m_groups_sizer(0), m_newgroup(0), m_user_dialog(0)
{
    SetScrollRate(0,10);
    SetupControls();
}

ManageGroupsPanel::~ManageGroupsPanel()
{
    if ( m_user_dialog )
        delete m_user_dialog;
}

wxSizer* ManageGroupsPanel::GetGroupSizer( const wxString& group )
{
    wxStaticBoxSizer* stBox = new wxStaticBoxSizer ( wxVERTICAL, this, _("Group ") + group );
    wxBoxSizer* gBox = new wxBoxSizer( wxHORIZONTAL );
//    wxBoxSizer* nameBox = new wxBoxSizer( wxVERTICAL );
//    wxStaticText* name = new wxStaticText( this, -1, group );
//    nameBox->Add( name );
//    gBox->Add( nameBox, 0, wxALL|wxEXPAND, 10 );

    wxBoxSizer* actionsBox = new wxBoxSizer( wxVERTICAL );
    wxStaticText* aLabel = new wxStaticText( this, -1, _("Select actions") );
    actionsBox->Add( aLabel, 0, wxBOTTOM, 5);
    for ( int i = 1; i < UserActions::m_numActions; ++i)
    {
        //encoding the actionenum value in the id
        int id = (int) std::pow( 2.0, i);
        wxCheckBox* chk = new wxCheckBox( this, id, m_actionNames[i], wxDefaultPosition,
            wxDefaultSize, 0, wxDefaultValidator, group );
        UserActions::ActionType act = useractions().GetGroupAction( group );
        UserActions::ActionType act1 = ( (UserActions::ActionType) id );
        if ( act != UserActions::ActNone )
            chk->SetValue( ( act &  act1 ) != 0 );
        actionsBox->Add( chk );
    }
    gBox->Add( actionsBox, 0, wxALL|wxEXPAND, 10 );

    wxBoxSizer* secondColum = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* colorBox = new wxBoxSizer( wxHORIZONTAL );
    wxStaticText* cLabel = new wxStaticText( this, -1, _("Highlight color") );
    colorBox->Add( cLabel,0, wxBOTTOM, 5 );

    int id = ID_COLOR_BUTTON;

    static unsigned int idcount = 0;
    id += idcount;
    idcount++;
    Connect( id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ManageGroupsPanel::OnColorButton ) );
    ColorButton* m_color = new ColorButton( this, id, useractions().GetGroupColor(group), wxDefaultPosition, wxSize( 20,20 ),
                                        wxBU_AUTODRAW,wxDefaultValidator, group );
    colorBox->Add( m_color,0, wxLEFT|wxBOTTOM, 5 );
    wxButton* userButton = new wxButton ( this, ID_USER_BUTTON, _("add/remove users"),wxDefaultPosition, wxSize( -1,30 ),
                                        0,wxDefaultValidator, group  );
    wxButton* deleteButton = new wxButton ( this, ID_DEL_BUTTON, _("delete group"),wxDefaultPosition, wxSize( -1,30 ),
                                        0,wxDefaultValidator, group  );

    secondColum->Add( colorBox, 1, wxALL|wxEXPAND, 10 );
    secondColum->Add( userButton, 0 , wxALL|wxEXPAND, 4 );
    secondColum->Add( deleteButton, 0 , wxALL|wxEXPAND, 4 );
    gBox->Add( secondColum, 1,wxLEFT|wxEXPAND, 25);
    stBox->Add( gBox, 0, wxALL|wxEXPAND, 5) ;
    return stBox;

}

void ManageGroupsPanel::SetupControls()
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_main_sizer->Clear();

    //add group controls
    wxStaticBoxSizer* stAddBox = new wxStaticBoxSizer ( wxVERTICAL, this, _("new group") );
    wxBoxSizer* addBox = new wxBoxSizer( wxHORIZONTAL );
    wxStaticText* addText = new wxStaticText( this, -1, _("New group:") );
    m_newgroup = new wxTextCtrl ( this, -1 );
    wxButton* addButton = new wxButton( this, ID_ADD_BUTTON, _("add") );
    addBox->Add( addText, 0, wxLEFT|wxRIGHT, 15);
    addBox->Add( m_newgroup, 0, wxLEFT|wxRIGHT, 15);
    addBox->Add( addButton, 0, wxLEFT|wxRIGHT, 15);
    stAddBox->Add( addBox,0, wxALL, 5);
    m_main_sizer->Add( stAddBox, 0, wxALL, 10 );

    //sizers for all groups
    ReloadGroupSizer();

    SetSizer( m_main_sizer );
    this->Layout();
}

void ManageGroupsPanel::OnColorButton( wxCommandEvent& event )
{
    ColorButton* origin = (ColorButton*) event.GetEventObject();
    wxString group = origin->GetName();
    wxColour c = GetColourFromUser( this, origin->GetColor(), group );
    //if ( c.IsOk() )

    {

        origin->SetColor( c );
        useractions().SetGroupColor( group, c );
    }
    #ifdef __WXMSW__
        ReloadGroupSizer();
        Refresh();

    #endif
}

void ManageGroupsPanel::OnAddButton( wxCommandEvent& event )
{
    wxString newgroup = m_newgroup->GetValue();
    useractions().AddGroup( newgroup );
    m_newgroup->SetValue(wxEmptyString);
    ReloadGroupSizer();
}

void ManageGroupsPanel::OnDeleteButton( wxCommandEvent& event )
{
    wxString newgroup = ( (wxButton*)event.GetEventObject() )->GetName();
    useractions().DeleteGroup( newgroup );
    ReloadGroupSizer();
}

void ManageGroupsPanel::OnCheckBox( wxCommandEvent& event )
{
    UserActions::ActionType action = (UserActions::ActionType) event.GetId();
    wxCheckBox* chkBox = (wxCheckBox*) event.GetEventObject();
    wxString group = chkBox->GetName();
    useractions().ChangeAction( group, action, chkBox->IsChecked() );
}

void ManageGroupsPanel::ReloadGroupSizer()
{
    if ( m_groups_sizer )
    {
        m_groups_sizer->Clear( true );
        m_main_sizer->Remove( m_groups_sizer );
    }
    m_groups_sizer = new wxBoxSizer ( wxVERTICAL );
    wxSortedArrayString groupnames = useractions().GetGroupNames();
    for ( unsigned int i = 0; i < groupnames.GetCount(); ++i)
    {
        m_groups_sizer->Add( GetGroupSizer( groupnames[i] ),1, wxEXPAND|wxALL, 10 );
    }
    m_main_sizer->Add( m_groups_sizer );
    this->SetSizerAndFit( m_main_sizer );
    this->Layout();
}

void ManageGroupsPanel::OnUserButton( wxCommandEvent& event )
{
    if ( m_user_dialog && m_user_dialog->IsShown())
    {
            //display err
    }
    else
    {
        wxString group = ( (wxButton*)event.GetEventObject() )->GetName();
        m_user_dialog = new GroupUserDialog( this, -1, _(""), group, wxDefaultPosition, wxSize( 800,800) );
        m_user_dialog->Show( true );
    }
}
