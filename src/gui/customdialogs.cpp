/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "customdialogs.h"

#include <wx/icon.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/defs.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/font.h>
#include <wx/event.h>
#include <wx/stattext.h>
#include <wx/artprov.h>
#include <wx/statbmp.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>
#include <wx/app.h>
#include <wx/choicdlg.h>

#include "utils/conversion.h"
#include "settings.h"
#include "images/springlobby.xpm"
#include "images/springsettings.xpm"
#include "utils/uievents.h"

const static int slButtonSizerFlags = wxOK | wxCANCEL | wxYES | wxNO | wxHELP | wxNO_DEFAULT;

BEGIN_EVENT_TABLE(CustomMessageBox, wxDialog)
EVT_BUTTON(wxID_NO, CustomMessageBox::OnOptionsNo)
END_EVENT_TABLE()

wxWindow* CustomMessageBoxBase::m_settingsWindow = 0;
wxWindow* CustomMessageBoxBase::m_lobbyWindow = 0;
static CustomMessageBox* s_nonmodbox = 0;
static ServerMessageBox* s_serverMsgBox = 0;
static ActNotifBox* s_actNotifBox = 0;
static MutelistWindow* s_mutelistWindow = 0;
static TimedMessageBox* s_timedMessageBox = 0;

CustomMessageBox::CustomMessageBox(wxIcon* icon, wxWindow* parent, const wxString& message,
				   const wxString& caption,
				   long style, const wxPoint& pos)
    : wxDialog(parent, -1, caption, pos, wxDefaultSize, style | wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_DIALOG_STYLE)
{
	if (icon)
		SetIcon(*icon);

	//******** copied from wxsource/generic/msgdlgg.cpp with small modifications***********************************************************


	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* icon_text = new wxBoxSizer(wxHORIZONTAL);


	// 1) icon

	//! \todo use bitmap from customisation
	wxBitmap bitmap;
	switch (style & wxICON_MASK) {
		default:
			bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
			break;

		case wxICON_ERROR:
			bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
			break;

		case wxICON_INFORMATION:
			bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
			break;

		case wxICON_WARNING:
			bitmap = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
			break;

		case wxICON_QUESTION:
			bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);
			break;
	}

	wxStaticBitmap* info_icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
	icon_text->Add(info_icon, 0, wxCENTER);

	// 2) text
	icon_text->Add(CreateTextSizer(message), 0, wxALIGN_TOP | wxLEFT, 10);

	topsizer->Add(icon_text, 1, wxCENTER | wxLEFT | wxRIGHT | wxTOP, 10);
	topsizer->Add(0, 10);

	// 3) buttons
	int center_flag = wxEXPAND;
	if (style & wxYES_NO)
		center_flag = wxALIGN_CENTRE;
	wxSizer* sizerBtn = CreateButtonSizer(style & slButtonSizerFlags);
	if (sizerBtn)
		topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10);


	SetAutoLayout(true);
	SetSizer(topsizer);

	topsizer->SetSizeHints(this);
	topsizer->Fit(this);
	/*
	wxSize size( GetSize() );
	if (size.x > size.y*3/2)
	{
	    size.x = size.y*3/2;
	    SetSize( size );
	}
	*/
	Centre(wxBOTH | wxCENTER_FRAME);
	/***************************************************************************************************/
}

CustomMessageBox::~CustomMessageBox()
{
        delete this->GetSizer();
}


void CustomMessageBox::OnOptionsNo(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_NO);
}

void CustomMessageBoxBase::setLobbypointer(wxWindow* arg)
{
	m_lobbyWindow = arg;
}

void CustomMessageBoxBase::setSettingspointer(wxWindow* arg)
{
	m_settingsWindow = arg;
}

wxWindow* CustomMessageBoxBase::getLobbypointer()
{
	//	 if (m_lobbyWindow==0)
	//			 wxLogWarning(_T("null parent window in custom message dialog"));
	return m_lobbyWindow;
}

wxWindow* CustomMessageBoxBase::getSettingspointer()
{
	//	 if (m_settingsWindow==0)
	//		 wxLogWarning(_T("null parent window in custom message dialog"));
	return m_settingsWindow;
}

wxIcon getIcon(int whichIcon)
{
	switch (whichIcon) {
		case SL_MAIN_ICON:
			return wxIcon(springlobby_xpm);
		case SS_MAIN_ICON:
			return wxIcon(springsettings_xpm);
		default:
			return wxIcon(wxNullIcon);
	}
}

wxWindow* getParent(int which)
{
	switch (which) {
		case SL_MAIN_ICON:
			return CustomMessageBoxBase::getLobbypointer();
		case SS_MAIN_ICON:
			return CustomMessageBoxBase::getSettingspointer();
	}
	return NULL;
}


int customMessageBox(int whichIcon, const wxString& message, const wxString& caption,
		     long style, int x, int y)
{
	wxWindow* parent = getParent(whichIcon);
	wxIcon icon = getIcon(whichIcon);
	CustomMessageBox dlg(&icon, parent, message, caption, style, wxPoint(x, y));
	int re = dlg.ShowModal();
	switch (re) {
		case wxID_OK:
			return wxOK;
		case wxID_CANCEL:
			return wxCANCEL;
		case wxID_YES:
			return wxYES;
		case wxID_NO:
			return wxNO;
		default:
			return -1;
	}
}

int timedMessageBox(int whichIcon, const wxString& message,
		    const wxString& caption, unsigned int delay, // miliseconds
		    long style, const int x, const int y)
{
	wxWindow* parent = getParent(whichIcon);
	wxIcon icon = getIcon(whichIcon);
	TimedMessageBox dlg(whichIcon, parent, message, caption, delay, style, wxPoint(x, y));
	int re = dlg.ShowModal();
	switch (re) {
		case wxID_OK:
			return wxOK;
		case wxID_CANCEL:
			return wxCANCEL;
		case wxID_YES:
			return wxYES;
		case wxID_NO:
			return wxNO;
	}
	return -1;
}

void timedMessageBoxNoModal(int whichIcon, const wxString& message,
			    const wxString& caption, unsigned int delay, // miliseconds
			    long style, const int x, const int y)
{
	wxWindow* parent = getParent(whichIcon);
	s_timedMessageBox = new TimedMessageBox(whichIcon, parent, message, caption, delay, style, wxPoint(x, y));
	s_timedMessageBox->Show(true);
}

void customMessageBoxNoModal(int whichIcon, const wxString& message, const wxString& caption,
			     long style, int x, int y)
{
	wxWindow* parent = getParent(whichIcon);
	wxIcon icon = getIcon(whichIcon);
	s_nonmodbox = new CustomMessageBox(&icon, parent, message, caption, style, wxPoint(x, y));
	s_nonmodbox->Show(true);
}

template <class T>
void closeAndDestroy(T* win)
{
	if (win) {
		win->Show(false);
		win->Destroy();
		win = 0;
	}
}

void freeStaticBox()
{
	closeAndDestroy(s_nonmodbox);
	closeAndDestroy(s_serverMsgBox);
	closeAndDestroy(s_actNotifBox);
	closeAndDestroy(s_mutelistWindow);
	closeAndDestroy(s_timedMessageBox);
}

CreditsDialog::CreditsDialog(wxWindow* parent, wxString title, int whichIcon)
    : wxDialog(parent, -1, title, wxDefaultPosition,
	       wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxFRAME_FLOAT_ON_PARENT)
{
	wxBoxSizer* container = new wxBoxSizer(wxVERTICAL);
	text_ctrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);
	container->Add(text_ctrl, 1, wxEXPAND);

	container->Add(wxDialog::CreateButtonSizer(wxOK));
	SetSizer(container);
	wxIcon icon = getIcon(whichIcon);
	SetIcons(icon);
}

void CreditsDialog::AddCredit(wxString person, wxString message)
{
	wxFont heavyFont(10, wxDEFAULT, wxNORMAL, wxBOLD);
	text_ctrl->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, heavyFont));
	text_ctrl->AppendText(person);
	if (!message.IsEmpty()) {
		text_ctrl->AppendText(_T(": "));
		text_ctrl->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, *wxNORMAL_FONT));
		text_ctrl->AppendText(message);
	}
	text_ctrl->AppendText(_T("\n\n"));
}

CreditsDialog::~CreditsDialog()
{
}

ServerMessageBox::~ServerMessageBox()
{
}

ServerMessageBox::ServerMessageBox(wxIcon* icon, wxWindow* parent, const wxString& message,
				   const wxString& caption,
				   long style, const wxPoint& pos)
    : wxDialog(parent, -1, caption, pos, wxDefaultSize, style | wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_DIALOG_STYLE | wxEXPAND)
{
	if (icon)
		SetIcon(*icon);

	m_messages = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT);
	m_messages->InsertColumn(0, wxEmptyString);
	topsizer = new wxBoxSizer(wxVERTICAL);

	AppendMessage(message);

	topsizer->Add(m_messages, 1, wxALL | wxEXPAND | wxALIGN_CENTRE, 10);
	topsizer->Add(0, 10);

	wxSizer* sizerBtn = CreateButtonSizer(wxOK);
	topsizer->Add(sizerBtn, 0, wxALL | wxALIGN_CENTRE, 10);

	SetSizer(topsizer);
	Centre(wxBOTH | wxCENTER_FRAME);
}

void ServerMessageBox::AppendMessage(const wxString& message)
{
	if (message == wxEmptyString)
		return;
	m_messages->InsertItem(m_messages->GetItemCount(), message);
	m_messages->SetColumnWidth(0, wxLIST_AUTOSIZE);

	SetSize(m_messages->GetColumnWidth(0) + 25, -1);
	Layout();
}

ActNotifBox::ActNotifBox(wxIcon* icon, wxWindow* parent, const wxString& message,
			 const wxString& caption,
			 long style, const wxPoint& pos)
    : ServerMessageBox(icon, parent, message, caption, style, pos)
{
}

ActNotifBox::~ActNotifBox()
{
}

void ActNotifBox::AppendMessage(const wxString& message)
{
	wxDateTime now = wxDateTime::Now();
	wxString msg = now.Format(_T("%H:%M: ")) + message;
	ServerMessageBox::AppendMessage(msg);
}

void serverMessageBox(int whichIcon, const wxString& message, const wxString& caption,
		      long style, int x, int y)
{
	wxWindow* parent = getParent(whichIcon);
	wxIcon icon = getIcon(whichIcon);
	if (s_serverMsgBox != 0 && s_serverMsgBox->IsShown()) {
		s_serverMsgBox->AppendMessage(message);
	} else {
		s_serverMsgBox = new ServerMessageBox(&icon, parent, message, caption, style, wxPoint(x, y));
		s_serverMsgBox->Show(true);
	}
}

void actNotifBox(int whichIcon, const wxString& message, const wxString& caption,
		 long style, int x, int y)
{
	if (sett().GetUseNotificationPopups()) {
		UiEvents::GetNotificationEventSender().SendEvent(UiEvents::NotficationData(UiEvents::UserAction, message));
	} else {
		wxWindow* parent = getParent(whichIcon);
		wxIcon icon = getIcon(whichIcon);
		if (s_actNotifBox != 0 && s_actNotifBox->IsShown()) {
			s_actNotifBox->AppendMessage(message);
		} else {
			s_actNotifBox = new ActNotifBox(&icon, parent, wxEmptyString, caption, style, wxPoint(x, y));
			s_actNotifBox->AppendMessage(message);
			s_actNotifBox->Show(true);
		}
	}
}

int GetSingleChoiceIndex(const wxString& message,
			 const wxString& caption,
			 const wxArrayString& choices,
			 const int selected,
			 wxWindow* parent,
			 int /*unused*/,
			 int /*unused*/,
			 bool /*unused*/)
{
	wxSingleChoiceDialog dialog(parent, message, caption, choices);
	dialog.SetSelection(selected);

	int choice;
	if (dialog.ShowModal() == wxID_OK)
		choice = dialog.GetSelection();
	else
		choice = -1;

	return choice;
}

void mutelistWindow(const wxString& message, const wxString& caption,
		    long style, const int x, const int y)
{
	wxWindow* parent = getParent(SL_MAIN_ICON);
	wxIcon icon = getIcon(SL_MAIN_ICON);

	if (s_mutelistWindow != 0 && s_mutelistWindow->IsShown()) {
		s_mutelistWindow->AppendMessage(message);
	} else {
		s_mutelistWindow = new MutelistWindow(&icon, parent, wxEmptyString, caption, style, wxPoint(x, y));
		s_mutelistWindow->AppendMessage(message);
		s_mutelistWindow->Show(true);
	}
}

MutelistWindow::MutelistWindow(wxIcon* icon, wxWindow* parent, const wxString& message,
			       const wxString& caption,
			       long style, const wxPoint& pos)
    : ServerMessageBox(icon, parent, message, caption, style, pos)
{
}

MutelistWindow::~MutelistWindow()
{
}

BEGIN_EVENT_TABLE(TimedMessageBox, wxDialog)
EVT_CLOSE(TimedMessageBox::OnClose)
EVT_BUTTON(wxID_NO, TimedMessageBox::OnOptionsNo)
END_EVENT_TABLE()

TimedMessageBox::TimedMessageBox(int whichIcon, wxWindow* parent, const wxString& message,
				 const wxString& caption,
				 unsigned int delay,
				 long style, const wxPoint& pos)
    : wxDialog(parent, -1, caption, pos, wxDefaultSize, style | wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_DIALOG_STYLE)
    , m_delay(delay)
    , m_display_hits(0)
{
	wxIcon ico = getIcon(whichIcon);
	SetIcon(ico);

	//******** copied from wxsource/generic/msgdlgg.cpp with small modifications***********************************************************


	topsizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* icon_text = new wxBoxSizer(wxHORIZONTAL);


	// 1) icon

	wxBitmap bitmap;
	switch (style & wxICON_MASK) {
		default:
			bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
			break;

		case wxICON_ERROR:
			bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
			break;

		case wxICON_INFORMATION:
			bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
			break;

		case wxICON_WARNING:
			bitmap = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
			break;

		case wxICON_QUESTION:
			bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);
			break;
	}

	wxStaticBitmap* info_icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
	icon_text->Add(info_icon, 0, wxCENTER);

	// 2) text
	icon_text->Add(CreateTextSizer(message), 0, wxALIGN_TOP | wxLEFT, 10);

	topsizer->Add(icon_text, 1, wxCENTER | wxLEFT | wxRIGHT | wxTOP, 10);
	topsizer->Add(0, 10);

	// 3) buttons
	int center_flag = wxEXPAND;
	if (style & wxYES_NO)
		center_flag = wxALIGN_CENTRE;
	sizerBtn = CreateButtonSizer(style & slButtonSizerFlags);
	if (sizerBtn) {
		topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10);
		sizerBtn->Show(false);
	}

	m_delay_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_delay_notif = new wxStaticText(this, -1, wxString::Format(_("Please standby %d seconds"), delay / 1000));
	m_delay_sizer->Add(m_delay_notif, 0, center_flag | wxALL, 10);
	topsizer->Add(m_delay_sizer);


	SetAutoLayout(true);
	SetSizer(topsizer);

	topsizer->SetSizeHints(this);
	topsizer->Fit(this);
	/*
	wxSize size( GetSize() );
	if (size.x > size.y*3/2)
	{
	    size.x = size.y*3/2;
	    SetSize( size );
	}
	*/
	Centre(wxBOTH | wxCENTER_FRAME);

	wxWindowID delay_timerID = wxNewId();
	wxWindowID display_timerID = wxNewId();
	m_delay_timer.SetOwner(this, delay_timerID);
	m_display_timer.SetOwner(this, display_timerID);
	Connect(delay_timerID, wxEVT_TIMER, wxTimerEventHandler(TimedMessageBox::OnUnlock));
	Connect(display_timerID, wxEVT_TIMER, wxTimerEventHandler(TimedMessageBox::OnUpdate));
	m_delay_timer.Start(m_delay, wxTIMER_ONE_SHOT);
	m_display_timer.Start(m_update_interval, wxTIMER_CONTINUOUS);
	/***************************************************************************************************/
}

TimedMessageBox::~TimedMessageBox()
{
}

void TimedMessageBox::OnOptionsNo(wxCommandEvent&)
{
	EndModal(wxID_NO);
}

void TimedMessageBox::OnUpdate(wxTimerEvent&)
{
	m_display_hits++;
	int remainder = (m_delay - (m_display_hits * m_update_interval)) / 1000;
	m_delay_notif->SetLabel(wxString::Format(_("Please standby %d seconds"), remainder));
}

void TimedMessageBox::OnClose(wxCloseEvent&)
{
	if (!m_delay_timer.IsRunning())
		Close();
}

void TimedMessageBox::OnUnlock(wxTimerEvent&)
{
	m_delay_sizer->Show(false);
	sizerBtn->Show(true);
	topsizer->SetSizeHints(this);
	topsizer->Fit(this);
	Layout();
}

AutocloseMessageBox::AutocloseMessageBox(wxWindow* parent, const wxString& message,
					 const wxString& caption,
					 unsigned int delay,
					 long style, const wxPoint& pos)
    : TimedMessageBox(SL_MAIN_ICON, parent, message, caption, delay, style, pos)
    , delay_timerID(wxNewId())
{
	m_delay_timer.SetOwner(this, delay_timerID);
	Connect(delay_timerID, wxEVT_TIMER, wxTimerEventHandler(AutocloseMessageBox::OnUnlock));
	wxSizer* topsizer = GetSizer();
	wxSizer* sizerBtn = CreateSeparatedButtonSizer(wxCANCEL);
	topsizer->Add(sizerBtn, 0, wxALL | wxALIGN_CENTRE, 10);
	topsizer->SetSizeHints(this);
	topsizer->Fit(this);
	Layout();
}

void AutocloseMessageBox::OnUnlock(wxTimerEvent& /*evt*/)
{
	EndModal(wxID_OK);
}

AutocloseMessageBox::~AutocloseMessageBox()
{
}

void AutocloseMessageBox::EndModal(int retCode)
{
	Disconnect(delay_timerID, wxEVT_TIMER, wxTimerEventHandler(AutocloseMessageBox::OnUnlock));
	m_delay_timer.Stop();
	TimedMessageBox::EndModal(retCode);
}
