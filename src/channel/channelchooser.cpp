#include "channelchooser.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>

#include "channellistctrl.h"
#include "server.h"
#include "ui.h"
#include "channel.h"

ChannelChooserPanel::ChannelChooserPanel(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style , const wxString& name)
    : wxScrolledWindow (parent,  id, pos, size, style, name)
{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    m_channellist = new ChannelListctrl ( this, -1, _T("channellistctrl") );


    m_main_sizer->Add ( m_channellist, 1, wxALL|wxEXPAND, 5 );
    SetSizer( m_main_sizer );
}

ChannelChooserPanel::~ChannelChooserPanel()
{
    //dtor
}

void ChannelChooserPanel::AddChannel( const wxString& name, int usercount, const wxString& topic )
{
    m_channellist->AddChannel( name, usercount, topic );
    Layout();
}

void ChannelChooserPanel::ClearChannels()
{
    m_channellist->ClearChannels();
}
