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

SimpleFront::SimpleFront( wxWindow* parent,const wxString& modname )
: SimpleFrontBase( parent, wxID_ANY, modname ),
m_settings( 0 ),
m_skirmish( 0 ),
m_modname( modname )
{
	m_mod_customs.loadOptions( OptionsWrapper::ModCustomizations, m_modname );

	wxString icon_img_path = m_mod_customs.getSingleValue( _T("icon") );
	wxBitmap icon_bmp (usync().GetImage( m_modname, icon_img_path ) );
	m_frame_ico.CopyFromBitmap( icon_bmp );
    SetIcon( m_frame_ico );

    wxString bg_img_path = m_mod_customs.getSingleValue( _T("bg_image") );
    m_bg_img = wxBitmap( usync().GetImage( m_modname, bg_img_path ) );
    SetSize( m_bg_img.GetWidth(), m_bg_img.GetHeight() );
    Layout();
    PushEventHandler( new wxBackgroundBitmap( m_bg_img ) );

}

void SimpleFront::OnSingleplayer( wxCommandEvent& event )
{
	m_skirmish = new SkirmishDialog( this, m_frame_ico, m_bg_img, m_modname, m_mod_customs );
	m_skirmish->Show();
}

void SimpleFront::OnMultiplayer( wxCommandEvent& event )
{
	ui().mw().ShowTab( MainWindow::PAGE_JOIN );
	Close();
}

void SimpleFront::OnSettings( wxCommandEvent& event )
{
//	if ( !m_settings ) //TODO cleanup the exit mess in SS
        m_settings = new settings_frame( this, wxID_ANY, wxT("SpringSettings"),
                                wxDefaultPosition, wxDefaultSize );

    m_settings->Show( true );

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
