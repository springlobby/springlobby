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

class wxWindow;
class ScreenPosition;
class INotification
{
	public:
		INotification(){};
		virtual ~INotification(){};
		virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data ) = 0;
};

class ToasterNotification : public INotification
{
	public:
		ToasterNotification(wxWindow* parent);
		virtual ~ToasterNotification();
		virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data );
	protected:
		void SetPopupPosition( const size_t pos );
		int m_width,m_height,m_x_offset,m_y_offset;
		ToasterBox* m_toasterbox;
};

//#ifndef HAVE_LIBNOTIFY
//		ToasterBox* m_toasterbox;
//#else
//#endif

NotificationManager& notificationManager();

#endif // NOTIFICATIONMANAGER_H
