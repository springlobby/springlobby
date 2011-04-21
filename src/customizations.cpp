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
bool Customizations::Init(const wxString& shortname,const wxString& version )
{
	//!TODO require blocking usync init if it's not loaded
	m_modname = usync().GetNameForShortname( shortname, version );
    if ( !usync().ModExists( m_modname ) )
        return false;
    susynclib().SetCurrentMod( m_modname );
    bool ret = m_customs.loadOptions( OptionsWrapper::ModCustomizations, m_modname );
    if ( ret ) {
        wxString icon_img_path = m_customs.getSingleValue( _T("icon") );
#ifdef SL_QT_MODE
		m_shortname = ToQString( shortname );
		wxBitmap icon_bmp ( wxQtConvertImage( usync().GetQImage( m_modname, icon_img_path, false ) ) );
#else
		wxBitmap icon_bmp (usync().GetImage( m_modname, icon_img_path, false ) );
#endif
        m_app_ico.CopyFromBitmap( icon_bmp );
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
