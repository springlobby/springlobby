/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */

#include "customizations.h"
#include <lslunitsync/c_api.h>
#include <lslunitsync/unitsync.h>
#include <lslunitsync/image.h>
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
    std::string name = STD_STRING(archive_name);
    m_archive = archive_name;
    LSL::susynclib().AddArchive( name );
	//!TODO require blocking usync init if it's not loaded
    bool ret = m_customs.loadOptions( LSL::OptionsWrapper::ModCustomizations, name );
    if ( ret ) {
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
        m_help_url = TowxString(m_customs.getSingleValue( "help_url" ));
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
        const wxString path = TowxString(m_customs.getSingleValue(STD_STRING(key)));
#ifdef SL_QT_MODE
        wxBitmap icon_bmp ( wxQtConvertImage( LSL::usync().GetQImage( m_archive, path, false ) ) );
#else
        LSL::UnitsyncImage img(LSL::usync().GetImage( STD_STRING(m_archive), STD_STRING(path), false ));
        wxBitmap icon_bmp;assert(false);//(img);
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
    LSL::Enum::OptionType dummy;
    return m_customs.keyExists( STD_STRING(key), LSL::OptionsWrapper::ModCustomizations, false, dummy );
}

bool Customizations::Provides( const wxString& key ) const
{
	return m_active && KeyExists( key );
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

bool Customizations::Init( const QString& shortname, const QString& version )
{
	m_shortname = shortname;
    QString archive( m_shortname + "_Gui.sdd" );
    bool init_success = Init( TowxString(archive) );
	m_modname = LSL::usync().GetNameForShortname( TowxString(shortname), TowxString(version) );
	return init_success;
}

QString Customizations::DataBasePath()
{
	static bool cached = false;
	if ( cached )
		return dataBasePath_;

	QList<QString> checked_paths;
    QString sub_path = "games/" + ToQString( m_archive );
	for ( int i = 0; i < LSL::susynclib().GetSpringDataDirCount(); ++i ) {
		QDir data ( ToQString( LSL::susynclib().GetSpringDataDirByIndex(i) ) );
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

const QString& Customizations::GetModshortname() const
{
    return m_shortname;
}

const char* Customizations::DataException::what() const throw()
{
    std::stringstream out;
    QString s;
    foreach ( s, errors_ ) {
        out << s.toStdString() << std::endl;
        qDebug() << s;
    }
    return out.str().c_str();
}

#endif
