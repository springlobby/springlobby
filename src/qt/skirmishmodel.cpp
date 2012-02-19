#include "skirmishmodel.h"

#include "../customizations.h"
#include "../springunitsync.h"
#include "../springunitsynclib.h"

#include <cstdlib>

SkirmishModel::SkirmishModel(QObject *parent)
	: QAbstractListModel(parent),
	m_mod_customs( SLcustomizations().GetCustomizations() )
{
    m_battle.SetHostMod( SLcustomizations().Archive(), wxEmptyString );
    bool loaded = m_battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, SLcustomizations().Archive() );
    assert ( loaded );
	//this block populates the radiobox and loads the skirmish options into the map
    std::string md = SLcustomizations().DataBasePath().append( "/skirmish/" ).toStdString();
    wxArrayString skirmishes = susynclib().DirListVFS( TowxString(md), _T("*.lua"), _T("r") );//only raw vfs part

    foreach ( const wxString skirmish_file, skirmishes )
    {
        wxString skirmish_name = skirmish_file.AfterLast('/').BeforeLast('.');
        OptionsWrapper temp;
        loaded = temp.loadOptions( OptionsWrapper::SkirmishOptions, SLcustomizations().Archive(),
                                   wxString(_T("skirmish/")) + skirmish_name + wxString(_T(".lua")) );
        assert( loaded );
        m_skirmishes.push_back( std::make_pair( skirmish_name, temp ) );
	}

//    {
//        mmOptionList suggested_maps;
//        mmOptionList suggested_sides;
//        OptionsWrapper map_op = m_skirmishes.begin()->second;
//        for ( SpringUnitSync::OptionMapListConstIter it = map_op.m_opts[optFlag].list_map.begin();
//              it != map_op.m_opts[optFlag].list_map.end(); ++it) {
//            mmOptionList current = it->second;
//            if ( _T("suggested_maps") == current.key ) {
//                suggested_maps = current;
//            }
//            else if ( _T("suggested_sides") == current.key ) {
//                suggested_sides = current;
//            }
//        }
//    }
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
			const wxString desc = opts.getSingleValue( _T("description") );
			return QVariant::fromValue( QString(desc.mb_str()) );
		}
		case Name:
		default: {
			return FromwxString<QVariant>( m_skirmishes[row].first );
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
		const mmOptionList current = it->second;
		if ( current.key == _T("ai_sides") ) {
			for ( ListItemVec::const_iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor ) {
				ai_sides.push_back( itor->name );
			}
			break;
		}
	}

	wxString default_ai = m_mod_customs.getSingleValue( _T("default_ai" ) );
	std::vector<wxString> ai_names ( ai_sides.size(), default_ai );
	for ( SpringUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
		const mmOptionList current = it->second;
		if ( current.key == _T("ai_names") ) {
			for ( ListItemVec::const_iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor) {
				const size_t idx = FromwxString<size_t>( itor->key );
				if ( idx < ai_sides.size() )
					ai_names[idx] = itor->name;
			}
			break;
		}
	}

	for ( SpringUnitSync::OptionMapListConstIter it = opts.m_opts[optFlag].list_map.begin(); it != opts.m_opts[optFlag].list_map.end(); ++it) {
		const mmOptionList current = it->second;
		if ( current.key == _T("ai_team_ids") ) {
			size_t i = 0;
			for ( ListItemVec::const_iterator itor = current.listitems.begin(); itor != current.listitems.end(); ++itor, ++i ) {
				ASSERT_EXCEPTION( i < ai_sides.size(), _T("The setup is listing more AI opponents than AI sides") );
				const wxString ai = ai_names.size() > i ? ai_names[i] : default_ai;
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
