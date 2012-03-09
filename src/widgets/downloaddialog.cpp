#include "downloaddialog.h"

#include <wx/sizer.h>

#include "downloadpanel.h"
#include "../settings.h"

WidgetDownloadDialog::WidgetDownloadDialog(wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
	: wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style, _T("WIDGETDIALOG") ),
	WindowAttributesPickle( _T("WIDGETDIALOG"), this, wxSize( 670, 400 ) )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    m_widgets_panel = new WidgetDownloadPanel( this, -1, _("widgets") );
    m_main_sizer->Add( m_widgets_panel, 1, wxALL | wxEXPAND, 5 );

    SetSizer( m_main_sizer );
    Layout();
}

WidgetDownloadDialog::~WidgetDownloadDialog()
{

}

bool WidgetDownloadDialog::Show( bool show )
{
    return wxDialog::Show( show );
}
