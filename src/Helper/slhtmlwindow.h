#ifndef SLHTMLWINDOW_H
#define SLHTMLWINDOW_H

#include <wx/html/htmlwin.h>

//! A simple wxHtmlWindow that forwards link clicks to system browser
class slHtmlWindow : public wxHtmlWindow
{
    public:
        slHtmlWindow( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxHW_DEFAULT_STYLE,
            const wxString& name = _T("htmlWindow") );
        virtual ~slHtmlWindow();

        void OnLinkClicked( wxHtmlLinkEvent& evt );

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // SLHTMLWINDOW_H
