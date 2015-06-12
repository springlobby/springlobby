/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "notificationmanager.h"

#include <lslutils/globalsmanager.h>
#include "gui/uiutils.h"
#include "gui/ui.h"
#include "utils/slconfig.h"
#include "utils/globalevents.h"
#include "settings.h"
#include "spring.h"
#include "gui/mainwindow.h"
#include "inotification.h"
#include "sound/alsound.h"

#include "images/springlobby_64.png.h"

#ifdef HAVE_LIBNOTIFY
#include "libnotify.h"
typedef LibnotifyNotification NotificationWrapperType;
#else
#include "toasternotification.h"
typedef ToasterNotification NotificationWrapperType;
#endif


SLCONFIG("/GUI/NotificationPopupDisableIngame", true, "disable popup notifications when ingame");

const int this_timer_id = wxNewId();

BEGIN_EVENT_TABLE(NotificationManager, wxEvtHandler)
EVT_TIMER(this_timer_id, NotificationManager::OnTimer)
END_EVENT_TABLE()


NotificationManager& notificationManager()
{
	static LSL::Util::LineInfo<NotificationManager> m(AT);
	static LSL::Util::GlobalObjectHolder<NotificationManager, LSL::Util::LineInfo<NotificationManager> > m_manager(m);
	return m_manager;
}

NotificationManager::NotificationManager()
    : wxEvtHandler()
    , m_notification_wrapper(new NotificationWrapperType(&ui().mw()))
    , m_rate_limit_timer(this, this_timer_id)
    , m_rate_limit_ms(2000)
    , m_showNotificationSink(this, &UiEvents::GetNotificationEventSender())
{
	GlobalEventManager::Instance()->Subscribe(this, GlobalEventManager::OnQuit, wxObjectEventFunction(&NotificationManager::OnQuit));
}

NotificationManager::~NotificationManager()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);
	
	m_rate_limit_timer.Stop();
	delete m_notification_wrapper;
	m_notification_wrapper = NULL;
}

void NotificationManager::OnQuit(wxCommandEvent& /*data*/)
{
	delete m_notification_wrapper;
	m_notification_wrapper = NULL;
}

void NotificationManager::OnShowNotification(UiEvents::NotficationData data)
{
	if (m_rate_limit_timer.IsRunning()) {
		m_eventDataBuffer.push_back(data);
		return;
	}
	ShowNotification(data);
	m_rate_limit_timer.Start(m_rate_limit_ms, wxTIMER_ONE_SHOT);
}

void NotificationManager::ShowNotification(const UiEvents::NotficationData& data)
{
	if (sett().GetUseNotificationPopups()) {
		const bool disable_if_ingame = cfg().ReadBool(_T("/GUI/NotificationPopupDisableIngame"));
		if (m_notification_wrapper && !(disable_if_ingame && spring().IsRunning())) {
			//! \todo use image from customizations
			wxBitmap nmp(charArr2wxBitmap(springlobby_64_png, sizeof(springlobby_64_png)));
			m_notification_wrapper->Show(nmp, sett().GetNotificationPopupPosition(), data);
		}
	}
	if (sett().GetChatPMSoundNotificationEnabled())
		slsound().pm();
}

void NotificationManager::OnTimer(wxTimerEvent& /*event*/)
{
	UiEvents::NotficationData data;
	if (!m_eventDataBuffer.empty()) {
		if (m_eventDataBuffer.size() == 1)
			data = m_eventDataBuffer.front();
		else
			data.second = wxString::Format(_("%d more events"), (int)m_eventDataBuffer.size());
		m_eventDataBuffer.clear();
		ShowNotification(data);
	}
}
