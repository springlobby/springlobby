/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */

#include "customizations.h"
#include "springunitsynclib.h"
#include "images/springlobby.xpm"
#include "images/springlobby_64.png.h"
#include "uiutils.h"

#include <wx/image.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>
#ifdef SL_QT_MODE
	#include <QImage>
	#include <qt/converters.h>
#endif
const wxString Customizations::IntroKey = wxString ( _T("intro_file") );

const OptionsWrapper& Customizations::GetCustomizations() const
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

/** @brief Init
  *
  * @todo: document this function
  */
bool Customizations::Init(const wxString& archive_name )
{
	//!TODO require blocking usync init if it's not loaded
	m_modname = archive_name;
    bool ret = m_customs.loadOptions( OptionsWrapper::ModCustomizations, m_modname );
    if ( ret ) {
#ifdef SL_QT_MODE
		m_shortname = ToQString( shortname );
#endif
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
	 : m_app_icons(wxIcon(springlobby_xpm)),
	 m_active( false )
{
	m_app_icons.AddIcon( charArr2wxIcon( springlobby_64_png, sizeof(springlobby_64_png) ) );
}

bool Customizations::GetBitmap( const wxString& key, wxBitmap& bitmap )
{
	if ( Provides( key ) )
	{
		const wxString path = m_customs.getSingleValue( key );
#ifdef SL_QT_MODE
		wxBitmap icon_bmp ( wxQtConvertImage( usync().GetQImage( m_modname, path, false ) ) );
#else
		wxBitmap icon_bmp (usync().GetImage( m_modname, path, false ) );
#endif
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

#ifdef SL_QT_MODE

#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include "qt/qerrorwindow.h"
#include <QCoreApplication>

QString Customizations::DataBasePath()
{
	static bool cached = false;
	if ( cached )
		return dataBasePath_;

	QList<QString> checked_paths;
	QString sub_path( "lobby/SpringLobby/customizations/" );
	sub_path.append( m_shortname );
	for ( int i = 0; i < susynclib().GetSpringDataDirCount(); ++i ) {
		QDir data ( ToQString( susynclib().GetSpringDataDirByIndex(i) ) );
		checked_paths.append( data.absolutePath().append("/").append( sub_path ) );
		if ( data.cd( sub_path ) ) {
			dataBasePath_ = data.absolutePath();
			break;
		}
	}
	if( dataBasePath_ != QString() )
		return dataBasePath_ ;

	checked_paths.prepend( QString("Couldn't find customization data in any of these directories:\n ") );
	throw DataException( checked_paths );

	return QString();
}

QString Customizations::QmlDir()
{
	return QDir( DataBasePath() + "/" + "qml" ).absolutePath();
}
QString Customizations::GraphicsDir()
{
	return QDir( DataBasePath() + "/" + "graphics" ).absolutePath();
}
QString Customizations::SoundsDir()
{
	return QDir( DataBasePath() + "/" + "sounds" ).absolutePath();
}
QString Customizations::MusicDir()
{
	return QDir( DataBasePath() + "/" + "music" ).absolutePath();
}
#endif
