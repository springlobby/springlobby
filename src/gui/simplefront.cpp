#include "simplefront.h"

#include "wxbackgroundimage.h"
#include "../uiutils.h"
#include "../ui.h"
#include "../springunitsync.h"
#include "../springunitsynclib.h"
#include "../mainwindow.h"
#include "../settings.h"
#include "../settings++/frame.h"
#include "skirmish_dialog.h"
#include "../customizations.h"

#include <wx/app.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include "wxgradientbutton.h"
#include "../customizations.h"
#include "../simpleinterface/simplempframe.h"

SimpleFront::SimpleFront( wxWindow* parent )
: CustomizedFrame( parent ),
m_settings_frame( 0 ),
m_skirmish( 0 )
{
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_sp = new wxGradientButton( this, wxID_ANY, _("Skirmish"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_sp->SetDefault();
	m_button_sizer->Add( m_sp, 0, wxALL, 5 );

	m_mp = new wxGradientButton( this, wxID_ANY, _("Multiplayer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_mp, 0, wxALL, 5 );

	m_settings = new wxGradientButton( this, wxID_ANY, _("Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_settings, 0, wxALL, 5 );

    m_help = new wxGradientButton( this, wxID_ANY, _("Help"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_help, 0, wxALL, 5 );
    m_help->Show( !SLcustomizations().GetHelpUrl().IsEmpty() );

	m_exit = new wxGradientButton( this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_exit, 0, wxALL, 5 );

	bSizer1->Add( m_button_sizer, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

    m_sp_button_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_back = new wxGradientButton( this, wxID_ANY, _("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sp_button_sizer->Add( m_back, 0, wxALL, 5 );

	m_advanced = new wxGradientButton( this, wxID_ANY, _("Advanced setup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sp_button_sizer->Add( m_advanced, 0, wxALL, 5 );

	m_start = new wxGradientButton( this, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sp_button_sizer->Add( m_start, 0, wxALL, 5 );
    m_sp_button_sizer->Show( false );

    bSizer1->Add( m_sp_button_sizer, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_skirmish_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_skirmish = new SkirmishDialog( this, -1 );
	m_skirmish_sizer->Add( m_skirmish, 0, wxALIGN_BOTTOM, 0 );
	m_skirmish_sizer->Show( false );

	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_sp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSingleplayer ), NULL, this );
	m_mp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnMultiplayer ), NULL, this );
	m_settings->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSettings ), NULL, this );
	m_help->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnHelp ), NULL, this );
	m_exit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnExit ), NULL, this );

    m_back->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, m_skirmish );
	m_advanced->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, m_skirmish );
	m_start->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnStart ), NULL, m_skirmish );

	Layout();

}

SimpleFront::~SimpleFront()
{
    m_sp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSingleplayer ), NULL, this );
	m_mp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnMultiplayer ), NULL, this );
	m_settings->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSettings ), NULL, this );
	m_help->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnHelp ), NULL, this );
	m_exit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnExit ), NULL, this );

    m_back->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnBack ), NULL, m_skirmish );
	m_advanced->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnAdvanced ), NULL, m_skirmish );
	m_start->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SkirmishDialog::OnStart ), NULL, m_skirmish );

}

void SimpleFront::OnSingleplayer( wxCommandEvent&  )
{
    ShowSP( true );
}

void SimpleFront::ShowSP( bool show )
{
    m_button_sizer->Show( !show );
	m_skirmish_sizer->Show( show );
	m_sp_button_sizer->Show( show );
    bSizer1->Detach( 3 );
    if ( show )
        bSizer1->Add( m_skirmish_sizer, 1, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 0 );
    else
        bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	Layout();
}

void SimpleFront::OnMultiplayer( wxCommandEvent&  )
{
	SimpleMPFrame* f = new SimpleMPFrame( this );
	f->Show(true);
}

void SimpleFront::OnSettings( wxCommandEvent&  )
{
//	if ( !m_settings ) //TODO cleanup the exit mess in SS
        m_settings_frame = new settings_frame( this, wxID_ANY, wxT("SpringSettings"),
                                wxDefaultPosition, wxDefaultSize );

    m_settings_frame->Show( true );

}

void SimpleFront::OnExit( wxCommandEvent&  )
{
	Destroy();
}

void SimpleFront::OnSkipChecked( wxCommandEvent&  )
{
	// TODO: save in settings
}

void SimpleFront::Close()
{
    ui().ShowMainWindow();
    wxTheApp->SetTopWindow( &ui().mw() );
    Destroy();
}

void SimpleFront::OnHelp( wxCommandEvent&  )
{
    OpenWebBrowser( SLcustomizations().GetHelpUrl() );

}

