#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

class ToasterBox;

#include "../utils/uievents.h"
#include "../utils/isink.h"

class NotificationManager : public OnQuitSink < NotificationManager >
{
    public:
        virtual ~NotificationManager();

        void ShowNotification( UiEvents::NotficationData data );
		void OnQuit( GlobalEvents::GlobalEventData data );

    protected:
        NotificationManager();
        ToasterBox* m_toasterbox;
        void SetPopupPosition();

        //make globals holder have access to ctor
        template <class PB, class I >
        friend class GlobalObjectHolder;

        EventReceiverFunc< NotificationManager, UiEvents::NotficationData, &NotificationManager::ShowNotification> m_showNotificationSink;
		int m_width,m_height,m_x_offset,m_y_offset;

};

NotificationManager& notificationManager();

#endif // NOTIFICATIONMANAGER_H
