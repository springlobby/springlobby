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
	QtArg customization( 'c', "customize", "Load lobby customizations from game archive. Expects the long name.", true, true );
	QtArgDefaultHelpPrinter helpPrinter( "Testing help printing.\n" );
	QtArgHelp help( &cmd );
	help.setPrinter( &helpPrinter );
	cmd.addArg( config_file );
	cmd.addArg( customization );
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
		 wxFileName fn ( Settings::m_user_defined_config_path );
		 if ( ! fn.IsAbsolute() ) {
			 qDebug() << "path for parameter \"config-file\" must be absolute";
			 QMessageBox::critical( 0, "Fatal error", QString("path for parameter \"config-file\" must be absolute") );
			 return false;
		 }
		 if ( ! fn.IsFileWritable() ) {
			 qDebug() << "path for parameter \"config-file\" must be writeable";
			 QMessageBox::critical( 0, "Fatal error", QString("path for parameter \"config-file\" must be writable") );
			 return false;
		 }
		 qDebug() << Settings::m_user_defined_config_path.mb_str();
	}

#ifdef __WXMSW__
	sett().SetSearchSpringOnlyInSLPath( sett().GetSearchSpringOnlyInSLPath() );
#endif
	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

	if ( !wxDirExists( GetConfigfileDir() ) )
		wxMkdir( GetConfigfileDir() );

	usync().FastLoadUnitSyncLib( sett().GetCurrentUsedUnitSync() );

	if ( customization.isPresent() ) {
		QString customization_value = customization.value().toString();
		wxString customization_value_wx = TowxString( customization_value.toStdString() );
		wxLogError( customization_value_wx.c_str() );
		if ( !SLcustomizations().Init( customization_value_wx ) ) {
			qDebug() << "init false";
			QMessageBox::critical( 0, "Fatal error", QString("loading customizations failed for ").append( customization_value ) );
			return false;
		}
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

