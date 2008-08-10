#include "colorbutton.h"
#include <wx/image.h>

ColorButton::ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
    const wxPoint& pos , const wxSize& size , long style , const wxValidator& validator,
    const wxString& name )
    : wxBitmapButton( parent, id, bitmap, pos , size , style , validator, name ),
    m_size(size)
{
    //ctor
}

ColorButton::~ColorButton()
{
    //dtor
}

void ColorButton::SetColor( const wxColor& color )
{
    wxImage colorImg ( m_size.GetWidth(), m_size.GetHeight(), true );
    unsigned char* data = colorImg.GetData();
    unsigned int pixels = m_size.GetWidth()*m_size.GetHeight();

    for (unsigned int i = 0; i < pixels*3; i+=3)
    {
        data[i] = color.Red();
        data[i+1] = color.Green();
        data[i+2] = color.Blue();
    }

    wxBitmap bitmap ( colorImg );
    SetBitmapLabel ( bitmap );
}
