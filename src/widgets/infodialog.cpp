#include "infodialog.h"
#include "infopanel.h"

#include <wx/sizer.h>

WidgetInfoDialog::WidgetInfoDialog(Widget& widget, wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style )
{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    m_panel = new WidgetInfoPanel( widget, this, -1 );
    m_main_sizer->Add( m_panel, 1, wxALL|wxEXPAND, 5 );
    SetSizer( m_main_sizer );
    Layout();
}

WidgetInfoDialog::~WidgetInfoDialog()
{
    //dtor
}
