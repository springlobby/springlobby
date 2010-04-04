/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#include "pastedialog.h"

#include <wx/icon.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/defs.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/artprov.h>
#include <wx/statbmp.h>
#include <wx/listctrl.h>

#include "../images/springlobby.xpm"
#include "../utils/conversion.h"

#ifdef HAVE_WX29
	//in < 29 this is defined in wxDialogBase, which seems to have disappeared
	enum { ButtonSizerFlags = wxOK|wxCANCEL|wxYES|wxNO|wxHELP|wxNO_DEFAULT };
#endif

BEGIN_EVENT_TABLE(PasteDialog,wxDialog)
//  EVT_BUTTON(wxID_NO, PasteDialog::OnOptionsNo)
END_EVENT_TABLE()

PasteDialog::PasteDialog( wxWindow *parent, const wxString& message )
	: wxDialog(parent,-1,_( "Flood warning" ),wxDefaultPosition,wxDefaultSize,wxFRAME_FLOAT_ON_PARENT|wxDEFAULT_DIALOG_STYLE),
	m_message( message )
{
	SetIcon( wxIcon(springlobby_xpm) );

//******** copied from wxsource/generic/msgdlgg.cpp with small modifications***********************************************************
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );

	wxBitmap bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);

	wxStaticBitmap *info_icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
	icon_text->Add( info_icon, 0, wxCENTER );

	// 2) text
	icon_text->Add( CreateTextSizer( message ), 0, wxALIGN_TOP| wxLEFT, 10 );

	topsizer->Add( icon_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
	topsizer->Add(0,10);

	// 3) buttons
	wxSizer *sizerBtn = CreateButtonSizer(wxYES_NO);
	if ( sizerBtn )
		topsizer->Add(sizerBtn, 0, wxALIGN_CENTRE| wxALL, 10 );


	SetAutoLayout( true );
	SetSizer( topsizer );

	topsizer->SetSizeHints( this );
	topsizer->Fit( this );

	Centre( wxBOTH | wxCENTER_FRAME);
/***************************************************************************************************/
}

PasteDialog::~PasteDialog()
{}
