#include "custom_msgbox.h"
#include <wx/icon.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/defs.h>

wxWindow* CustomMessageBox::m_settingsWindow = 0;
wxWindow* CustomMessageBox::m_lobbyWindow = 0;

CustomMessageBox::CustomMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos ) 
			: wxMessageDialog(parent,message,caption,style|wxFRAME_FLOAT_ON_PARENT,pos)
{
	SetIcon(*icon);
}

CustomMessageBox::~CustomMessageBox()
{
}

void CustomMessageBox::setLobbypointer(wxWindow* arg)
{
	m_lobbyWindow = arg;
}

 void CustomMessageBox::setSettingspointer(wxWindow* arg)
{
	m_settingsWindow = arg;
}

 wxWindow* CustomMessageBox::getLobbypointer()
{
	return m_lobbyWindow;
}

 wxWindow* CustomMessageBox::getSettingspointer()
{
	return m_settingsWindow;
}

int customMessageBox( wxWindow *parent, wxIcon* icon , const wxString& message,const wxString& caption,
		long style , int x, int y )
{
		CustomMessageBox dlg(icon,parent,message,caption,style,wxPoint(x,y));
		int re = dlg.ShowModal();
		switch (re)
		{
			case wxID_OK: return wxOK;
			case wxID_CANCEL: return wxCANCEL;
			case wxID_YES: return wxYES;
			case wxID_NO: return wxNO;
		}
		return -1;
}
