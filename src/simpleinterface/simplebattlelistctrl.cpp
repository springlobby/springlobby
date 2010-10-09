#include "simplebattlelistctrl.h"

#include "../settings.h"
#include "../ibattle.h"
#include "../battlelistfilter.h"
#include "../customizations.h"

template<> SortOrder CustomVirtListCtrl<IBattle*,SimpleBattlelistCtrl>::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE(SimpleBattlelistCtrl, SimpleBattlelistCtrl::BaseType )

END_EVENT_TABLE()

SimpleBattlelistCtrl::SimpleBattlelistCtrl( wxWindow* parent )
	: CustomVirtListCtrl< IBattle *,SimpleBattlelistCtrl>(parent, SI_BLIST_LIST, wxDefaultPosition, wxSize(150,360),
														  wxLC_NO_HEADER | wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT,
														  _T("SimpleBattleListCtrl"), 2, &SimpleBattlelistCtrl::CompareOneCrit,
			false /*highlight*/, UserActions::ActHighlight, true /*periodic sort*/ ),
	m_addSink( this, &BattleEvents::GetBattleEventSender( ( BattleEvents::AddBattle ) ) ),
	m_removeSink( this, &BattleEvents::GetBattleEventSender( ( BattleEvents::RemoveBattle ) ) ),
	m_updateSink( this, &BattleEvents::GetBattleEventSender( ( BattleEvents::UpdateBattle ) ) )
{

	const int hd = wxLIST_AUTOSIZE_USEHEADER;

#if defined(__WXMAC__)
/// on mac, autosize does not work at all
	const int widths[10] = {20,20,20,170,140,130,110,28,28,28};
#else
	const int widths[10] = {150,hd,hd,170,140,130,110,hd,hd,hd};
#endif

	AddColumn( 0, widths[0], _T("Description"), _T("Description") );
	//AddColumn( 1, widths[1], _T("Country"), _T("Country") );

	if ( m_sortorder.size() == 0 ) {
		m_sortorder[0].col = 0;
		m_sortorder[0].direction = true;
	}
}

wxString SimpleBattlelistCtrl::GetItemText(long item, long column) const
{
	if ( m_data[item] == NULL )
		return wxEmptyString;

	const IBattle& battle= *m_data[item];

	switch( column ) {
		case 0: return battle.GetDescription();
		default: return wxEmptyString;
	}
}

int SimpleBattlelistCtrl::GetItemImage(long /*item*/) const
{
	return -1;
}

int SimpleBattlelistCtrl::GetItemColumnImage(long /*item*/, long /*column*/) const
{
	return -1;
}

wxListItemAttr * SimpleBattlelistCtrl::GetItemAttr(long /*item*/) const
{
	return 0;
}

void SimpleBattlelistCtrl::AddBattle( BattleEvents::BattleEventData data )
{
	wxString filter = SLcustomizations().GetCustomizations().getSingleValue( _T("battlelist_filter") );
	if ( BattleListFilter::StringMatches(data.first->GetHostModName(),filter) )
		AddItem( data.first );
}

void SimpleBattlelistCtrl::RemoveBattle( BattleEvents::BattleEventData data )
{
	RemoveItem( data.first );
}

void SimpleBattlelistCtrl::UpdateBattle( BattleEvents::BattleEventData data )
{
	int index = GetIndexFromData( data.first );

	RefreshItem( index );
	MarkDirtySort();
}

void SimpleBattlelistCtrl::Sort()
{
	if ( m_data.size() > 0 )
	{
		SaveSelection();
		SLInsertionSort( m_data, m_comparator );
		RestoreSelection();
	}
}

int SimpleBattlelistCtrl::CompareOneCrit( DataType /*u1*/, DataType /*u2*/, int /*col*/, int /*dir*/ ) const
{
	return 0;
}

int SimpleBattlelistCtrl::GetIndexFromData( const DataType& data ) const
{
	static long seekpos;
	seekpos = clamp( seekpos, 0l , (long)m_data.size() );
	int index = seekpos;

	for ( DataCIter f_idx = m_data.begin() + seekpos; f_idx != m_data.end() ; ++f_idx )
	{
		if ( *f_idx != 0 && data->Equals( *(*f_idx) ) )
		{
			seekpos = index;
			return seekpos;
		}
		index++;
	}
	//it's ok to init with seekpos, if it had changed this would not be reached
	int r_index = seekpos - 1;
	for ( DataRevCIter r_idx = m_data.rbegin() + ( m_data.size() - seekpos ); r_idx != m_data.rend() ; ++r_idx )
	{
		if ( *r_idx != 0 && data->Equals( *(*r_idx) ) )
		{
			seekpos = r_index;
			return seekpos;
		}
		r_index--;
	}

	return -1;
}
