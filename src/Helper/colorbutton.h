#ifndef SL_HEADERGUARD_COLORBUTTON_H
#define SL_HEADERGUARD_COLORBUTTON_H

#include <wx/bmpbuttn.h>


class ColorButton : public wxBitmapButton
{
    public:
        ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap = wxBitmap(), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = _T("button") );
        virtual ~ColorButton();

        void SetColor( const wxColor& color );

    protected:
        wxSize m_size;

};

#endif // SL_HEADERGUARD_COLORBUTTON_H
