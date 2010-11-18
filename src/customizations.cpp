/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "customizations.h"
#include "springunitsynclib.h"
#include "images/springlobby.xpm"

#include <wx/image.h>
#include <wx/msgdlg.h>
#ifdef SL_QT_MODE
#include <QImage>
#include <qt/converters.h>
#endif
const wxString Customizations::IntroKey = wxString ( _T("intro_file") );

/** @brief GetBackground
  *
  * @todo: document this function
  */
const wxBitmap& Customizations::GetBackground() const
{
    return m_background;
}

wxSize Customizations::GetBackgroundSize() const
{
    return wxSize( m_background.GetWidth(), m_background.GetHeight() );
}

const OptionsWrapper& Customizations::GetCustomizations() const
{
    return m_customs;
}

/** @brief GetAppIcon
  *
  * @todo: document this function
  */
const wxIcon& Customizations::GetAppIcon() const
{
    return m_app_ico;
}

/** @brief GetHelpUrl
  *
  * @todo: document this function
  */
const wxString& Customizations::GetHelpUrl() const
{
    return m_help_url;
}

/** @brief GetModname
  *
  * @todo: document this function
  */
const wxString& Customizations::GetModname() const
{
    return m_modname;
}

/** @brief Init
  *
  * @todo: document this function
  */
bool Customizations::Init(const wxString& modname)
{
	//!TODO require blocking usync init if it's not loaded
    m_modname = modname;
    if ( !usync().ModExists( m_modname ) )
        return false;
    susynclib().SetCurrentMod( m_modname );
    bool ret = m_customs.loadOptions( OptionsWrapper::ModCustomizations, m_modname );
    if ( ret ) {
        wxString icon_img_path = m_customs.getSingleValue( _T("icon") );
		wxString bg_img_path = m_customs.getSingleValue( _T("bg_image") );
#ifdef SL_QT_MODE
		wxBitmap icon_bmp ( wxQtConvertImage( usync().GetQImage( m_modname, icon_img_path, false ) ) );
		m_background_image = wxQtConvertImage ( usync().GetQImage( m_modname, bg_img_path, false ) );
#else
		wxBitmap icon_bmp (usync().GetImage( m_modname, icon_img_path, false ) );
		m_background_image =  usync().GetImage( m_modname, bg_img_path, false );
#endif


        m_app_ico.CopyFromBitmap( icon_bmp );
		m_background = wxBitmap( m_background_image );
		assert( m_background_image.IsOk() );

        m_help_url = m_customs.getSingleValue( _T("help_url") );
    }
	m_active =  ret;
    return ret;
}

/** @brief Customizations
  *
  * @todo: document this function
  */
 Customizations::Customizations()
	 : m_app_ico(springlobby_xpm),
	 m_active( false )
{

}

bool Customizations::Active() const
{
	return m_active;
}

bool Customizations::KeyExists( const wxString& key ) const
{
	OptionType dummy;
	return m_customs.keyExists( key, OptionsWrapper::ModCustomizations, false, dummy );
}

bool Customizations::Provides( const wxString& key ) const
{
	return m_active && KeyExists( key );
}

wxString Customizations::GetIntroText() const
{
	if ( !m_active )
		return wxEmptyString;
	return usync().GetTextfileAsString( m_modname, m_customs.getSingleValue( IntroKey ) );
}

/** @brief SLcustomizations
  *
  * @todo: document this function
  */
Customizations& SLcustomizations()
{
    static LineInfo<Customizations> m( AT );
    static GlobalObjectHolder<Customizations, LineInfo<Customizations> > s_customizations( m );
    return s_customizations;
}

