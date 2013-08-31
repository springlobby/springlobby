#include "colorbutton.h"

#include "../uiutils.h"
#include "../utils/controls.h"
#include <wx/image.h>

#ifdef __WXMSW__
    #include "../images/colourbox.xpm"
#endif


ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
    const wxPoint& pos , const wxSize& size , long style , const wxValidator& validator,
    const wxString& name )
    : wxBitmapButton( parent, id, bitmap, pos , size , style , validator, name ),
    m_size(size), m_color()
{
    //ctor
}

ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxColour& color,
    const wxPoint& pos , const wxSize& size , long style , const wxValidator& validator,
    const wxString& name )
    : wxBitmapButton( parent, id, wxBitmap(), pos , size , style , validator, name ),
    m_size(size),m_color(color)
{
    SetColor( color );
}

ColorButton::~ColorButton()
{
    //dtor
}

wxColour ColorButton::GetColor( )
{
    return m_color;
}

void ColorButton::SetColor( const wxColour& color )
{
    m_color = color;
    SetBitmapLabel ( GetBitmapFromColor( color ) );

	SetBackgroundColour( GetDefaultAttributes().colBg );
}

wxBitmap ColorButton::GetBitmapFromColor( const wxColour& colour )
{
	unsigned int h = m_size.GetHeight() > 0 ? m_size.GetHeight() : CONTROL_HEIGHT;
	unsigned int w = m_size.GetWidth() > 0 ? m_size.GetWidth() : m_size.GetHeight();
	wxImage img ( w, h, true );
	unsigned char* data = img.GetData();
	unsigned int pixels = w * h;

	for (unsigned int i = 0; i < pixels*3; i+=3)
	{
		data[i] = colour.Red();
		data[i+1] = colour.Green();
		data[i+2] = colour.Blue();
	}
	return wxBitmap ( img );
}
