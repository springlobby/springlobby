#include "downloaddialog.h"

#include <wx/sizer.h>

#include "downloadpanel.h"
#include "../settings.h"

WidgetDownloadDialog::WidgetDownloadDialog(wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style, _T("WIDGETDIALOG") )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    m_widgets_panel = new WidgetDownloadPanel( this, -1, _("widgets") );
    m_main_sizer->Add( m_widgets_panel, 1, wxALL | wxEXPAND, 5 );

    SetSizer( m_main_sizer );

    wxString name = GetName();
    wxPoint pos = sett().GetWindowPos( name, wxDefaultPosition );
    wxSize size = sett().GetWindowSize( name, wxSize( 670, 400 ) );
    SetSize( pos.x , pos.y, size.GetWidth(), size.GetHeight() );
    Layout();
}

WidgetDownloadDialog::~WidgetDownloadDialog()
{

}

bool WidgetDownloadDialog::Show( bool show )
{
    if ( !show ) {
        sett().SetWindowSize( GetName(), GetSize() );
        sett().SetWindowPos( GetName(), GetPosition() );
    }
    return wxDialog::Show( show );
}
