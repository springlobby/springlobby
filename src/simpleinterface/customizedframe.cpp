#include "customizedframe.h"
#include "../customizations.h"
#include "../gui/wxbackgroundimage.h"
CustomizedFrame::CustomizedFrame( wxWindow* parent )
	: wxFrame(parent, wxID_ANY, _T(""), wxDefaultPosition, wxSize( -1,-1 ), wxCAPTION|wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL )
{
	SetIcon( SLcustomizations().GetAppIcon() );
	SetTitle( SLcustomizations().GetModname() );

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxFRAME_EX_METAL );
	SetSize( SLcustomizations().GetBackgroundSize() );
	Layout();
	PushEventHandler( new wxBackgroundBitmap( SLcustomizations().GetBackground() ) );
}
