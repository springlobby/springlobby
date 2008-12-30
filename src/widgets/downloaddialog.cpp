#include "downloaddialog.h"

#include <wx/sizer.h>

#include "downloadpanel.h"

WidgetDownloadDialog::WidgetDownloadDialog(wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style )
{
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    m_widgets_panel = new WidgetDownloadPanel( this, -1, _("widgets") );
    m_main_sizer->Add( m_widgets_panel, 1, wxALL | wxEXPAND, 5 );

    SetSizer( m_main_sizer );

//    wxString name = _T("CHANNELCHOOSER");
//    wxPoint pos = sett().GetWindowPos( name, wxDefaultPosition );
//    wxSize size = sett().GetWindowSize( name, wxSize( 470, 400 ) );
//    SetSize( pos.x , pos.y, size.GetWidth(), size.GetHeight() );
//    Layout();
}

WidgetDownloadDialog::~WidgetDownloadDialog()
{
    //dtor
}
