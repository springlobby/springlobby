#include "channelchooser.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>

#include "channellistctrl.h"
#include "server.h"
#include "ui.h"
#include "channel.h"

ChannelChooser::ChannelChooser(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos , const wxSize& size , long style , const wxString& name)
    : wxDialog (parent,  id, title, pos, size, style, name)
{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    m_channellist = new ChannelListctrl ( this, -1, _T("channellistctrl") );


    m_main_sizer->Add ( m_channellist, 1, wxALL|wxEXPAND, 15 );
}

ChannelChooser::~ChannelChooser()
{
    //dtor
}

void ChannelChooser::AddChannel( const wxString& name, int usercount, const wxString& topic )
{
    m_channellist->AddChannel( name, usercount, topic );
}

void ChannelChooser::ClearChannels()
{
    m_channellist->ClearChannels();
}
