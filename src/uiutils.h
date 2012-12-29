/// \file uiutils.h

#ifndef SPRINGLOBBY_HEADERGUARD_UIUTILS_H
#define SPRINGLOBBY_HEADERGUARD_UIUTILS_H

#include <wx/intl.h>
#include <wx/gdicmn.h>
#include <vector>
#include "utils/mixins.hh"

class wxColour;
class wxImage;
class wxIcon;
class wxBitmap;

wxString RTFtoText( const wxString& rtfinput );
bool AreColoursSimilar( const wxColour& col1, const wxColour& col2, int mindiff = 10 );

void ColourDelta( int& r, int& g, int& b, const int& delta );
wxColour ColourDelta( const wxColour& colour, const int& delta );

wxColour GetColorFromFloatStrng( const wxString& color );

//! takes best fitting size of original inside bounds keeping aspect ratio
wxSize MakeFit(const wxSize &original, const wxSize &bounds);

//! apply standard alpha blending to images
wxImage BlendImage( const wxImage& foreground, const wxImage& background, bool blend_alpha = true );
wxBitmap BlendBitmaps( const wxBitmap& background, const wxBitmap& overlay, const int dim = 16 );
//! used to load png data into a wxImage
wxImage charArr2wxImage(const unsigned char * arg, int size);
//! used to load png data into a wxBitmap
wxBitmap charArr2wxBitmap(const unsigned char * arg, int size);
wxBitmap charArr2wxBitmapWithBlending(const unsigned char * arg, int size,
            const unsigned char * text, int text_size);
//! png array data to Icon
wxIcon charArr2wxIcon(const unsigned char * arg, int size);

wxBitmap getColourIcon( const wxColour& colour );

//! shrinks/expands image by removing/duplicating rows/columns from the center of the image
wxImage BorderInvariantResizeImage(  const wxImage& image, int width, int height );


wxImage ReplaceChannelStatusColour( wxBitmap img, const wxColour& colour );

std::vector<wxColour>& GetBigFixColoursPalette( int numteams );


void CopyToClipboard( const wxString& text );

//! open either plattform default browser or binary saved in settings with given url
void OpenWebBrowser( const wxString& url );

class wxDateTime;
//! measures time diff between ctor and dtor, shows result in msg box
struct TimerMessageBox: public SL::NonCopyable {
	//! msg_format must contain exactly one %s to be filled with the elapsed time
	TimerMessageBox( const wxString& msg_format );
	~TimerMessageBox();
	wxDateTime* m_start;
	wxString m_msg_format;
};


#endif
// SPRINGLOBBY_HEADERGUARD_UIUTILS_H



/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

