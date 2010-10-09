#include "slhtmlwindow.h"

#include "../uiutils.h"

BEGIN_EVENT_TABLE( slHtmlWindow, wxHtmlWindow )
    EVT_HTML_LINK_CLICKED(wxID_ANY, slHtmlWindow::OnLinkClicked)
END_EVENT_TABLE()

slHtmlWindow::slHtmlWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos ,
            const wxSize& size, long style,
            const wxString& name )
    : wxHtmlWindow( parent, id, pos, size, style, name )
{
    //ctor
}

slHtmlWindow::~slHtmlWindow()
{
    //dtor
}

void slHtmlWindow::OnLinkClicked( wxHtmlLinkEvent& evt )
{
    wxString url = evt.GetLinkInfo().GetHref();
    OpenWebBrowser( url );
}
