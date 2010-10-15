#include "notificationmanager.h"

#include "../globalsmanager.h"
#include "../uiutils.h"
#include "../ui.h"
#include "../settings.h"
#include "../spring.h"
#include "../mainwindow.h"
#include "../utils/platform.h"
#include "../images/springlobby_64.png.h"
#include "inotification.h"

#ifdef HAVE_LIBNOTIFY
	#include "libnotify.h"
	typedef LibnotifyNotification NotificationWrapperType;
#else
	#include "toasternotification.h"
	typedef ToasterNotification NotificationWrapperType;
#endif

NotificationManager& notificationManager()
{
    static LineInfo<NotificationManager> m( AT );
    static GlobalObjectHolder<NotificationManager, LineInfo<NotificationManager> > m_manager( m );
    return m_manager;
}

NotificationManager::NotificationManager()
	: m_showNotificationSink( this, &UiEvents::GetNotificationEventSender( ) ),
	m_notification_wrapper( new NotificationWrapperType( &ui().mw() ) )
{
}

NotificationManager::~NotificationManager()
{
	delete m_notification_wrapper;
}

void NotificationManager::ShowNotification( UiEvents::NotficationData data )
{

	const bool spring_running = spring().IsRunning();
	const bool disable_if_ingame = sett().Get<bool>( _T("/GUI/NotificationPopupDisableIngame"), true );
	if ( m_notification_wrapper &&  ! ( disable_if_ingame && spring_running ) ) {
		//! \todo use image from customizations
		wxBitmap nmp ( charArr2wxBitmap( springlobby_64_png, sizeof(springlobby_64_png) ) );
		m_notification_wrapper->Show( nmp, sett().GetNotificationPopupPosition(), data );
	}
}

void NotificationManager::OnQuit( GlobalEvents::GlobalEventData /*data*/ )
{
	delete m_notification_wrapper;
}
