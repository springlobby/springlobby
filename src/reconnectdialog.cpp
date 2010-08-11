#include "reconnectdialog.h"

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "ui.h"
#include "mainwindow.h"
#include "uiutils.h"
#include "utils/controls.h"
#include "utils/customdialogs.h"

BEGIN_EVENT_TABLE( ReconnectDialog, wxDialog )

	EVT_BUTTON  ( CONNECT_RECONNECT, 		ReconnectDialog::OnReconnect    		)
	EVT_BUTTON  ( CONNECT_NEXT,     		ReconnectDialog::OnCancel        		)
	EVT_BUTTON  ( CONNECT_CANCEL, 		    ReconnectDialog::OnTryNext	         	)

END_EVENT_TABLE()

ReconnectDialog::ReconnectDialog()
: wxDialog( (wxFrame*)&ui().mw(), -1, _( "Reconnect Dialog" ), wxDefaultPosition, wxSize( 400, 150 ), wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE ),
  WindowAttributesPickle( _T("reconnectdialog"), this, wxSize( 400, 150 ) )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_static_text = new wxStaticText( this, wxID_ANY, _( "Couldn't estabilish a connection with the server\nWhat would you like to do?" ), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	wxButton* m_retry_btn = new wxButton( this, CONNECT_RECONNECT, _( "Try Reconnecting" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_retry_btn->SetToolTip( TE( _( "Tries to connect to the next server in the list." ) ) );
	m_buttons_sizer->Add( m_retry_btn, 0, wxALL, 5 );

	wxButton* m_next_btn = new wxButton( this, CONNECT_NEXT, _( "Try Next Server" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_next_btn->SetToolTip( TE( _( "Tries to connect to the next server in the list." ) ) );
	m_buttons_sizer->Add( m_next_btn, 0, wxALL, 5 );

	wxButton* m_cancel_btn = new wxButton( this, CONNECT_CANCEL, _( "Do nothing" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_cancel_btn, 0, wxALL, 5 );

	m_main_sizer->Add( m_static_text, 0, wxEXPAND | wxHORIZONTAL, 5 );
	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

	this->SetSizer( m_main_sizer );
	this->Layout();

	m_next_btn->SetFocus();
}


void ReconnectDialog::OnReconnect ( wxCommandEvent& /*event*/ )
{
	EndModal( wxID_YES );
}

void ReconnectDialog::OnCancel ( wxCommandEvent& /*event*/ )
{
	EndModal( wxID_CANCEL );
}

void ReconnectDialog::OnTryNext ( wxCommandEvent& /*event*/ )
{
	EndModal( wxID_NO );
}
