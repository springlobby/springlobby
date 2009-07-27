#include "simplefront.h"

#include "wxbackgroundimage.h"
#include "../uiutils.h"
#include "../images/s44.png.h"
#include "../ui.h"
#include "../mainwindow.h"
#include "../settings.h"
#include "../settings++/frame.h"

#include <wx/app.h>

SimpleFront::SimpleFront( wxWindow* parent )
: SimpleFrontBase( parent ),
m_settings( 0 )
{
    PushEventHandler(
        new wxBackgroundBitmap(
            charArr2wxBitmap( s44_png, sizeof( s44_png ) )
            )
        );
}

void SimpleFront::OnSingleplayer( wxCommandEvent& event )
{
	ui().mw().ShowSingleplayer();
	Close();
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
