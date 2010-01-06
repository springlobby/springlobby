#include "notificationmanager.h"

#include "../toasterbox/ToasterBox.h"
#include "../globalsmanager.h"
#include "../uiutils.h"
#include "../ui.h"
#include "../mainwindow.h"
#include "../images/springlobby_64.png.h"

NotificationManager& notificationManager()
{
    static LineInfo<NotificationManager> m( AT );
    static GlobalObjectHolder<NotificationManager, LineInfo<NotificationManager> > m_manager( m );
    return m_manager;
}

NotificationManager::NotificationManager()
	: m_showNotificationSink( this, &UiEvents::GetNotificationEventSender( ) ),
	m_width(300),
	m_height(80)
{
	m_toasterbox = new ToasterBox(&ui().mw());
        m_toasterbox->SetPopupPauseTime(3000);
  m_toasterbox->SetPopupSize(m_width, m_height );

    m_toasterbox->SetPopupBackgroundColor(0,0,0);
    m_toasterbox->SetPopupTextColor(255,255,255);
//    m_toasterbox->SetPopupScrollSpeed(pScrollSpeed);
	wxBitmap nmp ( charArr2wxBitmap( springlobby_64_png, sizeof(springlobby_64_png) ) );
//    wxBitmap icon (  ) );
    m_toasterbox->SetPopupBitmap( nmp );
}

NotificationManager::~NotificationManager()
{
    delete m_toasterbox;
}

void NotificationManager::ShowNotification( UiEvents::NotficationData data )
{
    //call this before showing everytime to accout for desktop resolution changes
    SetPopupPosition();

    m_toasterbox->SetPopupText( data.second, false);
    m_toasterbox->Play();
}

void NotificationManager::SetPopupPosition()
{
    int dim_x = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
    int dim_y = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );
	m_toasterbox->SetPopupPosition( dim_x - m_width - 30, dim_y - m_height - 30 );
}
