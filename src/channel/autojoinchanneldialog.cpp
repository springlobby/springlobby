#include "autojoinchanneldialog.h"


#include <wx/intl.h>
#include <wx/datetime.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/tokenzr.h>
#include <wx/event.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include "../settings.h"

BEGIN_EVENT_TABLE( AutojoinChannelDialog, wxDialog )

	EVT_BUTTON( wxID_OK,     AutojoinChannelDialog::OnOk )
	EVT_BUTTON( wxID_CANCEL, AutojoinChannelDialog::OnCancel )


END_EVENT_TABLE()

AutojoinChannelDialog::AutojoinChannelDialog( wxWindow* parent )
		: wxDialog( parent, -1, _( "Edit auto-joined channels" ), wxDefaultPosition, wxSize( 400, 500 ),
		            wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN )
{
	wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* textSizer = new wxBoxSizer( wxVERTICAL );
	m_channel_list = new wxTextCtrl( this, wxID_ANY, _T( "" ), wxDefaultPosition, wxDefaultSize,
	                                 wxTE_MULTILINE | wxTE_DONTWRAP );
	wxSizer *buttonSizer= CreateButtonSizer( wxOK|wxCANCEL|wxALIGN_CENTRE );
	wxStaticLine* seperator = new wxStaticLine( this );
	wxStaticText* hint =    new wxStaticText( this, -1, _( "Add one channel per line like this:\n"
	        "channelname password\n(passwords for existing channels are not displayed)" ) );
	textSizer->Add( hint, 0, wxEXPAND|wxALL|wxALIGN_CENTRE, 10 );
	textSizer->Add( seperator );
	textSizer->Add( m_channel_list, 1,  wxEXPAND|wxALL|wxALIGN_CENTRE, 10 );
	mainSizer->Add( textSizer,1,  wxEXPAND|wxALL|wxALIGN_CENTRE, 10 );
	mainSizer->Add( seperator );
	mainSizer->Add( buttonSizer, 0,  wxALL|wxALIGN_CENTRE, 10 );
	SetSizer( mainSizer );
	wxString channels;
	for ( int i = 0; i < sett().GetNumChannelsJoin(); ++i )
	{
		channels << sett().GetChannelJoinName( i ) + wxT( "\n" );
	}
	m_channel_list->SetValue( channels );
	m_old_channels = channels;
}

AutojoinChannelDialog::~AutojoinChannelDialog()
{
	//dtor
}


void AutojoinChannelDialog::OnOk( wxCommandEvent& event )
{
    wxString newChannels = m_channel_list->GetValue();

    //add new channels
	wxStringTokenizer tokenList( newChannels, _T( "\n" ) );
	while ( tokenList.HasMoreTokens() )
	{
		wxString line = tokenList.GetNextToken();
		wxString chan = line.BeforeFirst( *wxT( " " ) );
		wxString key = line.AfterFirst( *wxT( " " ) );
		sett().AddChannelJoin( chan, key );
	}

	//remove old channels
	wxStringTokenizer oldList( m_old_channels, _T( "\n" ) );
	while ( oldList.HasMoreTokens() )
	{
		wxString line = oldList.GetNextToken();
		wxString chan = line.BeforeFirst( *wxT( " " ) );
		wxString key = line.AfterFirst( *wxT( " " ) );
		if ( !newChannels.Contains(chan) )
            sett().RemoveChannelJoin(chan);
	}
	m_old_channels = newChannels;
	this->Show( false );
}

void AutojoinChannelDialog::OnCancel( wxCommandEvent& event )
{
	this->Show( false );
}


