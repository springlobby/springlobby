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
#include "ui.h"
#include <cmath>

BEGIN_EVENT_TABLE( ManageGroupsPanel, wxScrolledWindow )
  EVT_CHECKBOX( wxID_ANY, ManageGroupsPanel::OnCheckBox )
  EVT_BUTTON( ID_COLOR_BUTTON, ManageGroupsPanel::OnColorButton )
  EVT_BUTTON( ID_ADD_BUTTON, ManageGroupsPanel::OnAddButton )
END_EVENT_TABLE()

ManageGroupsPanel::ManageGroupsPanel( wxWindow* parent )
    : wxScrolledWindow( parent, -1 ), m_main_sizer(0),
      m_groups_sizer(0), m_newgroup(0)
{
    SetScrollRate(0,10);
    SetupControls();
}

ManageGroupsPanel::~ManageGroupsPanel()
{

}

wxSizer* ManageGroupsPanel::GetGroupSizer( const wxString& group )
{
    wxStaticBoxSizer* stBox = new wxStaticBoxSizer ( wxVERTICAL, this );
    wxBoxSizer* gBox = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* nameBox = new wxBoxSizer( wxVERTICAL );
    wxStaticText* name = new wxStaticText( this, -1, group );
    nameBox->Add( name );
    gBox->Add( nameBox, 0, wxALL|wxEXPAND, 10 );

    wxBoxSizer* actionsBox = new wxBoxSizer( wxVERTICAL );
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

    wxBoxSizer* colorBox = new wxBoxSizer( wxHORIZONTAL );
    wxStaticText* cLabel = new wxStaticText( this, -1, _("Highlight color") );
    colorBox->Add( cLabel );
    wxButton* m_color = new wxButton( this, ID_COLOR_BUTTON, wxEmptyString, wxDefaultPosition, wxSize( 20,20 ),
                                        0,wxDefaultValidator, group );
    m_color->SetBackgroundColour( wxColour( 255, 0, 0 ) );
    colorBox->Add( m_color );
    gBox->Add( colorBox, 0, wxALL|wxEXPAND, 10 );
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
    stAddBox->Add( addBox, 0, wxALL|wxEXPAND, 5);
    m_main_sizer->Add( stAddBox, 0, wxEXPAND|wxALL, 10 );

    //sizers for all groups
    ReloadGroupSizer();

    SetSizer( m_main_sizer );
    this->Layout();
}

void ManageGroupsPanel::OnColorButton( wxCommandEvent& event )
{
    wxButton* origin = (wxButton*) event.GetEventObject();
    wxString group = origin->GetName();
    wxColourData c;
    c.SetColour( origin->GetBackgroundColour() );
    wxColourDialog dlg( this, &c );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColor color = dlg.GetColourData().GetColour();
        origin->SetBackgroundColour( color );
        useractions().SetGroupColor( group, color );
    }
}

void ManageGroupsPanel::OnAddButton( wxCommandEvent& event )
{
    wxString newgroup = m_newgroup->GetValue();
    useractions().AddGroup( newgroup );
    SetupControls();
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
        m_groups_sizer->Add( GetGroupSizer( groupnames[i] ),0, wxEXPAND|wxALL, 10 );
    }
    m_main_sizer->Add( m_groups_sizer );
    this->SetSizerAndFit( m_main_sizer );
    this->Layout();
}
