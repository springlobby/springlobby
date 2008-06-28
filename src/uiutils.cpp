/// \file uiutils.cpp

/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/colour.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <cmath>
#include "uiutils.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"

wxString RefineMapname( const wxString& mapname )
{
    wxString ret = mapname;
    ret = ret.BeforeLast( '.' );
    ret.Replace(_T("_"), _T(" ") );
    ret.Replace(_T("-"), _T(" ") );
    return ret;
}


wxString RefineModname( const wxString& modname )
{
    wxString ret = modname;
    ret.Replace(_T("Absolute Annihilation"), _T("AA") );
    ret.Replace(_T("Complete Annihilation"), _T("CA") );
    ret.Replace(_T("Balanced Annihilation"), _T("BA") );
    ret.Replace(_T("Expand and Exterminate"), _T("EE") );
    ret.Replace(_T("War Evolution"), _T("WarEv") );
    ret.Replace(_T("TinyComm"), _T("TC") );
    ret.Replace(_T("BETA"), _T("b") );
    ret.Replace(_T("Public Alpha"), _T("pa") );
    ret.Replace(_T("Public Beta"), _T("pb") );
    ret.Replace(_T("Public"), _T("p") );
    ret.Replace(_T("Alpha"), _T("a") );
    ret.Replace(_T("Beta"), _T("b") );
    return ret;
}


wxString RTFtoText( const wxString& rtfinput )
{
    wxString ret = rtfinput;
    ret = ret.AfterFirst( '{' ).BeforeLast( '}' );

    ret.Replace( _T("\\pard"), _T("") ); // remove a ambiguus char

    ret.Replace( _T("\\par"), _T(" \n") ); // convert the end of lines

    wxString BeforeBrack = ret.BeforeFirst( '{' );
    wxString AfterBrack = ret.AfterLast( '}' );
    ret = BeforeBrack + AfterBrack; // remove everyhting that matches { text }

    wxString out;
    while ( ret.Find('\\') >= 0 ) //remove anything in the form \please\ignore\this
    {
        out += ret.BeforeFirst( '\\' );
        ret = ret.AfterFirst ( '\\' );
        ret = ret.AfterFirst ( ' ' );
    } ;

    return out;
}

bool AreColoursSimilar( const wxColour& col1, const wxColour& col2, int mindiff )
{
    int r,g,b;
    r = col1.Red() - col2.Red();
    g = col1.Green() - col2.Green();
    b = col1.Blue() - col2.Blue();
    r = r>0?r:-r;
    g = g>0?g:-g;
    b = b>0?b:-b;
    if ( (r <= mindiff) && (g <= mindiff) && (b <= mindiff) ) return true;
    return false;
}


void ColourDelta( int& r, int& g, int& b, const int& delta )
{
    int tmpdelta;
    if ( delta > 0 )
    {
        r += delta;
        tmpdelta = delta;
        if ( r > 255 )
        {
            tmpdelta += r - 255;
            r = 255;
        }
        g += tmpdelta;
        tmpdelta = delta;
        if ( g > 255 )
        {
            tmpdelta += g - 255;
            g = 255;
        }
        b += tmpdelta;
        if ( b > 255 ) b = 255;
    }
    else
    {
        r += delta;
        tmpdelta = -delta;
        if ( r < 0 )
        {
            tmpdelta -= r;
            r = 0;
        }
        g -= tmpdelta;
        tmpdelta = -delta;
        if ( g < 0 )
        {
            tmpdelta -= g;
            g = 0;
        }
        b -= tmpdelta;
        if ( b < 0 ) b = 0;
    }
}


wxColour ColourDelta( const wxColour& colour, const int& delta )
{
    int r = colour.Red();
    int g = colour.Green();
    int b = colour.Blue();
    ColourDelta( r, g, b, delta );
    return wxColour( r, g, b );
}



wxString GetColorString( const wxColour& color )
{
    return wxString::Format( _T("%d %d %d"), color.Red(), color.Green(), color.Blue() );

}



wxColour GetColorFromStrng( const wxString color )
{
    wxString c = color;
    long r = 0, g = 0, b = 0;
    c.BeforeFirst( ' ' ).ToLong( &r );
    c = c.AfterFirst( ' ' );
    c.BeforeFirst( ' ' ).ToLong( &g );
    c = c.AfterFirst( ' ' );
    c.BeforeFirst( ' ' ).ToLong( &b );
    return wxColour( r%256, g%256, b%256 );
}

void BlendImage(wxImage& foreground, wxImage&  background, int img_dim)
{
    unsigned char* background_data = background.GetData();
    unsigned char* foreground_data = foreground.GetData();

    if ( background.GetWidth()  != img_dim ||
         foreground.GetWidth()  != img_dim ||
         background.GetHeight() != img_dim ||
         foreground.GetHeight() != img_dim )
    {
        wxLogDebugFunc(_T("size mismatch while blending"));
        return;
    }

    if ( background.HasAlpha() && foreground.HasAlpha() )
    {
        unsigned char* background_alpha = background.GetAlpha();
        unsigned char* foreground_alpha = foreground.GetAlpha();
        unsigned int pixel_count = img_dim*img_dim;

        for ( unsigned int i = 0, i_a = 0; i < pixel_count * 3; i+=3,  i_a++ )
        {
            unsigned char back_alpha = background_alpha[i_a] ;
            unsigned char fore_alpha = foreground_alpha[i_a] ;
            float back_blend_fac = ( 255 - fore_alpha)/255.0;
            float fore_blend_fac = fore_alpha/255.0 ;

            background_data[i]    = foreground_data[i]   * fore_blend_fac + background_data[i]   * back_blend_fac ;
            background_data[i+1]  = foreground_data[i+1] * fore_blend_fac + background_data[i+1] * back_blend_fac ;
            background_data[i+2]  = foreground_data[i+2] * fore_blend_fac + background_data[i+2] * back_blend_fac ;
            background_alpha[i_a] = fore_alpha           * fore_blend_fac + back_alpha           * back_blend_fac ;
        }
    }
    else
    {
        wxLogDebugFunc(_T("cannot blend without alpha"));
        return;
    }
}

wxBitmap* charArr2wxBitmap(const unsigned char * arg, int size)
{
    wxMemoryInputStream istream( arg, size );
    wxImage temp( istream, wxBITMAP_TYPE_PNG );
    return new wxBitmap(temp );
}

//wxBitmap charArr2wxBitmap(const unsigned char * arg, int size)
//{
//    wxMemoryInputStream istream( arg, size );
//     wxImage temp( istream, wxBITMAP_TYPE_PNG );
//    return wxBitmap(temp );
//}

wxBitmap* charArr2wxBitmapAddText(const unsigned char * dest, int dest_size, const unsigned char * text, int text_size, unsigned int img_dim)
{
    wxMemoryInputStream istream1( dest, dest_size );
    wxImage dest_img( istream1, wxBITMAP_TYPE_PNG );
    wxMemoryInputStream istream2( text, text_size );
    wxImage text_img( istream2, wxBITMAP_TYPE_PNG );
    BlendImage(text_img, dest_img, img_dim );

    return new wxBitmap( dest_img );

}


