#include "managegroupspanel.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include "useractions.hh"
#include "ui.h"

BEGIN_EVENT_TABLE( ManageGroupsPanel, wxScrolledWindow )
  EVT_CHECKBOX( wxID_ANY, ManageGroupsPanel::OnCheckBox )
  EVT_BUTTON( ID_COLOR_BUTTON, ManageGroupsPanel::OnColorButton )
END_EVENT_TABLE()

ManageGroupsPanel::ManageGroupsPanel( wxWindow* parent )
    : wxScrolledWindow( parent, -1 )
{
    SetupControls();
}

ManageGroupsPanel::~ManageGroupsPanel()
{

}

wxSizer* ManageGroupsPanel::GetGroupSizer( const wxString& group )
{
    wxBoxSizer* gBox = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* nameBox = new wxBoxSizer( wxVERTICAL );
    wxStaticText* name = new wxStaticText( this, -1, group );
    nameBox->Add( name );
    gBox->Add( nameBox );

    wxBoxSizer* actionsBox = new wxBoxSizer( wxVERTICAL );
    for ( int i = 1; i < UserActions::m_numActions; ++i)
    {
        wxCheckBox* chk = new wxCheckBox( this, wxID_ANY, m_actionNames[i], wxDefaultPosition,
            wxDefaultSize, 0, wxDefaultValidator, group + m_actionNames[i]);
        actionsBox->Add( chk );
    }
    gBox->Add( actionsBox );

    wxBoxSizer* colorBox = new wxBoxSizer( wxHORIZONTAL );
    wxStaticText* cLabel = new wxStaticText( this, -1, _T("Highlight color") );
    colorBox->Add( cLabel );
    wxButton* m_color = new wxButton( this, ID_COLOR_BUTTON, wxEmptyString, wxDefaultPosition, wxSize( 20,20 ), 0,wxDefaultValidator, group );
    m_color->SetBackgroundColour( wxColour( 255, 0, 0 ) );
    colorBox->Add( m_color );
    gBox->Add( colorBox );

    return gBox;

}

void ManageGroupsPanel::SetupControls()
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );
    //UserActions& useractions = ui().GetGroupNames();
    wxSortedArrayString groupnames = useractions().GetGroupNames();
    for ( unsigned int i = 0; i < groupnames.GetCount(); ++i)
    {
        m_main_sizer->Add( GetGroupSizer( groupnames[i] ) );
    }

    SetSizer( m_main_sizer );
}

void ManageGroupsPanel::OnColorButton( wxCommandEvent& event )
{

}

void ManageGroupsPanel::OnCheckBox( wxCommandEvent& event )
{

}
