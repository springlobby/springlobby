#include "channelchooserdialog.h"

#include <wx/sizer.h>
#include "channelchooser.h"
#include "../settings.h"

ChannelChooserDialog::ChannelChooserDialog(wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    m_chooser_panel = new ChannelChooserPanel( this, -1, _("Choose channels to join") );
    m_main_sizer->Add( m_chooser_panel, 1, wxALL | wxEXPAND, 5 );

    SetSizer( m_main_sizer );

    wxString name = _T("CHANNELCHOOSER");
    wxPoint pos = sett().GetWindowPos( name, wxDefaultPosition );
    wxSize size = sett().GetWindowSize( name, wxSize( 470, 400 ) );
    SetSize( pos.x , pos.y, size.GetWidth(), size.GetHeight() );
    Layout();
}

ChannelChooserDialog::~ChannelChooserDialog()
{
    wxString name = _T("CHANNELCHOOSER");
    sett().SetWindowSize( name, GetSize() );
    sett().SetWindowPos( name, GetPosition() );
    sett().SaveSettings();
}

void ChannelChooserDialog::AddChannel( const wxString& name, int usercount, const wxString& topic )
{
    m_chooser_panel->AddChannel( name, usercount, topic );
    m_main_sizer->FitInside( this );
    Layout();
}

void ChannelChooserDialog::ClearChannels()
{
    m_chooser_panel->ClearChannels();
}
