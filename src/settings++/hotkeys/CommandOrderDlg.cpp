#include "CommandOrderDlg.h"
#include "../../utils/customdialogs.h"
#include "keybinder.h"

CommandOrderDlg::CommandOrderDlg( const wxString& key, const CmdSet& cmds, wxWindow* parent ) :
						CommandOrderDlgBase( parent ), m_key( key ), m_cmds( cmds ), m_normKeyCount(0)
{
}

void CommandOrderDlg::OnInitDialog( wxInitDialogEvent& )
{
	this->updateOrderMap();

	this->fillCommandList();

	this->SetTitle( this->GetTitle() + wxT(" - Configure Key: ") + this->m_key );
}

const CommandOrderDlg::ListIndexCmdMap& CommandOrderDlg::getOrderedCommands() const
{
	return this->m_prioCmdMap;
}

void CommandOrderDlg::fillCommandList()
{
	this->m_listBoxCommands->Clear();

	for( size_t i=0; i < this->m_prioCmdMap.size(); ++i )
	{
		wxString text = this->m_prioCmdMap[i].command;
		if ( this->m_prioCmdMap[i].any )
		{
			text = wxT("(Any) ") + text;
		}

		this->m_listBoxCommands->Append( text ); 
	}	
}

void CommandOrderDlg::updateOrderMap()
{
	typedef std::map<size_t, wxString> SortedCmds;
	
	size_t listIdx = 0;
	m_normKeyCount = 0;

	{	//add "normal"-keys first (do not show normal keys when user clicked an ANY-key)
		if ( !this->m_key.StartsWith( wxT("Any+") ) )
		{
			SortedCmds tmp;
			for ( CmdSet::const_iterator iter = m_cmds.begin(); iter != m_cmds.end(); ++iter )
			{		
				for( int i = 0; i < (*iter)->GetShortcutCount(); ++i )
				{
					const wxKeyBind* pKb = (*iter)->GetShortcut( i );
					if ( !pKb->HasAnyModifier() && pKb->Match(this->m_key ) )
					{
						tmp[pKb->GetOrderIndex()] = (*iter)->GetName();
						++m_normKeyCount;
					}
				}
			}

			for( SortedCmds::const_iterator iter = tmp.begin(); iter != tmp.end(); ++iter )
			{
				this->m_prioCmdMap[listIdx++] = CmdInfo( iter->second, false, iter->first );
			}
		}
	}

	{	//add "any"-keys
		SortedCmds tmp;
		for ( CmdSet::const_iterator iter = m_cmds.begin(); iter != m_cmds.end(); ++iter )
		{		
			for( int i = 0; i < (*iter)->GetShortcutCount(); ++i )
			{
				const wxKeyBind* pKb = (*iter)->GetShortcut( i );
				if ( pKb->HasAnyModifier() && pKb->Match( this->m_key ) )
				{
					tmp[pKb->GetOrderIndex()] = (*iter)->GetName();
				}
			}
		}

		for( SortedCmds::const_iterator iter = tmp.begin(); iter != tmp.end(); ++iter )
		{
			this->m_prioCmdMap[listIdx++] = CmdInfo( iter->second, true, iter->first );
		}
	}
}

void CommandOrderDlg::OnButtonUpClick( wxCommandEvent& )
{
	const int selIdx = this->m_listBoxCommands->GetSelection();
	
	if ( selIdx == wxNOT_FOUND || selIdx <= 0 )
	{
		return;
	}

	if ( selIdx == static_cast<int>( m_normKeyCount ) )
	{
		//we cant move up the command into "normal"-area
		return;
	}

	assert( selIdx > 0 );
	const CmdInfo tmp = this->m_prioCmdMap[selIdx-1];
	this->m_prioCmdMap[selIdx-1] = this->m_prioCmdMap[selIdx];
	this->m_prioCmdMap[selIdx] = tmp;

	this->fillCommandList();
	this->m_listBoxCommands->Select( selIdx - 1 );
}

void CommandOrderDlg::OnButtonDownClick( wxCommandEvent& )
{
	const int selIdx = this->m_listBoxCommands->GetSelection();
	
	if ( selIdx == wxNOT_FOUND || static_cast<unsigned int>( selIdx+2 ) > this->m_listBoxCommands->GetCount() )
	{
		return;
	}

	if ( ( selIdx + 1) == static_cast<int>( m_normKeyCount ) )
	{
		//we cant move up the any-command into "any"-area
		return;
	}

	//remember to convert to 1-based order indices
	const CmdInfo tmp = this->m_prioCmdMap[selIdx];
	this->m_prioCmdMap[selIdx] = this->m_prioCmdMap[selIdx+1];
	this->m_prioCmdMap[selIdx+1] = tmp;

	this->fillCommandList();
	this->m_listBoxCommands->Select( selIdx + 1 );
}

void CommandOrderDlg::OnButtonOkClick( wxCommandEvent& )
{
	this->EndModal( wxID_OK );
}
