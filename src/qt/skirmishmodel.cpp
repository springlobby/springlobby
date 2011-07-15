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
	for ( SpringUnitSync::OptionMapListConstIter it = m_mod_customs.m_opts[optFlag].list_map.begin(); it != m_mod_customs.m_opts[optFlag].list_map.end(); ++it) {
		mmOptionList current = it->second;
		if ( _T("scenarios") == current.key ) {

			wxString tooltip;
			int i = 0;
			for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor )
			{
				tooltip+= _T("\n") + itor->name + _T(": ") + itor->desc;
				OptionsWrapper temp;
				wxString filename = sk_dir + _T("/") + itor->key + _T(".lua") ;
				temp.loadOptions( OptionsWrapper::SkirmishOptions, SLcustomizations().Archive(), filename );
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
	assert( m_skirmishes.size() );
	OptionsWrapper map_op = m_skirmishes.begin()->second;
	for ( SpringUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin(); it != map_op.m_opts[optFlag].list_map.end(); ++it) {
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

int SkirmishModel::rowCount(const QModelIndex &/*parent*/ ) const
{
	return m_skirmishes.size();
}

QVariant SkirmishModel::data(const QModelIndex &index, int role ) const
{
	int row =  index.row();
	if ( !index.isValid() || row >= int(m_skirmishes.size()) )
		   return QVariant();
	const OptionsWrapper& opts = m_skirmishes[row].second;
	switch ( role ) {
		case Description: {
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

void SkirmishModel::run( const int id, const int side, const int map )
{
	assert( id < int(m_skirmishes.size()) );
	OptionsWrapper& opts = m_skirmishes.at(id).second;
	// this overwrites any modoptions with those found in the skirmish definition
	m_battle.CustomBattleOptions().MergeOptions( opts, OptionsWrapper::ModOption );

	//now add AIs
	OptionsWrapper::GameOption optFlag = OptionsWrapper::SkirmishOptions;
	// we need to store Sides for AIs first, so we can later add them in batch w/o needing to remember a mapping
	std::vector<wxString> ai_sides;
	for ( SpringUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
		mmOptionList current = it->second;
		if ( current.key == _T("ai_sides") ) {

			for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor ) {
				ai_sides.push_back( itor->name );
			}
		break;
		}
	}

	wxString default_ai = m_mod_customs.getSingleValue( _T("default_ai" ) );
	std::vector<wxString> ai_names ( ai_sides.size(), default_ai );
	for ( SpringUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
		mmOptionList current = it->second;
		if ( current.key == _T("ai_names") ) {
			for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor) {
				size_t idx = FromwxString<size_t>( itor->key );
				if ( idx < ai_sides.size() )
					ai_names[idx] = itor->name;
			}
		break;
		}
	}

	for ( SpringUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
		mmOptionList current = it->second;
		if ( current.key == _T("ai_team_ids") ) {

			size_t i = 0;
			for ( ListItemVec::iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor, ++i ) {
				ASSERT_EXCEPTION( i < ai_sides.size(), _T("The setup is listing more AI opponents than AI sides") );
				wxString ai = ai_names.size() > i ? ai_names[i] : default_ai;
				m_battle.AddBot( ai, FromwxString<int>( itor->name ), ai_sides[i] );
			}
		break;
		}
	}

	User& me = m_battle.GetMe();
	me.BattleStatus().side = side;

//    if ( m_map_random->IsChecked() )
//        m_map->SetSelection( std::rand() % ( m_map->GetCount() ) ); //if anyone complains about this not being a uniform distribution imma invoke stab-over-tcp ((c) BD )
	UnitSyncMap mapname = usync().GetMap( map );
	m_battle.SetHostMap( mapname.name, mapname.hash );
	m_battle.StartSpring();
}
