#include "simplefront.h"

#include "wxbackgroundimage.h"
#include "../uiutils.h"
#include "../images/s44.png.h"
#include "../ui.h"
#include "../springunitsync.h"
#include "../mainwindow.h"
#include "../settings.h"
#include "../settings++/frame.h"
#include "skirmish_dialog.h"

#include <wx/app.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/frame.h>

SimpleFront::SimpleFront( wxWindow* parent,const wxString& modname )
: wxFrame( parent, wxID_ANY, modname, wxDefaultPosition, wxSize( -1,-1 ), wxCAPTION|wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL ),
m_settings_frame( 0 ),
m_skirmish( 0 ),
m_modname( modname )
{
    m_mod_customs.loadOptions( OptionsWrapper::ModCustomizations, m_modname );

	wxString icon_img_path = m_mod_customs.getSingleValue( _T("icon") );
	wxBitmap icon_bmp (usync().GetImage( m_modname, icon_img_path ) );
	m_frame_ico.CopyFromBitmap( icon_bmp );
    SetIcon( m_frame_ico );

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxFRAME_EX_METAL );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );


	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_sp = new wxButton( this, wxID_ANY, _("Singleplayer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sp->SetDefault();
	m_button_sizer->Add( m_sp, 0, wxALL, 5 );

	m_mp = new wxButton( this, wxID_ANY, _("Multiplayer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_mp, 0, wxALL, 5 );

	m_settings = new wxButton( this, wxID_ANY, _("Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_settings, 0, wxALL, 5 );

	m_exit = new wxButton( this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_sizer->Add( m_exit, 0, wxALL, 5 );

	bSizer1->Add( m_button_sizer, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_skirmish_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_skirmish = new SkirmishDialog( this, m_frame_ico, m_bg_img, m_modname, m_mod_customs );
	m_skirmish_sizer->Add( m_skirmish, 0, wxALL, 0 );
	m_skirmish_sizer->Show( false );
	bSizer1->Add( m_skirmish_sizer, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_sp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSingleplayer ), NULL, this );
	m_mp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnMultiplayer ), NULL, this );
	m_settings->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSettings ), NULL, this );
	m_exit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnExit ), NULL, this );

    wxString bg_img_path = m_mod_customs.getSingleValue( _T("bg_image") );
    m_bg_img = wxBitmap( usync().GetImage( m_modname, bg_img_path ) );
    SetSize( m_bg_img.GetWidth(), m_bg_img.GetHeight() );
    Layout();
    PushEventHandler( new wxBackgroundBitmap( m_bg_img ) );

}

SimpleFront::~SimpleFront()
{
    m_sp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSingleplayer ), NULL, this );
	m_mp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnMultiplayer ), NULL, this );
	m_settings->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnSettings ), NULL, this );
	m_exit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleFront::OnExit ), NULL, this );
}

void SimpleFront::OnSingleplayer( wxCommandEvent& event )
{
    ShowSP( true );
}

void SimpleFront::ShowSP( bool show )
{
    m_button_sizer->Show( !show );
	m_skirmish_sizer->Show( show );
	Layout();
}

void SimpleFront::OnMultiplayer( wxCommandEvent& event )
{
	ui().mw().ShowTab( MainWindow::PAGE_JOIN );
	Close();
}

void SimpleFront::OnSettings( wxCommandEvent& event )
{
//	if ( !m_settings ) //TODO cleanup the exit mess in SS
        m_settings_frame = new settings_frame( this, wxID_ANY, wxT("SpringSettings"),
                                wxDefaultPosition, wxDefaultSize );

    m_settings_frame->Show( true );

}

void SimpleFront::OnExit( wxCommandEvent& event )
{
	Destroy();
}

void SimpleFront::OnSkipChecked( wxCommandEvent& event )
{
	// TODO: save in settings
}

void SimpleFront::Close()
{
    ui().ShowMainWindow();
    wxTheApp->SetTopWindow( &ui().mw() );
    Destroy();
}
