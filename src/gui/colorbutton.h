/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SL_HEADERGUARD_COLORBUTTON_H
#define SL_HEADERGUARD_COLORBUTTON_H

#include <wx/bmpbuttn.h>
//! a wxBitmapButton whose Bitmap is a rectangle with uni color solid fill
class ColorButton : public wxBitmapButton
{
public:
	ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap = wxBitmap(), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
		    const wxValidator& validator = wxDefaultValidator, const wxString& name = _T("button"));
	ColorButton(wxWindow* parent, wxWindowID id, const wxColour& color, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
		    const wxValidator& validator = wxDefaultValidator, const wxString& name = _T("button"));
	virtual ~ColorButton();

	void SetColor(const wxColour& color);
	wxColour GetColor();

private:
	wxSize m_size;
	wxColour m_color;
	wxBitmap GetBitmapFromColor(const wxColour& color);
};

#endif // SL_HEADERGUARD_COLORBUTTON_H
