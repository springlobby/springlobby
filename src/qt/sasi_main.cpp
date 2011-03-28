//#include "qmlapplicationviewer.h"

#include "sasi_app.h"

#include <springunitsync.h>

#include <settings.h>
#include <utils/platform.h>
#include <utils/conversion.h>
#include <customizations.h>
#include <globalsmanager.h>

#include <wx/intl.h>
#include <wx/log.h>
#include <wx/filename.h>

#include <QtArg/Arg>
#include <QtArg/XorArg>
#include <QtArg/CmdLine>
#include <QtArg/Help>
#include <QSplashScreen>

#include <QDebug>
#include <QMessageBox>

bool CmdInit()
{
	PwdGuard pwd_guard;//makes us invulnerabel to cwd changes in usync loading
	QtArgCmdLine cmd;
	QtArg config_file( 'f', "config-file", "absolute path to config file", false, true );
	QtArg customization( 'c', "customize", "Load lobby customizations from game archive. Expects the shortname.", true, true );
	QtArg version( 'r', "version", "Load lobby customizations from game archive given shortname and this version.", true, true );
	QtArgDefaultHelpPrinter helpPrinter( "Testing help printing.\n" );
	QtArgHelp help( &cmd );
	help.setPrinter( &helpPrinter );
	cmd.addArg( config_file );
	cmd.addArg( customization );
	cmd.addArg( version );
	try {
			cmd.parse();
	}
	catch( const QtHelpHasPrintedEx & x )
	{
	}
	catch( const QtArgBaseException & x )
	{
			qDebug() << x.what();
			QMessageBox::critical( 0, "Fatal error", QString("Parsing command line failed:\n").append(x.what()) );
			return false;
	}
	Settings::m_user_defined_config = config_file.isPresent();

	if ( Settings::m_user_defined_config ) {
		Settings::m_user_defined_config_path = TowxString( config_file.value().toString().toStdString() );
		qDebug() << ToQString( Settings::m_user_defined_config_path );
	}

#ifdef __WXMSW__
	sett().SetSearchSpringOnlyInSLPath( false );
	sett().SetPortableMode( false );
#endif
//	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
//	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

	qDebug() << "current used usync path" << ToQString(sett().GetCurrentUsedUnitSync()) ;

	if ( !wxDirExists( GetConfigfileDir() ) )
		wxMkdir( GetConfigfileDir() );

	usync().FastLoadUnitSyncLib( sett().GetCurrentUsedUnitSync() );

	QString customization_value = customization.value().toString();
	QString version_value = version.value().toString();
	qDebug() << QString( "shortname: %1\tversion: %2").arg( customization_value ).arg( version.value().toString() );
	if ( !SLcustomizations().Init( TowxString( customization_value.toStdString() ),
								  TowxString( version_value.toStdString() ) ) )
	{
		qDebug() << "init false";
		QMessageBox::critical( 0, "Fatal error", QString("loading customizations failed for ").append( customization_value ) );
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	SasiApp app(argc, argv);
	if ( !CmdInit() )
		return 1;
	int ret = app.exec();
	sett().SaveSettings();
	DestroyGlobals();
	return ret;
}

