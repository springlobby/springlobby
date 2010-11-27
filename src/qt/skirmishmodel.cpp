#include "skirmishmodel.h"

#include "../customizations.h"
#include "../springunitsync.h"
#include "../springunitsynclib.h"

#include <cstdlib>

SkirmishModel::SkirmishModel(QObject *parent)
	: QAbstractListModel(parent),
	m_mod_customs( SLcustomizations().GetCustomizations() )
{
	m_battle.SetHostMod( SLcustomizations().GetModname(), wxEmptyString );
	m_battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, SLcustomizations().GetModname() );
	const wxString sk_dir = m_mod_customs.getSingleValue( _T("skirmish_directory"), OptionsWrapper::ModCustomizations );

	//this block populates the radiobox and loads the skirmish options into the map
	OptionsWrapper::GameOption optFlag = OptionsWrapper::ModCustomizations;
	for ( IUnitSync::OptionMapListConstIter it = m_mod_customs.m_opts[optFlag].list_map.begin(); it != m_mod_customs.m_opts[optFlag].list_map.end(); ++it) {
		mmOptionList current = it->second;
		if ( _T("scenarios") == current.key ) {

			wxString tooltip;
			int i = 0;
			for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor )
			{
				tooltip+= _T("\n") + itor->name + _T(": ") + itor->desc;
				OptionsWrapper temp;
				wxString filename = sk_dir + _T("/") + itor->key + _T(".lua") ;
				temp.loadOptions( OptionsWrapper::SkirmishOptions, SLcustomizations().GetModname(), filename );
				m_skirmishes.push_back( std::make_pair( itor->name, temp ) );
				i++;
			}
//            m_scenario_choice->SetToolTip(TE(tooltip));

//            m_scenario_choice->SetName(current.key);

			break;
		}
	}

	optFlag = OptionsWrapper::SkirmishOptions;
	mmOptionList suggested_maps;
	mmOptionList suggested_sides;
	OptionsWrapper map_op = m_skirmishes.begin()->second;
	for ( IUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
		mmOptionList current = it->second;
		if ( _T("suggested_maps") == current.key ) {
			suggested_maps = current;
		}
		else if ( _T("suggested_sides") == current.key ) {
			suggested_sides = current;
		}
	}
	std::srand(time(NULL));

	QHash<int, QByteArray> roles;
	roles[Description] = "description";
	roles[Name] = "skirmish_name";
	setRoleNames(roles);

}

int SkirmishModel::rowCount(const QModelIndex &parent ) const
{
	return m_skirmishes.size();
}

QVariant SkirmishModel::data(const QModelIndex &index, int role ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= m_skirmishes.size() )
		   return QVariant();
	switch ( role ) {
		case Description: {
			const OptionsWrapper& opts = m_skirmishes[row].second;
			wxString desc = opts.getSingleValue( _T("description") );
			return QVariant::fromValue( QString(desc.mb_str()) );
		}
		case Name:
		default: {
			wxString name = m_skirmishes[row].first;
			return QVariant::fromValue( QString(name.mb_str()) );
		}
	}
}
