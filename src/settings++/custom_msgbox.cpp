#include "custom_msgbox.h"
#include <wx/icon.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/defs.h>

#include "../images/springsettings.xpm"
#include "../images/springlobby.xpm"
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

int customMessageBox( int whichIcon , const wxString& message,const wxString& caption,
		long style , int x, int y )
{
		wxWindow* parent;
		wxIcon* icon;
		switch (whichIcon)
		{
			case SL_MAIN_ICON: 
				icon = new wxIcon(springlobby_xpm); 
				parent = CustomMessageBox::getLobbypointer();
				break;
			case SS_MAIN_ICON: 
				icon = new wxIcon(springsettings_xpm);
				parent = CustomMessageBox::getSettingspointer();
				break;
			default: 
				icon = new wxIcon(wxNullIcon); 
				parent = 0;
				break;
					
		}
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
