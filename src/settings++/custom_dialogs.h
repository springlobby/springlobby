#ifndef CUSTOM_MSG_BOX_H_
#define CUSTOM_MSG_BOX_H_

#include <wx/msgdlg.h>
#include <wx/defs.h>
#include <wx/dialog.h>

#define SL_MAIN_ICON 1
#define SS_MAIN_ICON 2

class wxIcon;
class wxWindow;
class wxPoint;
class wxString;
class wxTextCtrl;
class wxCommandEvent;
class wxCloseEvent;
class wxBoxSizer;
class wxListCtrl;

#define SL_MAIN_WINDOW_PTR CustomMessageBox::getLobbypointer()
#define SE_FRAME_PTR CustomMessageBox::getSettingspointer()

int customMessageBox(int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );
void customMessageBoxNoModal(int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );
void serverMessageBox(int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxICON_INFORMATION,  const int x = -1, const int y = -1 );

void freeStaticBox();



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

class ServerMessageBox : public wxDialog
{
public:
	ServerMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
	        const wxString& caption = wxMessageBoxCaptionStr,
	        long style = wxOK, const wxPoint& pos = wxDefaultPosition);
	virtual ~ServerMessageBox();

    void AppendMessage(const wxString& message);

protected:

	wxBoxSizer* topsizer;
	wxListCtrl* m_messages;

};

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

class CreditsDialog: public wxDialog
{
public:
	CreditsDialog(wxWindow* parent,wxString title, int whichIcon);
	virtual ~CreditsDialog();
	void AddCredit(wxString,wxString);

private:
	wxTextCtrl* text_ctrl;
};

#endif /*CUSTOM_MSG_DLG_H_*/
