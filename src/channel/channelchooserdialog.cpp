#include "channelchooserdialog.h"

#include <wx/sizer.h>
#include "channelchooser.h"

ChannelChooserDialog::ChannelChooserDialog(wxWindow* parent, wxWindowID id,
            const wxString& title, long style, const wxPoint& pos,
            const wxSize& size )
    : wxDialog ( parent, id, title, pos, size, style )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    m_chooser_panel = new ChannelChooserPanel( this, -1, _("Choose channels to join") );
    m_main_sizer->Add( m_chooser_panel, 1, wxALL | wxEXPAND, 5 );

    SetSizer( m_main_sizer );
}

ChannelChooserDialog::~ChannelChooserDialog()
{
    //dtor
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
