/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "colorbutton.h"

#include <wx/image.h>

#include "gui/controls.h"
#include "gui/uiutils.h"
#ifdef __WXMSW__
#include "images/colourbox.xpm"
#endif


ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
			 const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
			 const wxString& name)
    : wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name)
    , m_size(size)
    , m_color()
{
	//ctor
}

ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxColour& color,
			 const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
			 const wxString& name)
    : wxBitmapButton(parent, id, wxBitmap(), pos, size, style, validator, name)
    , m_size(size)
    , m_color(color)
{
	SetColor(color);
}

ColorButton::~ColorButton()
{
	//dtor
}

wxColour ColorButton::GetColor()
{
	return m_color;
}

void ColorButton::SetColor(const wxColour& color)
{
	m_color = color;
	SetBitmapLabel(GetBitmapFromColor(color));

#ifdef __WXMSW__
	SetBackgroundColour(GetDefaultAttributes().colBg);
#endif
}

wxBitmap ColorButton::GetBitmapFromColor(const wxColour& colour)
{
#ifdef __WXMSW__
	wxImage img(colourbox_xpm);

	img.Replace(1, 1, 1, colour.Red(), colour.Green(), colour.Blue());

	int r, g, b;
	r = colour.Red() + 80;
	g = colour.Green() + 80;
	b = colour.Blue() + 80;
	img.Replace(2, 2, 2, r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
	return wxBitmap(img);
#else
	unsigned int h = m_size.GetHeight() > 0 ? m_size.GetHeight() : CONTROL_HEIGHT;
	unsigned int w = m_size.GetWidth() > 0 ? m_size.GetWidth() : m_size.GetHeight();
	wxImage img(w, h, true);
	unsigned char* data = img.GetData();
	unsigned int pixels = w * h;

	for (unsigned int i = 0; i < pixels * 3; i += 3) {
		data[i] = colour.Red();
		data[i + 1] = colour.Green();
		data[i + 2] = colour.Blue();
	}
	return wxBitmap(img);
#endif
}
