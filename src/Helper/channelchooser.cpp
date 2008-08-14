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

    for ( int i = 0; i < ui().GetServer().GetNumChannels(); ++i )
    {
        m_channellist->AddChannel( ui().GetServer().GetChannel(i).GetName(),
                                   ui().GetServer().GetChannel(i).GetNumUsers() );
    }
    m_channellist->AddChannel( _T("sltest"), 9231 );

    m_main_sizer->Add ( m_channellist, 1, wxALL|wxEXPAND, 15 );
}

ChannelChooser::~ChannelChooser()
{
    //dtor
}
