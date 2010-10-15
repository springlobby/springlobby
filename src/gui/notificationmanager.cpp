#include "notificationmanager.h"

#include "../toasterbox/ToasterBox.h"
#include "../globalsmanager.h"
#include "../uiutils.h"
#include "../ui.h"
#include "../settings.h"
#include "../spring.h"
#include "../mainwindow.h"
#include "../utils/platform.h"
#include "../images/springlobby_64.png.h"

#ifdef HAVE_LIBNOTIFY
	#include "libnotify.h"
	typedef LibnotifyNotification NotificationWrapperType;
#else
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

ToasterNotification::ToasterNotification( wxWindow* parent )
	:m_toasterbox( new ToasterBox( parent ) ),
	m_width(300),
	m_height(80),
	m_x_offset(30),
	m_y_offset(30)
{
	m_toasterbox->SetPopupSize(m_width, m_height );

	m_toasterbox->SetPopupBackgroundColor(0,0,0);
	m_toasterbox->SetPopupTextColor(255,255,255);
//	m_toasterbox->SetPopupScrollSpeed(pScrollSpeed);
}

void ToasterNotification::Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data )
{
	m_toasterbox->SetPopupPauseTime(sett().GetNotificationPopupDisplayTime()*1000);
	m_toasterbox->SetPopupBitmap( icon );
	//call this before showing everytime to accout for desktop resolution changes
	SetPopupPosition( pos );
	m_toasterbox->SetPopupText( data.second, false);
	m_toasterbox->Play();
}

ToasterNotification::~ToasterNotification()
{
	if ( m_toasterbox )
	{
		m_toasterbox->CleanList();
	}
	delete m_toasterbox;
	m_toasterbox = 0;
}

void ToasterNotification::SetPopupPosition( const size_t pos )
{
    int dim_x = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
    int dim_y = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );
	switch ( size_t(pos) )
	{
		case ScreenPosition::bottom_left :
			m_toasterbox->SetPopupPosition( 0 + m_x_offset, dim_y - m_height - m_y_offset );
			m_toasterbox->SetStackDirection( ToasterBox::StackUp );
			break;
		case ScreenPosition::top_left :
			m_toasterbox->SetPopupPosition( 0 + m_x_offset, 0 + m_y_offset );
			m_toasterbox->SetStackDirection( ToasterBox::StackDown );
			break;
		case ScreenPosition::top_right :
			m_toasterbox->SetPopupPosition( dim_x - m_width - m_x_offset, 0 + m_y_offset );
			m_toasterbox->SetStackDirection( ToasterBox::StackDown );
			break;
		default://bottom_right
			m_toasterbox->SetPopupPosition( dim_x - m_width - m_x_offset, dim_y - m_height - m_y_offset );
			m_toasterbox->SetStackDirection( ToasterBox::StackUp );
			break;
	}
}

void NotificationManager::OnQuit( GlobalEvents::GlobalEventData /*data*/ )
{
	delete m_notification_wrapper;
}
