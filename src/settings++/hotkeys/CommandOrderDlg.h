#ifndef __AddSelectionPanelDlg__
#define __AddSelectionPanelDlg__

#include "HotkeyForms.h"
#include "HotkeyTypes.h"
#include <wx/string.h>
#include "keybinder.h"


class CommandOrderDlg : public CommandOrderDlgBase 
{
public:
	CommandOrderDlg( const wxString& command, const CmdSet& cmds, wxWindow* parent );
	
protected:
	void updateOrderMap();
	void fillCommandList();
	void saveOrderMapToProfile();
	unsigned int getOrderIndexByCommand( const wxString& cmd ) const;

	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnButtonUpClick( wxCommandEvent& event );
	virtual void OnButtonDownClick( wxCommandEvent& event );
	virtual void OnButtonOkClick( wxCommandEvent& event );

	const wxString								m_key;
	const CmdSet&								m_cmds;

	typedef std::map<unsigned int, wxString>	CommandOrderMap;
	CommandOrderMap								m_cmdOrderMap;
};

#endif //__AddSelectionPanelBase__
