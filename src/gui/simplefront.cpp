#include "simplefront.h"

#include "wxbackgroundimage.h"
#include "../uiutils.h"
#include "../images/s44.png.h"
#include "../ui.h"
#include "../mainwindow.h"
#include "../settings.h"
#include "../settings++/frame.h"
#include "skirmish_dialog.h"

#include <wx/app.h>

SimpleFront::SimpleFront( wxWindow* parent,const wxString& modname )
: SimpleFrontBase( parent ),
m_settings( 0 ),
m_skirmish( 0 ),
m_modname( modname )
{
	m_mod_customs.loadOptions( OptionsWrapper::ModCustomizations, m_modname );

    PushEventHandler(
        new wxBackgroundBitmap(
            charArr2wxBitmap( s44_png, sizeof( s44_png ) )
            )
        );

}

void SimpleFront::OnSingleplayer( wxCommandEvent& event )
{
	m_skirmish = new SkirmishDialog( this, m_modname, m_mod_customs );
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
