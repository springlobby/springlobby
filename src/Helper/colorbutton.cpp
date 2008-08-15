#include "colorbutton.h"
#include "../utils.h"
#include <wx/image.h>

ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
    const wxPoint& pos , const wxSize& size , long style , const wxValidator& validator,
    const wxString& name )
    : wxBitmapButton( parent, id, bitmap, pos , size , style , validator, name ),
    m_size(size), m_color()
{
    //ctor
}

ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxColor& color,
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

wxColor ColorButton::GetColor( )
{
    return m_color;
}

void ColorButton::SetColor( const wxColor& color )
{
    m_color = color;
    SetBitmapLabel ( GetBitmapFromColor( color ) );
    SetBackgroundColour( color );
}

wxBitmap ColorButton::GetBitmapFromColor( const wxColor& color )
{
    unsigned int h = m_size.GetHeight() > 0 ? m_size.GetHeight() : CONTROL_HEIGHT;
    unsigned int w = m_size.GetWidth() > 0 ? m_size.GetWidth() : m_size.GetHeight();
    wxImage colorImg ( w, h, true );
    unsigned char* data = colorImg.GetData();
    unsigned int pixels = w * h;

    for (unsigned int i = 0; i < pixels*3; i+=3)
    {
        data[i] = color.Red();
        data[i+1] = color.Green();
        data[i+2] = color.Blue();
    }

    return wxBitmap ( colorImg );
}
