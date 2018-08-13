/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CUSTOM_MSG_BOX_H_
#define CUSTOM_MSG_BOX_H_

#include <wx/defs.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include "utils/mixins.h"
const unsigned SL_MAIN_ICON = 1;
const unsigned SS_MAIN_ICON = 2;


class wxIcon;
class wxWindow;
class wxPoint;
class wxString;
class wxTextCtrl;
class wxStaticText;
class wxCommandEvent;
class wxCloseEvent;
class wxBoxSizer;
class wxListCtrl;
class wxGauge;

#define SL_MAIN_WINDOW_PTR CustomMessageBox::getLobbypointer()
#define SE_FRAME_PTR CustomMessageBox::getSettingspointer()

/** \brief utlity function to display modal messagebox
 * \return wxOK|wxCANCEL|wxYES|wxNO according to option chosen
 */
int customMessageBox(int whichIcon, const wxString& message,
		     const wxString& caption = wxMessageBoxCaptionStr,
		     long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

/** \brief utlity function to display modal messagebox
 * the dialog itself is statically allocated, therefore only one
 * one of these may exist at any given time. \n
 * It is possible to use this with other styles than wxOK|wxICON_INFORMATION,
 * but since it's displayed non-modal nothing would come of it
 */
void customMessageBoxModal(int whichIcon, const wxString& message,
			   const wxString& caption = wxMessageBoxCaptionStr,
			   long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

/** \brief displays server messages when no chat window has focus
 * If dialog currently isn't shown, it's brought up. If dialog already is shown (not necessarily having focus)
 * message is appended, rather than old box replaced with new.
 */
void serverMessageBox(int whichIcon, const wxString& message,
		      const wxString& caption = wxMessageBoxCaptionStr,
		      long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

/** \brief displays user action notifications
 * If dialog currently isn't shown, it's brought up. If dialog already is shown (not necessarily having focus)
 * message is appended, rather than old box replaced with new.
 */
void actNotifBox(int whichIcon, const wxString& message,
		 const wxString& caption = _T("User action notification"),
		 long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

/** \brief show  mutelist for a specific channel
 *
 */
void mutelistWindow(const wxString& message,
		    const wxString& caption = _T("Mutelist"),
		    long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

/** \brief utlity function to display modal messagebox, closing is denied until timer ran down
 * \return wxOK|wxCANCEL|wxYES|wxNO according to option chosen
 */
int timedMessageBox(int whichIcon, const wxString& message,
		    const wxString& caption = wxMessageBoxCaptionStr, unsigned int delay = 3000, // miliseconds
		    long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

/** \brief same as timedMessageBox, but not Modal --> no return val
 * \return wxOK|wxCANCEL|wxYES|wxNO according to option chosen
 */
void timedMessageBoxNoModal(int whichIcon, const wxString& message,
			    const wxString& caption = wxMessageBoxCaptionStr, unsigned int delay = 3000, // miliseconds
			    long style = wxOK | wxICON_INFORMATION, const int x = -1, const int y = -1);

//! cleanup
void freeStaticBox();


/** \brief used to display information throughout the app
 * almost identical to wx's own dialog except for the possibility
 * to set a custom icon
 */
class CustomMessageBox : public wxDialog
{
public:
	CustomMessageBox(wxIcon* icon, wxWindow* parent, const wxString& message,
			 const wxString& caption = wxMessageBoxCaptionStr,
			 long style = wxOK | wxICON_INFORMATION, const wxPoint& pos = wxDefaultPosition);
	virtual ~CustomMessageBox();

protected:
	void OnOptionsNo(wxCommandEvent& event);
	void OnCloseEvent(wxCloseEvent& event);
	void CloseDialog();

private:
	wxBoxSizer* m_topsizer;
	wxBoxSizer* m_icon_text;
	DECLARE_EVENT_TABLE()
};

/** \brief MessageBox variant with timer
 */
class TimedMessageBox : public wxDialog
{
public:
	TimedMessageBox(int whichIcon, wxWindow* parent, const wxString& message,
			const wxString& caption = wxMessageBoxCaptionStr,
			unsigned int delay = 3000, // miliseconds
			long style = wxOK | wxICON_INFORMATION, const wxPoint& pos = wxDefaultPosition);
	virtual ~TimedMessageBox();

	void OnOptionsNo(wxCommandEvent& event);

protected:
	wxTimer m_delay_timer;

private:
	const int m_delay;
	wxSizer* sizerBtn;
	wxSizer* topsizer;
	wxSizer* m_delay_sizer;
	wxStaticText* m_delay_notif;
	wxTimer m_display_timer;
	unsigned int m_display_hits;
	static const unsigned int m_update_interval = 500;
	void OnUpdate(wxTimerEvent& evt);
	void OnUnlock(wxTimerEvent& evt);
	void OnClose(wxCloseEvent& evt);

	TimedMessageBox(const TimedMessageBox&);
	const TimedMessageBox& operator=(const TimedMessageBox&);

	DECLARE_EVENT_TABLE()
};

class AutocloseMessageBox : public TimedMessageBox
{
public:
	AutocloseMessageBox(wxWindow* parent, const wxString& message,
			    const wxString& caption = wxMessageBoxCaptionStr,
			    unsigned int delay = 3000, // miliseconds
			    long style = wxOK | wxICON_INFORMATION, const wxPoint& pos = wxDefaultPosition);
	virtual ~AutocloseMessageBox();
	virtual void EndModal(int retCode);

private:
	void OnUnlock(wxTimerEvent& evt);
	wxWindowID delay_timerID;
};

/** \brief used to display server messages when no chatwindow has focus
 */
class ServerMessageBox : public wxDialog, public SL::NonCopyable
{
public:
	ServerMessageBox(wxIcon* icon, wxWindow* parent, const wxString& message,
			 const wxString& caption = wxMessageBoxCaptionStr,
			 long style = wxOK, const wxPoint& pos = wxDefaultPosition);
	virtual ~ServerMessageBox();

	virtual void AppendMessage(const wxString& message);

private:
	wxBoxSizer* topsizer;
	wxListCtrl* m_messages;
};

/** \brief displays user action notifications */
class ActNotifBox : public ServerMessageBox
{
public:
	ActNotifBox(wxIcon* icon, wxWindow* parent, const wxString& message,
		    const wxString& caption = _T("User action notification"),
		    long style = wxOK, const wxPoint& pos = wxDefaultPosition);
	virtual ~ActNotifBox();

	virtual void AppendMessage(const wxString& message);
};

/** \brief displays channel mutelist */
class MutelistWindow : public ServerMessageBox
{
public:
	MutelistWindow(wxIcon* icon, wxWindow* parent, const wxString& message,
		       const wxString& caption = _T("User action notification"),
		       long style = wxOK, const wxPoint& pos = wxDefaultPosition);
	virtual ~MutelistWindow();

	//virtual void AppendMessage(const wxString& message);
};

/** \brief encapsulates pointers common to ServerMessageBox and CustomMessageBox
 * \todo this isn't really that well designed, make a real base class of this?
 */
class CustomMessageBoxBase
{
public:
	static void setLobbypointer(wxWindow*);
	static void setSettingspointer(wxWindow*);
	static wxWindow* getLobbypointer();
	static wxWindow* getSettingspointer();
	void AppendMessage(const wxString& message);

private:
	static wxWindow* m_settingsWindow;
	static wxWindow* m_lobbyWindow;
	wxListCtrl* m_messages;
};

/** \brief A generic Credits dialog
 * See showCredits() for modal use
 */
class CreditsDialog : public wxDialog, public SL::NonCopyable
{
public:
	CreditsDialog(wxWindow* parent, wxString title, int whichIcon);
	virtual ~CreditsDialog();
	/** \brief add a Name|Reason line to the dialog */
	void AddCredit(wxString, wxString);

private:
	wxTextCtrl* text_ctrl;

	CreditsDialog(const CreditsDialog&);
};

//! extends the wx method by allowing to set selection
int GetSingleChoiceIndex(const wxString& message,
			 const wxString& caption,
			 const wxArrayString& choices,
			 const int selected,
			 wxWindow* parent = NULL,
			 int x = wxDefaultCoord,
			 int y = wxDefaultCoord,
			 bool centre = true);


#endif /*CUSTOM_MSG_DLG_H_*/
