/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */

#include "customizations.h"
#include <lslunitsync/c_api.h>
#include <lslunitsync/unitsync.h>
#include <lslunitsync/image.h>
#include <utils/conversion.h>
#include "images/springlobby12x12.xpm"
#include "images/springlobby_64.png.h"
#include "uiutils.h"

#include <wx/image.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>
const wxString Customizations::IntroKey = wxString ( _T("intro_file") );

const LSL::OptionsWrapper& Customizations::GetCustomizations() const
{
    return m_customs;
}

/** @brief GetAppIcon
  *
  * @todo: document this function
  */
const wxIconBundle& Customizations::GetAppIconBundle() const
{
	return m_app_icons;
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

const wxString& Customizations::Archive() const
{
    return m_archive;
}

/** @brief Init
  *
  * @todo: document this function
  */
bool Customizations::Init( const wxString& archive_name )
{
    m_archive = archive_name;
    LSL::susynclib().AddArchive(STD_STRING(m_archive));
	//!TODO require blocking usync init if it's not loaded
    bool ret = m_customs.loadOptions( LSL::OptionsWrapper::ModCustomizations, STD_STRING(m_archive));
    if ( ret )
    {
      wxBitmap icon_bmp( wxNullBitmap );
      if ( GetBitmap( _T("icon"), icon_bmp ) )
      {
        wxIcon tmp;
        tmp.CopyFromBitmap( icon_bmp );
        m_app_icons = wxIconBundle( tmp );//replacing current
        int i = 1;
        while( GetBitmap( wxString::Format(_T("icon%d"), i ), icon_bmp ) )
        {
          tmp.CopyFromBitmap( icon_bmp );
          m_app_icons.AddIcon( tmp );
        }
      }
      m_help_url = TowxString(m_customs.getSingleValue("help_url"));
    }
    m_active =  ret;
    return ret;
}

/** @brief Customizations
  *
  * @todo: document this function
  */
 Customizations::Customizations()
   : m_app_icons(wxIcon(springlobby12x12_xpm)),
	 m_active( false )
{
	m_app_icons.AddIcon( charArr2wxIcon( springlobby_64_png, sizeof(springlobby_64_png) ) );
}

bool Customizations::GetBitmap( const wxString& key, wxBitmap& bitmap )
{
	if ( Provides( key ) )
	{
    const auto path = m_customs.getSingleValue(STD_STRING(key));
    wxBitmap icon_bmp;
		if( icon_bmp.IsOk() )
		{
			bitmap = icon_bmp;
			return true;
		}
	}
	return false;//either loaded bmp was kaput or key not found
}

bool Customizations::Active() const
{
	return m_active;
}

bool Customizations::KeyExists( const wxString& key ) const
{
  LSL::Enum::OptionType dummy;
  return m_customs.keyExists(STD_STRING(key), LSL::OptionsWrapper::ModCustomizations, false, dummy );
}

bool Customizations::Provides( const wxString& key ) const
{
  return m_active && KeyExists(key);
}

wxString Customizations::GetIntroText() const
{
	if ( !m_active )
		return wxEmptyString;
  return TowxString(LSL::usync().GetTextfileAsString(STD_STRING(m_archive), m_customs.getSingleValue(STD_STRING(IntroKey))));
}

/** @brief SLcustomizations
  *
  * @todo: document this function
  */
Customizations& SLcustomizations()
{
    static LSL::Util::LineInfo<Customizations> m( AT );
    static LSL::Util::GlobalObjectHolder<Customizations, LSL::Util::LineInfo<Customizations> > s_customizations( m );
    return s_customizations;
}

