/////////////////////////////////////////////////////////////////////////////
// Name:        based on wxGradientPanel.cpp
// Purpose:
// Author:      René Milk
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

#include "gradientpanel.h"
#include <wx/dcbuffer.h>

////@begin XPM images
////@end XPM images

/*!
* wxGradientPanel type definition
*/

IMPLEMENT_DYNAMIC_CLASS( wxGradientPanel, wxPanel )

/*!
* wxGradientPanel event table definition
*/

BEGIN_EVENT_TABLE( wxGradientPanel, wxPanel )

	////@begin wxGradientPanel event table entries
	EVT_SIZE( wxGradientPanel::OnSize )
	EVT_PAINT( wxGradientPanel::OnPaint )
	EVT_ERASE_BACKGROUND( wxGradientPanel::OnEraseBackground )
	EVT_LEFT_DOWN( wxGradientPanel::OnLeftDown )
	EVT_LEFT_UP( wxGradientPanel::OnLeftUp )

	////@end wxGradientPanel event table entries

END_EVENT_TABLE()

/*!
* wxGradientPanel constructors
*/

wxGradientPanel::wxGradientPanel()
{
	Init();
}

wxGradientPanel::wxGradientPanel( wxWindow* parent,
                                    wxWindowID id, const wxString & label, const wxPoint& pos,
                                    const wxSize& size, long style )
{
	Init();
	Create( parent, id, label, pos, size, style );
}

/*!
* wxGradientPanel creator
*/

bool wxGradientPanel::Create( wxWindow* parent,
                               wxWindowID id, const wxString & label, const wxPoint& pos,
                               const wxSize& size, long style )
{
////@begin wxGradientPanel creation
	wxWindow::Create( parent, id, pos, size, style );
	CreateControls();
////@end wxGradientPanel creation

	return true;
}

/*!
* wxGradientPanel destructor
*/

wxGradientPanel::~wxGradientPanel()
{
////@begin wxGradientPanel destruction
////@end wxGradientPanel destruction
}

/*!
* Member initialisation
*/

void wxGradientPanel::Init()
{
////@begin wxGradientPanel member initialisation
	m_GradientTopStartColour = wxColour( 132, 125, 132 );
	m_GradientTopEndColour = wxColour( 74, 69, 74 );
	m_GradientBottomStartColour = wxColour( 0, 0, 0 );
	m_GradientBottomEndColour = wxColour( 57, 56, 57 );
	m_PressedColourTop = wxColour( 57, 56, 57 );
	m_PressedColourBottom = wxColour( 0, 0, 0 );
////@end wxGradientPanel member initialisation
}

/*!
* Control creation for wxGradientPanel
*/

void wxGradientPanel::CreateControls()
{
////@begin wxGradientPanel content construction
	this->SetForegroundColour( wxColour( 255, 255, 255 ) );
	this->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	this->SetFont( wxFont( 8, wxSWISS, wxNORMAL, wxBOLD, false, wxT( "Tahoma" ) ) );
////@end wxGradientPanel content construction
}

/*!
* Should we show tooltips?
*/

bool wxGradientPanel::ShowToolTips()
{
	return true;
}

/*!
* Get bitmap resources
*/

wxBitmap wxGradientPanel::GetBitmapResource( const wxString& name )
{
// Bitmap retrieval
////@begin wxGradientPanel bitmap retrieval
	wxUnusedVar( name );
	return wxNullBitmap;
////@end wxGradientPanel bitmap retrieval
}

/*!
* Get icon resources
*/

wxIcon wxGradientPanel::GetIconResource( const wxString& name )
{
// Icon retrieval
////@begin wxGradientPanel icon retrieval
	wxUnusedVar( name );
	return wxNullIcon;
////@end wxGradientPanel icon retrieval
}

/*!
* wxEVT_PAINT event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientPanel::OnPaint( wxPaintEvent& event )
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

}

/*!
* wxEVT_LEFT_DOWN event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientPanel::OnLeftDown( wxMouseEvent& event )
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

void wxGradientPanel::OnLeftUp( wxMouseEvent& event )
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

void wxGradientPanel::OnEraseBackground( wxEraseEvent& event )
{
}

/*!
* wxEVT_SIZE event handler for ID_WXGRADIENTBUTTON
*/

void wxGradientPanel::OnSize( wxSizeEvent& event )
{
	Refresh();
}


