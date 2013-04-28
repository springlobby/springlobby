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

void slHtmlWindow::SetHTMLWindowTitle(wxString const& title)
{
  wxHtmlWindow::SetHTMLWindowTitle(title);
}

void slHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
  wxHtmlWindow::OnLinkClicked(link);
}

void slHtmlWindow::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
{
  wxHtmlWindow::OnHTMLLinkClicked(link);
}

wxHtmlOpeningStatus slHtmlWindow::OnHTMLOpeningURL(wxHtmlURLType type,
                                             const wxString& url,
                                             wxString *redirect) const
{
  return wxHtmlWindow::OnHTMLOpeningURL(type, url, redirect);
}

wxPoint slHtmlWindow::HTMLCoordsToWindow(wxHtmlCell *cell,
                                   const wxPoint& pos) const
{
  return wxHtmlWindow::HTMLCoordsToWindow(cell, pos);
}

wxWindow* slHtmlWindow::GetHTMLWindow()
{
  return wxHtmlWindow::GetHTMLWindow();
}

wxColour slHtmlWindow::GetHTMLBackgroundColour() const
{
  return wxHtmlWindow::GetHTMLBackgroundColour();
}

void slHtmlWindow::SetHTMLBackgroundColour(const wxColour& clr)
{
  wxHtmlWindow::SetHTMLBackgroundColour(clr);
}

void slHtmlWindow::SetHTMLBackgroundImage(const wxBitmap& bmpBg)
{
  wxHtmlWindow::SetHTMLBackgroundImage(bmpBg);
}

void slHtmlWindow::SetHTMLStatusText(const wxString& text)
{
  wxHtmlWindow::SetHTMLStatusText(text);
}

wxCursor slHtmlWindow::GetHTMLCursor(HTMLCursor type) const
{
  return wxHtmlWindow::GetHTMLCursor(type);
}

