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

        void SetHTMLWindowTitle(wxString const&);
        void OnLinkClicked( wxHtmlLinkEvent& evt );
        virtual void OnLinkClicked(const wxHtmlLinkInfo& link);
        virtual void OnHTMLLinkClicked(const wxHtmlLinkInfo& link);
        virtual wxHtmlOpeningStatus OnHTMLOpeningURL(wxHtmlURLType type,
                                                     const wxString& url,
                                                     wxString *redirect) const;
        virtual wxPoint HTMLCoordsToWindow(wxHtmlCell *cell,
                                           const wxPoint& pos) const;
        virtual wxWindow* GetHTMLWindow();
        virtual wxColour GetHTMLBackgroundColour() const;
        virtual void SetHTMLBackgroundColour(const wxColour& clr);
        virtual void SetHTMLBackgroundImage(const wxBitmap& bmpBg);
        virtual void SetHTMLStatusText(const wxString& text);
        virtual wxCursor GetHTMLCursor(HTMLCursor type) const;


        DECLARE_EVENT_TABLE()
};

#endif // SLHTMLWINDOW_H
