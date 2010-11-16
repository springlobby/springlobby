#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QDeclarativeContext>
#include <QtOpenGL/QGLWidget>
#include <stringdummy.h>
#define USE_OPENGL

#include <settings.h>
#include <utils/platform.h>

int main(int argc, char *argv[])
{
		QApplication app(argc, argv);

		QDeclarativeView view;
		// Visual initialization
 #ifdef USE_OPENGL
		QGLFormat format = QGLFormat::defaultFormat();
		 #ifdef Q_WS_MAC
				format.setSampleBuffers(true);
		 #else
				format.setSampleBuffers(false);
		 #endif
		QGLWidget *glWidget = new QGLWidget(format, &view);
		view.setViewport(glWidget);
		view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
 #endif

		view.setAttribute(Qt::WA_OpaquePaintEvent);
		view.setAttribute(Qt::WA_NoSystemBackground);
		view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
		view.setFocus();
		if ( !wxDirExists( GetConfigfileDir() ) )
			wxMkdir( GetConfigfileDir() );

	#ifdef __WXMSW__
		sett().SetSearchSpringOnlyInSLPath( sett().GetSearchSpringOnlyInSLPath() );
	#endif
		sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
		sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );
		QString de( sett().GetCachePath().mb_str() );
		StringDummy test(de );
		QDeclarativeContext* ctxt = view.rootContext();
		ctxt->setContextProperty("myAwesomeString", &test );

		view.setSource(QString("qml/mapview/main.qml"));
		view.show();


	return app.exec();
}

