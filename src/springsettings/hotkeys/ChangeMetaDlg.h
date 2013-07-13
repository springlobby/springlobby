#ifndef __ChangeMetaDlg__
#define __ChangeMetaDlg__

#include "HotkeyForms.h"
#include "HotkeyTypes.h"
#include <wx/string.h>

#define CHANGEMETADLG_KEY_PRESSED_ID	40001

class ChangeMetaDlg : public ChangeMetaDlgBase 
{
public:
	ChangeMetaDlg( const wxString& curMeta, wxWindow* parent );
	
	const wxString& GetNewMetaKey() const;

protected:
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnKeyPressed(wxKeyEvent &);

	wxString		m_curMetaKey;
	wxString		m_newMetaKey;
};

#endif //__AddSelectionPanelBase__
