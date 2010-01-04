#include "statusbar.h"

Statusbar::Statusbar( wxWindow* parent )
	:wxStatusBar( parent, wxNewId() )
{
	int w[1] = {-1};
	SetFieldsCount( 1, w );
	PushStatusText( _("app init") );
}
