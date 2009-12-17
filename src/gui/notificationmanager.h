#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

class ToasterBox;

#include "utils/uievents.h"

class NotificationManager
{
    public:
        virtual ~NotificationManager();

        void ShowNotification( UiEvents::NotficationData data );

    protected:
        NotificationManager();
        ToasterBox* m_toasterbox;
        void SetPopupPosition();

        //make globals holder have access to ctor
        template <class PB, class I >
        friend class GlobalObjectHolder;

        EventReceiverFunc< NotificationManager, UiEvents::NotficationData, &NotificationManager::ShowNotification> m_showNotificationSink;

};

NotificationManager& notificationManager();

#endif // NOTIFICATIONMANAGER_H
