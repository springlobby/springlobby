#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H


#include "../utils/uievents.h"
#include "../utils/globalevents.h"
#include "../utils/mixins.hh"
#include <lslutils/globalsmanager.h>
#include <wx/timer.h>
#include <wx/event.h>
#include <vector>

class INotification;

class NotificationManager : public SL::NonCopyable, public wxEvtHandler, public GlobalEvent
{
    public:
        virtual ~NotificationManager();

		void OnShowNotification( UiEvents::NotficationData data );

		void OnQuit( wxCommandEvent& data );

    protected:
        NotificationManager();
		INotification* m_notification_wrapper;

		wxTimer m_rate_limit_timer;
		const unsigned int m_rate_limit_ms;
		typedef std::vector<UiEvents::NotficationData>
			EventDataBuffer;
		EventDataBuffer m_eventDataBuffer;

		void OnTimer( wxTimerEvent& /*event*/ );
		void ShowNotification( const UiEvents::NotficationData& data );

        //make globals holder have access to ctor
        template <class PB, class I >
        friend class LSL::Util::GlobalObjectHolder;

		EventReceiverFunc< NotificationManager, UiEvents::NotficationData, &NotificationManager::OnShowNotification> m_showNotificationSink;

		DECLARE_EVENT_TABLE()

};

NotificationManager& notificationManager();

#endif // NOTIFICATIONMANAGER_H

/**
	This file is part of SpringLobby,
	Copyright (C) 2007-2011

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

