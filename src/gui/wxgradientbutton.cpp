/////////////////////////////////////////////////////////////////////////////
// Name:        wxGradientButton.cpp
// Purpose:
// Author:      Volodymir (T-Rex) Tryapichko
// Modified by:
// Created:     01/08/2008 20:25:42
// RCS-ID:
// Copyright:   Volodymir (T-Rex) Tryapichko, 2008
// Licence:
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxgradientbutton.h"
#include <wx/dcbuffer.h>

////@begin XPM images
////@end XPM images

/*!
* wxGradientButton type definition
*/

IMPLEMENT_DYNAMIC_CLASS( wxGradientButton, wxWindow )

/*!
* wxGradientButton event table definition
*/

BEGIN_EVENT_TABLE( wxGradientButton, wxWindow )

	////@begin wxGradientButton event table entries
	EVT_SIZE( wxGradientButton::OnSize )
	EVT_PAINT( wxGradientButton::OnPaint )
	EVT_ERASE_BACKGROUND( wxGradientButton::OnEraseBackground )
	EVT_LEFT_DOWN( wxGradientButton::OnLeftDown )
	EVT_LEFT_UP( wxGradientButton::OnLeftUp )

	////@end wxGradientButton event table entries

END_EVENT_TABLE()

/*!
* wxGradientButton constructors
*/

wxGradientButton::wxGradientButton()
{
	Init();
}

wxGradientButton::wxGradientButton( wxWindow* parent,
                                    wxWindowID id, const wxString & label, const wxPoint& pos,
                                    const wxSize& size, long style )
{
	Init();
	Create( parent, id, label, pos, size, style );
}

/*!
* wxGradientButton creator
*/

bool wxGradientButton::Create( wxWindow* parent,
                               wxWindowID id, const wxString & label, const wxPoint& pos,
                               const wxSize& size, long style )
{
////@begin wxGradientButton creation
	wxWindow::Create( parent, id, pos, size, style );
	CreateControls();
////@end wxGradientButton creation
	m_Label = label;
	return true;
}

/*!
* wxGradientButton destructor
*/

wxGradientButton::~wxGradientButton()
{
////@begin wxGradientButton destruction
////@end wxGradientButton destruction
}

/*!
* Member initialisation
*/

void wxGradientButton::Init()
{
////@begin wxGradientButton member initialisation
	m_GradientTopStartColour = wxColour( 132, 125, 132 );
	m_GradientTopEndColour = wxColour( 74, 69, 74 );
	m_GradientBottomStartColour = wxColour( 0, 0, 0 );
	m_GradientBottomEndColour = wxColour( 57, 56, 57 );
	m_PressedColourTop = wxColour( 57, 56, 57 );
	m_PressedColourBottom = wxColour( 0, 0, 0 );
////@end wxGradientButton member initialisation
}

/*!
* Control creation for wxGradientButton
*/

void wxGradientButton::CreateControls()
{
////@begin wxGradientButton content construction
	this->SetForegroundColour( wxColour( 255, 255, 255 ) );
	this->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	this->SetFont( wxFont( 8, wxSWISS, wxNORMAL, wxBOLD, false, wxT( "Tahoma" ) ) );
////@end wxGradientButton content construction
}

/*!
* Should we show tooltips?
*/

bool wxGradientButton::ShowToolTips()
{
	return true;
}

/*!
* Get bitmap resources
*/

wxBitmap wxGradientButton::GetBitmapResource( const wxString& name )
{
// Bitmap retrieval
////@begin wxGradientButton bitmap retrieval
	wxUnusedVar( name );
	return wxNullBitmap;
////@end wxGradientButton bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon wxGradientButton::GetIconResource( const wxString& name )
{
// Icon retrieval
////@begin wxGradientButton icon retrieval
	wxUnusedVar( name );
	return wxNullIcon;
////@end wxGradientButton icon retrieval
}

wxSize wxGradientButton::DoGetBestSize() const
{
	wxSize labelSize = wxDefaultSize;
	GetTextExtent( m_Label, &labelSize.x, &labelSize.y );
	return wxSize( wxMax( 40, labelSize.x + 20 ), wxMax( 20, labelSize.y + 10 ) );
}

/*!
* wxEVT_PAINT event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientButton::OnPaint( wxPaintEvent& event )
{
// Before editing this code, remove the block markers.
	wxBufferedPaintDC dc( this );

	wxRect clientRect = GetClientRect();
	wxRect gradientRect = clientRect;
	gradientRect.SetHeight( gradientRect.GetHeight() / 2 +
	                        ( ( GetCapture() == this ) ? 1 : 0 ) );
	if ( GetCapture() != this )
	{
		dc.GradientFillLinear( gradientRect,
		                       m_GradientTopStartColour, m_GradientTopEndColour, wxSOUTH );
	}
	else
	{
		dc.SetPen( wxPen( m_PressedColourTop ) );
		dc.SetBrush( wxBrush( m_PressedColourTop ) );
		dc.DrawRectangle( gradientRect );
	}

	gradientRect.Offset( 0, gradientRect.GetHeight() );

	if ( GetCapture() != this )
	{
		dc.GradientFillLinear( gradientRect,
		                       m_GradientBottomStartColour, m_GradientBottomEndColour, wxSOUTH );
	}
	else
	{
		dc.SetPen( wxPen( m_PressedColourBottom ) );
		dc.SetBrush( wxBrush( m_PressedColourBottom ) );
		dc.DrawRectangle( gradientRect );
	}
	dc.SetPen( wxPen( GetBackgroundColour() ) );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.DrawRectangle( 0, 0, clientRect.GetWidth(), clientRect.GetHeight() );
	dc.SetFont( GetFont() );
	dc.SetTextForeground( GetForegroundColour() );
	if ( GetCapture() == this )
	{
		clientRect.Offset( 1, 1 );
	}
	dc.DrawLabel( m_Label, clientRect, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL );
}

/*!
* wxEVT_LEFT_DOWN event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientButton::OnLeftDown( wxMouseEvent& event )
{
	if ( GetCapture() != this )
	{
		CaptureMouse();
		Refresh();
	}
}

/*!
* wxEVT_LEFT_UP event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientButton::OnLeftUp( wxMouseEvent& event )
{
	if ( GetCapture() == this )
	{
		ReleaseMouse();
		Refresh();
		if ( GetClientRect().Contains( event.GetPosition() ) )
		{
			wxCommandEvent evt( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
			GetEventHandler()->AddPendingEvent( evt );
		}
	}
}

/*!
* wxEVT_ERASE_BACKGROUND event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientButton::OnEraseBackground( wxEraseEvent& event )
{
}

/*!
* wxEVT_SIZE event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientButton::OnSize( wxSizeEvent& event )
{
	Refresh();
}


