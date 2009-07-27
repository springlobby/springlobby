#include "simplefront.h"

#include "wxbackgroundimage.h"
#include "../uiutils.h"
#include "images/s44.png.h"

SimpleFront::SimpleFront( wxWindow* parent )
:
SimpleFrontBase( parent )
{
    PushEventHandler(
        new wxBackgroundBitmap(
            charArr2wxBitmap( s44_png, sizeof( s44_png ) )
            )
        );
}

void SimpleFront::OnSingleplayer( wxCommandEvent& event )
{
	// TODO: Implement OnSingleplayer
}

void SimpleFront::OnMultiplayer( wxCommandEvent& event )
{
	// TODO: Implement OnMultiplayer
}

void SimpleFront::OnSettings( wxCommandEvent& event )
{
	// TODO: Implement OnSettings
}

void SimpleFront::OnExit( wxCommandEvent& event )
{
	// TODO: Implement OnExit
}

void SimpleFront::OnSkipChecked( wxCommandEvent& event )
{
	// TODO: Implement OnSkipChecked
}
