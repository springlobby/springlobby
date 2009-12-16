#include "notificationmanager.h"

#include "../toasterbox/ToasterBox.h"
#include "../globalsmanager.h"

NotificationManager::NotificationManager()
    : m_showNotificationSink( this, &UiEvents::GetNotificationEventSender( ) )
{
    m_toasterbox = new ToasterBox();
        m_toasterbox->SetPopupPauseTime(3000);
//    m_toasterbox->SetPopupTextColor(colFg.Red(),colFg.Green(),colFg.Blue());
    m_toasterbox->SetPopupBackgroundColor(255,255,255);
    m_toasterbox->SetPopupTextColor(0,0,0);
//    m_toasterbox->SetPopupScrollSpeed(pScrollSpeed);
//    wxBitmap icon ( charArr2wxBitmap  (single_player_icon_png , sizeof (single_player_icon_png) ) );
//    m_toasterbox->SetPopupBitmap( icon );
}

NotificationManager::~NotificationManager()
{
    delete m_toasterbox;
}

void NotificationManager::ShowNotification( UiEvents::NotficationData data )
{
    m_toasterbox->SetPopupText( data.second, false);
    m_toasterbox->Play();
}

NotificationManager& notificationManager()
{
    static LineInfo<NotificationManager> m( AT );
    static GlobalObjectHolder<NotificationManager, LineInfo<NotificationManager> > m_manager( m );
    return m_manager;
}
