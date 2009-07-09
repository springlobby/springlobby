#ifndef CUSTOM_MSG_BOX_H_
#define CUSTOM_MSG_BOX_H_

#include <wx/msgdlg.h>
#include <wx/defs.h>
#include <wx/dialog.h>
#include <wx/timer.h>
#include <wx/panel.h>

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
int customMessageBox(int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );

/** \brief utlity function to display modal messagebox
 * the dialog itself is statically allocated, therefore only one
 * one of these may exist at any given time. \n
 * It is possible to use this with other styles than wxOK|wxICON_INFORMATION,
 * but since it's displayed non-modal nothing would come of it
 */
void customMessageBoxNoModal(int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );

/** \brief displays server messages when no chat window has focus
 * If dialog currently isn't shown, it's brought up. If dialog already is shown (not necessarily having focus)
 * message is appended, rather than old box replaced with new.
 */
void serverMessageBox(int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );

/** \brief displays user action notifications
 * If dialog currently isn't shown, it's brought up. If dialog already is shown (not necessarily having focus)
 * message is appended, rather than old box replaced with new.
 */
void actNotifBox(int whichIcon , const wxString& message,
        const wxString& caption = _T("User action notification"),
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );

/** \brief show  mutelist for a specific channel
 *
 */
void mutelistWindow( const wxString& message,
        const wxString& caption = _T("Mutelist"),
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );

//! cleanup
void freeStaticBox();


/** \brief used to display information throughout the app
 * almost identical to wx's own dialog except for the possibility
 * to set a custom icon
 */
class CustomMessageBox : public wxDialog
{
public:
	CustomMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
	        const wxString& caption = wxMessageBoxCaptionStr,
	        long style = wxOK|wxICON_INFORMATION, const wxPoint& pos = wxDefaultPosition);
	virtual ~CustomMessageBox();

    void OnOptionsNo(wxCommandEvent& event);

protected:

     DECLARE_EVENT_TABLE()

};

/** \brief used to display server messages when no chatwindow has focus
 */
class ServerMessageBox : public wxDialog
{
public:
	ServerMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
	        const wxString& caption = wxMessageBoxCaptionStr,
	        long style = wxOK, const wxPoint& pos = wxDefaultPosition);
	virtual ~ServerMessageBox();

    virtual void AppendMessage(const wxString& message);

protected:

	wxBoxSizer* topsizer;
	wxListCtrl* m_messages;

};

/** \brief displays user action notifications */
class ActNotifBox : public ServerMessageBox
{
public:
    ActNotifBox (wxIcon* icon ,wxWindow *parent, const wxString& message,
	        const wxString& caption = _T("User action notification") ,
	        long style = wxOK, const wxPoint& pos = wxDefaultPosition);
    virtual ~ActNotifBox ();

    virtual void AppendMessage(const wxString& message);
};

/** \brief displays channel mutelist */
class MutelistWindow : public ServerMessageBox
{
public:
    MutelistWindow (wxIcon* icon ,wxWindow *parent, const wxString& message,
	        const wxString& caption = _T("User action notification") ,
	        long style = wxOK, const wxPoint& pos = wxDefaultPosition);
    virtual ~MutelistWindow ();

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

protected:
	static wxWindow* m_settingsWindow;
	static wxWindow* m_lobbyWindow;
	wxListCtrl* m_messages;

};

/** \brief A generic Credits dialog
 * See showCredits() for modal use
 */
class CreditsDialog: public wxDialog
{
public:
	CreditsDialog(wxWindow* parent,wxString title, int whichIcon);
	virtual ~CreditsDialog();
	/** \brief add a Name|Reason line to the dialog */
	void AddCredit(wxString,wxString);

private:
	wxTextCtrl* text_ctrl;
};

//! extends the wx method by allowing to set selection
int GetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& aChoices,
                            const int selected,
                            wxWindow *parent = NULL,
                            int x = wxDefaultCoord,
                            int y = wxDefaultCoord,
                            bool centre = true );

class ActivityNoticePanel: public wxPanel
{
    public:
        ActivityNoticePanel(wxWindow* parent,const wxString& file,const wxString& format = _("Caching file %s please wait") );
        virtual ~ActivityNoticePanel() {}

        void SetString(const wxString& file);
        virtual bool Show(bool show = true);
    protected:
        wxString m_filename;
        wxString m_format;
        wxGauge* m_gauge;
        wxStaticText* m_message;
//        wxStaticText* m_format;
        wxTimer m_timer;

        void OnTimer(wxTimerEvent& event);

        DECLARE_EVENT_TABLE()
};

class ActivityNotice: public wxDialog
{
    public:
        ActivityNotice(wxWindow* parent,const wxString& file, const wxString& format = _("Caching file %s please wait"));
        virtual ~ActivityNotice() {}
    protected:
        ActivityNoticePanel* m_panel;
};

//! use this for a notice that closes automatically when going out of scope
class ActivityNoticeContainer
{
    public:
        ActivityNoticeContainer(wxWindow* parent,const wxString& file, const wxString& format = _("Caching file %s please wait"));
        ~ActivityNoticeContainer();

    protected:
        ActivityNotice* m_window;
};

typedef std::auto_ptr<ActivityNoticeContainer>
    ScopedActivityNotice;

ScopedActivityNotice scopedActivityNotice(wxWindow* parent,const wxString& file, const wxString& format = _("Caching file %s please wait"));

#endif /*CUSTOM_MSG_DLG_H_*/

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

