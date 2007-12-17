#include "custom_msgbox.h"
#include <wx/icon.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/defs.h>

#include "../images/springsettings.xpm"
#include "../images/springlobby.xpm"

CustomMessageBox::CustomMessageBox(wxIcon* icon  ,wxWindow *parent, const wxString& message,
        const wxString& caption ,
        long style, const wxPoint& pos ) 
			: wxMessageDialog(parent,message,caption,style,pos)
{
	SetIcon(*icon);
}

CustomMessageBox::~CustomMessageBox()
{
}

int customMessageBox(int whichIcon, const wxString& message,const wxString& caption,
        long style , wxWindow *parent,int x, int y )
{
		wxIcon* icon;
		switch (whichIcon)
		{
			case SL_MAIN_ICON: icon = new wxIcon(springlobby_xpm); break;
			case SS_MAIN_ICON: icon = new wxIcon(springsettings_xpm); break;
			default: icon = new wxIcon(wxNullIcon); break;
					
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
