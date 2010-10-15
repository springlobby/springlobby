#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

class ToasterBox;

#include "../utils/uievents.h"
#include "../utils/isink.h"
#include "../utils/mixins.hh"
class INotification;

class NotificationManager : public OnQuitSink < NotificationManager > , public SL::NonCopyable
{
    public:
        virtual ~NotificationManager();

        void ShowNotification( UiEvents::NotficationData data );
		void OnQuit( GlobalEvents::GlobalEventData data );

    protected:
        NotificationManager();
		INotification* m_notification_wrapper;

        //make globals holder have access to ctor
        template <class PB, class I >
        friend class GlobalObjectHolder;

        EventReceiverFunc< NotificationManager, UiEvents::NotficationData, &NotificationManager::ShowNotification> m_showNotificationSink;

};

NotificationManager& notificationManager();

#endif // NOTIFICATIONMANAGER_H

/**
	This file is part of SpringLobby,
	Copyright (C) 2007-2010

	SpringLobby is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 2 as published by
	the Free Software Foundation.

	SpringLobby is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

