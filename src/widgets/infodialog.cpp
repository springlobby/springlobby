#include "infodialog.h"
#include "infopanel.h"

WidgetInfoDialog::WidgetInfoDialog(Widget& widget, wxWindow* parent, wxWindowID id,
            const wxString& title, long style )
    : wxDialog ( parent, id, title, wxDefaultPosition, wxDefaultSize, style )
{
    m_panel = new WidgetInfoPanel( widget, this, -1 );
}

WidgetInfoDialog::~WidgetInfoDialog()
{
    //dtor
}
