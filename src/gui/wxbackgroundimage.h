#ifndef SPRINGLOBBY_HEADERGUARD_WXBACKGROUNDIMAGE_H
#define SPRINGLOBBY_HEADERGUARD_WXBACKGROUNDIMAGE_H

#include <wx/event.h>
#include <wx/bitmap.h>

/** Usage:
ToolBarBackground = new wxBackgroundBitmap(wxBITMAP("test"));
ToolBar->PushEventHandler(ToolBarBackground);
**/

class wxBackgroundBitmap : public wxEvtHandler {
    //typedef wxEvtHandler Inherited;
public:
	wxBackgroundBitmap(const wxBitmap &B) :
		wxEvtHandler(),
		Bitmap(B),
		m_lastSize(wxDefaultSize)
			{ }
    virtual bool        ProcessEvent(wxEvent &Event);
protected:
	wxBitmap Bitmap;
	wxSize m_lastSize;
};

//copied from http://forum.wxwidgets.org/viewtopic.php?t=10019

#endif // SPRINGLOBBY_HEADERGUARD_WXBACKGROUNDIMAGE_H

