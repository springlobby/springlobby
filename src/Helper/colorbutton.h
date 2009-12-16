#ifndef SL_HEADERGUARD_COLORBUTTON_H
#define SL_HEADERGUARD_COLORBUTTON_H

#include <wx/bmpbuttn.h>

//! a wxBitmapButton whose Bitmap is a rectangle with uni color solid fill
class ColorButton : public wxBitmapButton
{
    public:
        ColorButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap = wxBitmap(), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = _T("button") );
        ColorButton(wxWindow* parent, wxWindowID id, const wxColour& color, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = _T("button") );
        virtual ~ColorButton();

        void SetColor( const wxColour& color );
        wxColour GetColor( );

    protected:
        wxSize m_size;
        wxColour m_color;
        wxBitmap GetBitmapFromColor( const wxColour& color );

};

#endif // SL_HEADERGUARD_COLORBUTTON_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

