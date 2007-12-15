#ifndef CUSTOM_MSG_BOX_H_
#define CUSTOM_MSG_BOX_H_

#include <wx/msgdlg.h>
#include <wx/defs.h>

#define SL_MAIN_ICON 1
#define SS_MAIN_ICON 2

class wxIcon;
class wxWindow;
class wxPoint;
class wxString;

int customMessageBox( int whichIcon , const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxCENTRE, wxWindow *parent = NULL, const int x = -1, const int y = -1 );

class CustomMessageBox : public wxMessageDialog
{
public:
	CustomMessageBox(wxIcon* icon ,wxWindow *parent, const wxString& message,
	        const wxString& caption = wxMessageBoxCaptionStr,
	        long style = wxOK|wxCENTRE, const wxPoint& pos = wxDefaultPosition);
	virtual ~CustomMessageBox();
};


#endif /*CUSTOM_MSG_DLG_H_*/
