#ifndef __CommandOrderDlg__
#define __CommandOrderDlg__

#include "HotkeyForms.h"
#include "HotkeyTypes.h"
#include <wx/string.h>

class CmdSet;

class CommandOrderDlg : public CommandOrderDlgBase 
{
public:
	CommandOrderDlg( const wxString& command, const CmdSet& cmds, wxWindow* parent );
	
	struct CmdInfo
	{
		CmdInfo() : globalSortIdx(0), any(false) {}

		CmdInfo(const wxString& cmdIn, bool anyIn, size_t globSortIdxIn ) : 
			command(cmdIn), globalSortIdx(globSortIdxIn), any(anyIn)
		{
		}

		wxString	command;
		size_t		globalSortIdx;
		bool		any;
	};

	typedef std::pair<size_t, wxString>				PrioCommandPair;	//couples the order index with the command
	typedef std::map<size_t, CmdInfo>				ListIndexCmdMap;
	const ListIndexCmdMap& getOrderedCommands() const;

protected:
	void updateOrderMap();
	void fillCommandList();
	
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnButtonUpClick( wxCommandEvent& event );
	virtual void OnButtonDownClick( wxCommandEvent& event );
	virtual void OnButtonOkClick( wxCommandEvent& event );

	const wxString								m_key;
	const CmdSet&								m_cmds;

	size_t											m_normKeyCount; //track any count to know how the list can be modified
	ListIndexCmdMap									m_prioCmdMap;
};

#endif //__AddSelectionPanelBase__
