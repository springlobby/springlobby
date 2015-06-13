/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "toasternotification.h"

#include "toasterbox/ToasterBox.h"
#include "settings.h"
#include <wx/settings.h>

ToasterNotification::ToasterNotification(wxWindow* parent)
    : m_toasterbox(new ToasterBox(parent))
    , m_width(330)
    , m_height(80)
    , m_x_offset(30)
    , m_y_offset(30)
{
	m_toasterbox->SetPopupSize(m_width, m_height);

	m_toasterbox->SetPopupBackgroundColor(0, 0, 0);
	m_toasterbox->SetPopupTextColor(255, 255, 255);
	//	m_toasterbox->SetPopupScrollSpeed(pScrollSpeed);
}

/**
 * Prepare and show notification window
 */
void ToasterNotification::Show(const wxBitmap& /*icon*/, const size_t pos, const UiEvents::NotficationData& data)
{
	m_toasterbox->SetPopupPauseTime(sett().GetNotificationPopupDisplayTime() * 1000);
	//	m_toasterbox->SetPopupBitmap( icon );
	//call this before showing everytime to accout for desktop resolution changes
	SetPopupPosition(pos);
	m_toasterbox->SetPopupData(data, false);
	m_toasterbox->Play();
}

ToasterNotification::~ToasterNotification()
{
	if (m_toasterbox) {
		m_toasterbox->CleanList();
	}
	delete m_toasterbox;
	m_toasterbox = 0;
}

void ToasterNotification::SetPopupPosition(const size_t pos)
{
	int dim_x = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int dim_y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	switch (size_t(pos)) {
		case ScreenPosition::bottom_left:
			m_toasterbox->SetPopupPosition(0 + m_x_offset, dim_y - m_height - m_y_offset);
			m_toasterbox->SetStackDirection(ToasterBox::StackUp);
			break;
		case ScreenPosition::top_left:
			m_toasterbox->SetPopupPosition(0 + m_x_offset, 0 + m_y_offset);
			m_toasterbox->SetStackDirection(ToasterBox::StackDown);
			break;
		case ScreenPosition::top_right:
			m_toasterbox->SetPopupPosition(dim_x - m_width - m_x_offset, 0 + m_y_offset);
			m_toasterbox->SetStackDirection(ToasterBox::StackDown);
			break;
		default: //bottom_right
			m_toasterbox->SetPopupPosition(dim_x - m_width - m_x_offset, dim_y - m_height - m_y_offset);
			m_toasterbox->SetStackDirection(ToasterBox::StackUp);
			break;
	}
}
