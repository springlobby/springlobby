#include "mmoptionswrapper.h"

#include "springunitsync.h"
#include "utils/conversion.h"
#include "utils/customdialogs.h"
#include "utils/debug.h"

#include <stdexcept>
#include <clocale>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/fileconf.h>
#include <wx/filename.h>

OptionsWrapper::OptionsWrapper()
{
	unLoadOptions();
	loadOptions( EngineOption, _T("") );
	loadOptions( PrivateOptions,_T("") );
}

void OptionsWrapper::unLoadOptions()
{
	for (int i = 0; i < LastOption; ++i)
	{
		unLoadOptions( (GameOption)i );
	}
}

void OptionsWrapper::unLoadOptions(GameOption i)
{
  GameOptions empty;
  m_opts[i] = empty;

	mmSectionTreeMap::iterator itor = m_sections.find( i );
	if ( itor != m_sections.end() ) m_sections.erase( itor );
}

OptionsWrapper::~OptionsWrapper()
{
}

bool OptionsWrapper::loadMapOptions( const wxString& mapname)
{
	return loadOptions(MapOption,mapname);
}

OptionType OptionsWrapper::GetSingleOptionType ( const wxString& key) const
{
	OptionType type = opt_undefined;
	for ( int g = 0; g < LastOption; g++ )
	{
		if (keyExists(key,(GameOption)g,false,type))
			return type;
	}
	return opt_undefined;
}

bool OptionsWrapper::loadAIOptions( const wxString& modname, int aiindex,const wxString& ainame )
{
	int mapindex = m_ais_indexes[ainame];
	if ( mapindex == 0 ) mapindex = m_ais_indexes.size() + LastOption;
	m_ais_indexes[ainame] = mapindex;
	wxLogDebugFunc( _T("bot name: ") + ainame + _T(" option index: ") + TowxString( mapindex ) );
	unLoadOptions((GameOption)mapindex);
	try
	{
		GameOptions opt = usync().GetAIOptions( modname, aiindex );
		ParseSectionMap( m_sections[mapindex], opt.section_map );
		m_opts[mapindex] = opt;
	} catch (...)
	{
		return false;
	}
	return true;
}

int OptionsWrapper::GetAIOptionIndex( const wxString& nick ) const
{
	std::map<wxString,int>::const_iterator itor = m_ais_indexes.find(nick);
	int pos = -1;
	if ( itor != m_ais_indexes.end() ) pos = itor->second;
	wxLogDebugFunc( _T("bot name: ") + nick + _T(" option index: ") + TowxString( pos ) );
	return pos;
}

bool OptionsWrapper::loadOptions( GameOption modmapFlag, const wxString& name, const wxString& extra_filename )
{
	unLoadOptions(modmapFlag);
	GameOptions opt;
	switch (modmapFlag)
	{
	    default:
            break;
		case MapOption:
			try
			{
                opt = usync().GetMapOptions(name);
                ParseSectionMap( m_sections[modmapFlag], opt.section_map );
			}
			catch(...)
			{
				wxLogError(_T("Could not load map options"));
				return false;
			}
			break;

		case ModOption:
			try
			{
                opt = usync().GetModOptions(name);
                ParseSectionMap( m_sections[modmapFlag], opt.section_map );
			}
			catch(...)
			{
				wxLogError(_T("Could not load game options"));
				return false;
			}
			break;

        case EngineOption: {
            mmOptionList startpos( _("Start Position Type"),_T("startpostype"), _("How players will select where to be spawned in the map\n0: fixed map positions\n1: random map positions\n2: choose in game\n3: choose in the lobby before starting"), _T("0") );
            startpos.addItem( _T("0"), _("Fixed"), _T("Use the start positions defined in the map, the positions will be assigned incrementally from the team with lowest number to highest") );
            startpos.addItem( _T("1"), _("Random"), _T("Use the start positions defined in the map, the positions will be assigned randomly") );
            startpos.addItem( _T("2"), _("Choose in-game"), _T("Players will be able to pick their own starting point right before the game starts, optionally limited by a bounding box defined by the host") );
            startpos.addItem( _T("3"), _("Choose before game"), _T("The host will place each player's start position in the map preview before the game is launched") );
            opt.list_map[_T("startpostype")] = startpos;
            break;
        }

        case PrivateOptions: {
            opt.string_map[_T("restrictions")] = mmOptionString(_("List of restricted units"), _T("restrictedunits"), _T("Units in this list won't be available in game"), _T(""), 0 ); // tab separated list
            opt.string_map[_T("mapname")] = mmOptionString(_("Map name"), _T("mapname"), _T("Map name"), _T(""), 0 );
            break;
        }

        case ModCustomizations: {
            try {
                opt = usync().GetModCustomizations( name );
            }
            catch(...) {
				wxLogError(_T("Could not load mod customizations"));
				return false;
			}
			break;
        }

        case SkirmishOptions: {
            try {
                opt = usync().GetSkirmishOptions( name, extra_filename );
            }
            catch(...) {
				wxLogError(_T("Could not load skirmish options"));
				return false;
			}
			break;
        }
	}
	m_opts[modmapFlag] = opt;
	return true;
}

OptionsWrapper::GameOption OptionsWrapper::GetSection( const wxString& key ) const
{
	GameOption ret = LastOption;
	bool found = false;
	for ( int flag = 0; flag < PrivateOptions; flag++ )
	{
		OptionType optType = opt_undefined;
		found = keyExists( key, (GameOption)flag, false, optType );
		if ( found )
		{
			 ret = (GameOption)flag;
			 break;
		}
	}
	return ret;
}

bool OptionsWrapper::keyExists( const wxString& key ) const
{
	bool found = false;
	for ( int flag = 0; flag < PrivateOptions; flag++ )
	{
		OptionType optType = opt_undefined;
		found = keyExists( key, (GameOption)flag, false, optType );
		if ( found ) break;
	}
	return found;
}

bool OptionsWrapper::keyExists( const wxString& key, const GameOption modmapFlag, bool showError, OptionType& optType ) const
{
	wxString duplicateKeyError = _T("Please contact the game's author and tell him\n\
										to use unique keys in his ModOptions.lua");
	bool exists = false;
	optType = opt_undefined;
    GameOptionsMap::const_iterator optIt = m_opts.find((int)modmapFlag);
    if ( optIt == m_opts.end() )
        return false;
	const GameOptions& gameoptions = optIt->second;
	if (  gameoptions.list_map.find(key) !=  gameoptions.list_map.end())
	{
		optType = opt_list;
		exists = true;
	}
	else if ( gameoptions.string_map.find(key) !=  gameoptions.string_map.end())
	{
		optType = opt_string;
		exists = true;
	}
	else if ( gameoptions.bool_map.find(key) !=  gameoptions.bool_map.end())
	{
		optType = opt_bool;
		exists = true;
	}
	else if ( gameoptions.float_map.find(key)!=  gameoptions.float_map.end())
	{
		optType = opt_float;
		exists = true;
	}
	else if ( gameoptions.section_map.find(key)!=  gameoptions.section_map.end())
	{
		optType = opt_section;
		exists = true;
	}
	if (exists && showError)
	{
		customMessageBoxNoModal(SL_MAIN_ICON,duplicateKeyError,_T("Mod/map option error"),wxOK);
		wxLogWarning(_T("duplicate key in mapmodoptions"));
		return false;
	}
	else if ( exists && !showError )
	{
	  return true;
	}
	else
		return false;
}

bool  OptionsWrapper::setOptions(wxStringPairVec* options, GameOption modmapFlag)
{
	for (wxStringPairVec::const_iterator it = options->begin(); it != options->end(); ++it)
	{
		wxString key = it->first;
		wxString value = it->second;

		//we don't want to add a key that doesn't already exists
		OptionType optType = opt_undefined;
		if(!keyExists(key,modmapFlag,false,optType))
			return false;
		else
		{
			if ( !setSingleOptionTypeSwitch( key, value, modmapFlag, optType) )
				return false;
		}
	}
	return true;
}

OptionsWrapper::wxStringTripleVec OptionsWrapper::getOptions( GameOption modmapFlag) const
{
    wxStringTripleVec list;
    GameOptionsMapCIter optIt = m_opts.find((int)modmapFlag);
    if ( optIt != m_opts.end() ) {
        const GameOptions& gameoptions = optIt->second;
        for (SpringUnitSync::OptionMapBoolConstIter it = gameoptions.bool_map.begin(); it != gameoptions.bool_map.end(); ++it) {
            list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name , TowxString(it->second.value) ) ) );
        }

        for (SpringUnitSync::OptionMapStringConstIter it = gameoptions.string_map.begin(); it != gameoptions.string_map.end(); ++it) {
            list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, it->second.value) ) );
        }

        for (SpringUnitSync::OptionMapFloatConstIter it = gameoptions.float_map.begin(); it != gameoptions.float_map.end(); ++it) {
            list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, TowxString(it->second.value) ) ) );
        }

        for (SpringUnitSync::OptionMapListConstIter it = gameoptions.list_map.begin(); it != gameoptions.list_map.end(); ++it) {
            list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, it->second.value ) ) );
        }
    }
	return list;
}

std::map<wxString,wxString> OptionsWrapper::getOptionsMap( GameOption modmapFlag ) const
{
    std::map<wxString,wxString> map;
    GameOptionsMapCIter optIt = m_opts.find((int)modmapFlag);
    if ( optIt != m_opts.end() ) {
        const GameOptions& gameoptions = optIt->second;
        for (SpringUnitSync::OptionMapBoolConstIter it = gameoptions.bool_map.begin(); it != gameoptions.bool_map.end(); ++it) {
            map[it->first] =  TowxString(it->second.value);
        }

        for (SpringUnitSync::OptionMapStringConstIter it = gameoptions.string_map.begin(); it != gameoptions.string_map.end(); ++it) {
            map[it->first] = it->second.value;
        }

        for (SpringUnitSync::OptionMapFloatConstIter it = gameoptions.float_map.begin(); it != gameoptions.float_map.end(); ++it) {
            map[it->first] = TowxString(it->second.value);
        }

        for (SpringUnitSync::OptionMapListConstIter it = gameoptions.list_map.begin(); it != gameoptions.list_map.end(); ++it) {
            map[it->first] = it->second.value;
        }
    }
	return map;
}

bool OptionsWrapper::setSingleOption( const wxString& key, const wxString& value,GameOption modmapFlag)
{
	OptionType optType = opt_undefined;
	keyExists( key, modmapFlag, false, optType );
	return setSingleOptionTypeSwitch(key,value,modmapFlag,optType);
}

bool OptionsWrapper::setSingleOption( const wxString& key, const wxString& value )
{
	OptionType optType = opt_undefined;
	if (keyExists(key,ModOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,ModOption,optType);
	else if (keyExists(key,MapOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,MapOption,optType);
	else
		return false;
}

wxString OptionsWrapper::getSingleValue( const wxString& key) const
{
	for ( int g = 0; g < LastOption; g++ )
	{
		const wxString tmp = getSingleValue(key, (GameOption)g);
		if (tmp != wxEmptyString)
			return tmp;
	}
	return wxEmptyString;
}
template < class MapType >
static inline typename MapType::mapped_type GetItem( const MapType& map, const typename MapType::key_type& key )
{
    typename MapType::const_iterator mapIt = map.find(key);
    if ( mapIt != map.end() )
        return mapIt->second;
    else
        return typename MapType::mapped_type();
}

wxString OptionsWrapper::getSingleValue( const wxString& key, GameOption modmapFlag) const
{
	OptionType optType = opt_undefined;

	if ( keyExists(key,modmapFlag,false,optType) )
	{
        GameOptionsMapCIter optIt = m_opts.find((int)modmapFlag);
        if ( optIt == m_opts.end() )
            return wxEmptyString;

        const GameOptions& tempOpt = optIt->second;
		switch (optType)
		{
		case opt_float:
			return TowxString( GetItem( tempOpt.float_map, key ).value );
		case opt_bool:
			return TowxString( GetItem( tempOpt.bool_map, key ).value );
		case opt_string:
			return  GetItem( tempOpt.string_map, key ).value ;
		case opt_list:
			return GetItem( tempOpt.list_map, key ).value;
        case opt_undefined:
        default:
            return wxEmptyString;
		}
	}

	return wxEmptyString;
}

wxString OptionsWrapper::getDefaultValue( const wxString& key, GameOption modmapFlag) const
{
	OptionType optType = opt_undefined;
	wxString ret;
	if ( keyExists(key,modmapFlag,false,optType) )
	{
			//purposefully create a copy, no better idea
        GameOptionsMapCIter optIt = m_opts.find((int)modmapFlag);
        if ( optIt == m_opts.end() )
            return wxEmptyString;

        const GameOptions& tempOpt = optIt->second;
		switch ( optType )
		{
			{
			case opt_bool:
				ret = TowxString( GetItem( tempOpt.bool_map, key ).def );
				break;
			}
			case opt_float:
			{
				ret = TowxString( GetItem( tempOpt.float_map, key ).def );
				break;
			}
			case opt_string:
			{
				ret = GetItem( tempOpt.string_map, key ).def;
				break;
			}
			case opt_list:
			{
				ret = GetItem( tempOpt.list_map, key ).def;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	return ret;
}

bool  OptionsWrapper::setSingleOptionTypeSwitch( const wxString& key, const wxString& value, GameOption modmapFlag, OptionType optType)
{
	GameOptions& gameoptions = m_opts[modmapFlag];
	switch (optType)
	{
		case opt_float :
		{
			//test if min < val < max
			double d_val;
			//temp set to C locale cause we get '.' as decimal seperator over the net
			const char* old_locale = std::setlocale(LC_NUMERIC, "C");
			bool d_conv_ok = value.ToDouble(&d_val);
			std::setlocale(LC_NUMERIC, old_locale);
			if( !d_conv_ok || d_val < (gameoptions.float_map)[key].min || d_val > (gameoptions.float_map)[key].max )
			{
				wxLogWarning(_T("recieved number option exceeds boundaries"));
				return false;
			}
			else
				(gameoptions.float_map)[key].value = d_val;
			break;
		}
		case opt_bool :
		{
			long l_val;
			bool l_conv_ok = value.ToLong(&l_val);
			if( !l_conv_ok || ( l_val != 1 && l_val != 0 ) )
			{
				wxLogWarning(_T("recieved bool option that is neither 0 or 1"));
				return false;
			}
			else
				(gameoptions.bool_map)[key].value = bool(l_val);
			break;
		}
		case opt_string :
		{
			// test if maxlength isn't exceeded
			unsigned int max_lenght = (gameoptions.string_map)[key].max_len;
			if ( ( max_lenght != 0 ) && ( value.Len() > max_lenght )  )
			{
				wxLogWarning(_T("recieved string option exceeds max_len"));
				return false;
			}
			else
				(gameoptions.string_map)[key].value = value;
			break;
		}
		case opt_list :
		{
			// test if valid value, aka is in list
			int listitemcount = (gameoptions.list_map)[key].listitems.size();
			bool valid_string = false;
			int j = 0;
			for (; j < listitemcount; ++j)
			{
				if ( (gameoptions.list_map)[key].listitems[j].key == value)
				{
					valid_string = true;
					break;
				}
			}

			if (valid_string)
			{
			    //LOOKATME (koshi) if there's a problem with list modoption look here first
				(gameoptions.list_map)[key].value = (gameoptions.list_map)[key].listitems[j].key;
				(gameoptions.list_map)[key].cur_choice_index = j;
			}
			else
			{
				wxLogWarning(_T("recieved list option is not valid"));
				return false;
			}
			break;
		}
		default:
			return false;
	}
	//if we made it here, all is good
	return true;
}

wxString OptionsWrapper::GetNameListOptValue( const wxString& key, GameOption flag) const
{
	OptionType optType;
	if ( keyExists(key,flag,false, optType) )
	{
		if ( optType == opt_list)
		{
            GameOptionsMapCIter optIt = m_opts.find((int)flag);
            if ( optIt == m_opts.end() )
                return wxEmptyString;

            GameOptions tempOpt = optIt->second;
			return ( (tempOpt.list_map)[key].cbx_choices[ (tempOpt.list_map)[key].cur_choice_index ] );
		}
	}

	// at this point retrieval failed
	return wxEmptyString;
}

wxString OptionsWrapper::GetNameListOptItemKey( const wxString& optkey, const wxString& itemname, GameOption flag) const
{
	OptionType optType;
	if ( keyExists(optkey,flag,false, optType) )
	{
		if ( optType == opt_list)
		{
            GameOptionsMapCIter optIt = m_opts.find((int)flag);
            if ( optIt == m_opts.end() )
                return wxEmptyString;

            GameOptions tempOpt = optIt->second;
			for (ListItemVec::const_iterator it = (tempOpt.list_map)[optkey].listitems.begin(); it != (tempOpt.list_map)[optkey].listitems.end(); ++it)
			{
				if (it->name == itemname)
					return it->key;
			}
		}
	}

	// at this point retrieval failed
	return wxEmptyString;
}

bool OptionsWrapper::MergeOptions( const OptionsWrapper& other, GameOption merge_into )
{
    GameOptionsMapCIter other_it = other.m_opts.begin();
    for ( ; other_it != other.m_opts.end(); ++other_it ) {
        const GameOptions& other_opts = other_it->second;
        //const GameOption other_id = (const GameOption)other_it->first; //TODO (koshi) what was this supposed to be used for?

        for (SpringUnitSync::OptionMapBoolConstIter it = other_opts.bool_map.begin(); it != other_opts.bool_map.end();++it ) {
            m_opts[merge_into].bool_map[it->first] = it->second;
        }

        for ( SpringUnitSync::OptionMapFloatConstIter it = other_opts.float_map.begin(); it != other_opts.float_map.end(); ++it ) {
            m_opts[merge_into].float_map[it->first] = it->second;
        }

        for ( SpringUnitSync::OptionMapListConstIter it = other_opts.list_map.begin(); it != other_opts.list_map.end(); ++it ){
            m_opts[merge_into].list_map[it->first] = it->second;
        }

        for ( SpringUnitSync::OptionMapStringConstIter it = other_opts.string_map.begin(); it != other_opts.string_map.end(); ++it ) {
            m_opts[merge_into].string_map[it->first] = it->second;
        }
    }
    return true;
}

void OptionsWrapper::ParseSectionMap( mmSectionTree& section_tree, const SpringUnitSync::OptionMapSection& section_map )
{

    // map child-key <-> parent-key
    typedef std::map<wxString,wxString> RelationMap;
	typedef std::map<wxString,wxString>::iterator RelationMapIter;
    RelationMap relation_map;

    //setup relation map
    for ( SpringUnitSync::OptionMapSectionConstIter it = section_map.begin(); it != section_map.end(); ++it )
    {
        relation_map[it->second.key] = it->second.section;
    }

    RelationMapIter rit = relation_map.begin();
    // no more items in the map means we've added them all
    while ( relation_map.size() > 0 )
    {
        RelationMapIter rit_next = rit; // in case we need to delete
        ++rit_next;

        if ( relation_map.find(rit->second) == relation_map.end() )
        {
            //either we already added this sections parent or it's a root section
            SpringUnitSync::OptionMapSectionConstIter section = section_map.find(rit->first);
            assert ( section != section_map.end() );
                section_tree.AddSection( section->second );


              //we're done with this section, so remove it
            relation_map.erase(rit);
        }

        rit = rit_next;

        //we've reached the end of the map, restart at beginning
        if ( rit == relation_map.end() )
            rit = relation_map.begin();
    }

}

mmSectionTree::mmSectionTree()
	: m_tree ( new wxFileConfig( _T("SL-temp"), wxEmptyString, wxFileName::CreateTempFileName( _T("springlobby-tree_") ) ) )
{}

mmSectionTree::~mmSectionTree()
{
    #ifndef NDEBUG
        m_tree->Flush();
    #else //no need to clutter tempfile directory if we're not debugging
        m_tree->DeleteAll();
    #endif
}

void mmSectionTree::AddSection ( const wxString& parentpath, const mmOptionSection& section )
{
    wxString fullpath = parentpath + _T("/") + section.key + _T("/");
    m_tree->Write( fullpath + _T("key") ,section.key );
    #ifndef NDEBUG
        m_tree->Flush();
    #endif
}

bool mmSectionTree::FindRecursive( const wxString& parent_key, wxString& path )
{
    wxString current;
    long cur_index;

    //search current level first before recursing
    bool cont = m_tree->GetFirstGroup( current, cur_index );
    while ( cont )
    {
        if ( current.EndsWith( parent_key ) ) {
            path = current;
            return true;
        }
        cont = m_tree->GetNextGroup( current, cur_index );
    }

    //we need to recurse into sub-paths
    cont = m_tree->GetFirstGroup( current, cur_index );
    while ( cont )
    {
        wxString old_path = m_tree->GetPath();
        m_tree->SetPath( old_path + _T("/") + current );
        if ( FindRecursive( parent_key,  path ) )
            return true;
        m_tree->SetPath( old_path );
        cont = m_tree->GetNextGroup( current, cur_index );
    }
    return false;
}

wxString mmSectionTree::FindParentpath ( const wxString& parent_key )
{
    wxString path = _T("/");
    if ( FindRecursive( parent_key, path ) )
        return path;
    else
        return _T("");
}

void mmSectionTree::AddSection( const mmOptionSection& section)
{
    m_section_map[section.key] = section;
    wxString name = section.section;
    if ( section.section == SLGlobals::nosection_name )
    {
        AddSection( _T("/"), section );
    }
    else
    {
        wxString parent = FindParentpath( section.section );
        AddSection( parent , section );
    }
}

//mmSectionTree::SectionVector mmSectionTree::GetSectionVector()
//{
//
//}

void mmSectionTree::Clear()
{
    m_section_map.clear();
    m_tree->DeleteAll();
}


