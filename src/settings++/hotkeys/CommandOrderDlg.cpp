#include "CommandOrderDlg.h"
#include "../../utils/customdialogs.h"


CommandOrderDlg::CommandOrderDlg( const wxString& key, const CmdSet& cmds, wxWindow* parent ) :
						CommandOrderDlgBase( parent ), m_key( key ), m_cmds( cmds ) 
{
}

void CommandOrderDlg::OnInitDialog( wxInitDialogEvent& )
{
	this->updateOrderMap();

	this->fillCommandList();

	this->SetTitle( this->GetTitle() + wxT(" - ") + this->m_key );
}

void CommandOrderDlg::fillCommandList()
{
	this->m_listBoxCommands->Clear();

	for( CommandOrderMap::const_iterator iter = this->m_cmdOrderMap.begin(); iter != this->m_cmdOrderMap.end(); ++iter )
	{
		this->m_listBoxCommands->Append( iter->second ); 
	}	
}

unsigned int CommandOrderDlg::getOrderIndexByCommand( const wxString& cmd ) const
{
	for( CommandOrderMap::const_iterator iter = this->m_cmdOrderMap.begin(); iter != this->m_cmdOrderMap.end(); ++iter )
	{
		if ( iter->second == cmd )
		{
			return iter->first;
		}
	}

	return 0; //not found
}

void CommandOrderDlg::updateOrderMap()
{
	for ( CmdSet::const_iterator iter = m_cmds.begin(); iter != m_cmds.end(); ++iter )
	{		
		for( int i = 0; i < (*iter)->GetShortcutCount(); ++i )
		{
			const wxKeyBind* pKb = (*iter)->GetShortcut( i );
			const wxString& keyString = pKb->GetStr();
			if ( keyString == m_key )
			{
				this->m_cmdOrderMap[pKb->GetOrderIndex()] = (*iter)->GetName();
			}
		}
	}
}

void CommandOrderDlg::saveOrderMapToProfile()
{
	for ( CmdSet::const_iterator iter = m_cmds.begin(); iter != m_cmds.end(); ++iter )
	{	
		const unsigned orderIdx = this->getOrderIndexByCommand( (*iter)->GetName() );
		if ( orderIdx == 0 )
		{
			continue;
		}

		for( int i = 0; i < (*iter)->GetShortcutCount(); ++i )
		{
			wxKeyBind* pKb = (*iter)->GetShortcut( i );
			const wxString& keyString = pKb->GetStr();
			if ( keyString == m_key )
			{
				pKb->SetOrderIndex( orderIdx );
			}
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

	//remember to convert to 1-based order indices
	const wxString tmp = this->m_cmdOrderMap[selIdx];
	this->m_cmdOrderMap[selIdx] = this->m_cmdOrderMap[selIdx+1];
	this->m_cmdOrderMap[selIdx+1] = tmp;

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

	//remember to convert to 1-based order indices
	const wxString tmp = this->m_cmdOrderMap[selIdx+1];
	this->m_cmdOrderMap[selIdx+1] = this->m_cmdOrderMap[selIdx+2];
	this->m_cmdOrderMap[selIdx+2] = tmp;

	this->fillCommandList();
	this->m_listBoxCommands->Select( selIdx + 1 );
}

void CommandOrderDlg::OnButtonOkClick( wxCommandEvent& )
{
	this->saveOrderMapToProfile();

	this->EndModal( wxID_OK );
}
