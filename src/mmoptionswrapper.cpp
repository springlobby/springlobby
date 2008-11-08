#include "mmoptionswrapper.h"

#include "iunitsync.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"

#include <stdexcept>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/fileconf.h>

OptionsWrapper::OptionsWrapper()
{
	unLoadOptions();
	loadOptions( EngineOption );
	loadOptions( PrivateOptions );
}

void OptionsWrapper::unLoadOptions()
{
	for (int i = 0; i < optionCategoriesCount; ++i)
	{
		unLoadOptions( (GameOption)i );
	}
}

void OptionsWrapper::unLoadOptions(GameOption i)
{
  GameOptions empty;
  opts[i] = empty;
}

OptionsWrapper::~OptionsWrapper()
{
}

bool OptionsWrapper::loadMapOptions(wxString mapname)
{
	return loadOptions(MapOption,mapname);
}

OptionType OptionsWrapper::GetSingleOptionType (wxString key) const
{
	OptionType type = opt_undefined;
	for ( int g = 0; g < optionCategoriesCount; g++ )
	{
		if (keyExists(key,(GameOption)g,false,type))
			return type;
	}
	return opt_undefined;
}


bool OptionsWrapper::loadOptions(GameOption modmapFlag, wxString name)
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
                ParseSectionMap( m_map_sections, opt.section_map );
			}
			catch(...)
			{
				wxLogError(_T("Could not load map Options"));
				return false;
			}
			break;
		case ModOption:
			try
			{
                opt = usync().GetModOptions(name);
                ParseSectionMap( m_mod_sections, opt.section_map );
			}
			catch(...)
			{
				wxLogError(_T("Could not load mod Options"));
				return false;
			}
			break;
    case EngineOption:
    {
        opt.int_map[_T("startpostype")] = mmOptionInt( _("Start Position Type"),_T("startpostype"),
        _("How players will select where to be spawned in the map\n0: fixed map positions\n1: random map positions\n2: chose in game\n3: chose in the lobby before starting"),
        0, 1, 0, 3);
        break;
    }
    case PrivateOptions:
    {
        opt.string_map[_T("restrictions")] = mmOptionString(_("List of restricted units"), /// tab separated list
        _T("restrictedunits"), _T("Units in this list won't be available in game"), _T(""), 0 );
        opt.string_map[_T("mapname")] = mmOptionString(_("Map name"), _T("mapname"), _T("Map name"), _T(""), 0 );
        break;
    }
	}
	opts[modmapFlag] = opt;
	return true;
}

bool OptionsWrapper::keyExists(wxString key, GameOption modmapFlag, bool showError, OptionType& optType) const
{
	wxString duplicateKeyError = _T("Please contact the mod's author and tell him\n"
										"to use unique keys in his ModOptions.lua");
	bool exists = false;
	optType = opt_undefined;
	if ( modmapFlag < ModOption || modmapFlag > LastOption -1 )
		return false;//, wxString::Format(_T("%d"),modmapFlag));
	if (  opts[modmapFlag].list_map.find(key) !=  opts[modmapFlag].list_map.end())
	{
		optType = opt_list;
		exists = true;
	}
	else if ( opts[modmapFlag].string_map.find(key) !=  opts[modmapFlag].string_map.end())
	{
		optType = opt_string;
		exists = true;
	}
	else if ( opts[modmapFlag].bool_map.find(key) !=  opts[modmapFlag].bool_map.end())
	{
		optType = opt_bool;
		exists = true;
	}
	else if ( opts[modmapFlag].float_map.find(key)!=  opts[modmapFlag].float_map.end())
	{
		optType = opt_float;
		exists = true;
	}
	else if ( opts[modmapFlag].int_map.find(key)!=  opts[modmapFlag].int_map.end())
	{
		optType = opt_int;
		exists = true;
	}
	else if ( opts[modmapFlag].section_map.find(key)!=  opts[modmapFlag].section_map.end())
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
	for (wxStringPairVec::iterator it = options->begin(); it != options->end(); ++it)
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
	for (IUnitSync::OptionMapBoolConstIter it = opts[modmapFlag].bool_map.begin(); it != opts[modmapFlag].bool_map.end(); ++it)
	{
		list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name , i2s(it->second.value) ) ) );
	}

	for (IUnitSync::OptionMapStringConstIter it = opts[modmapFlag].string_map.begin(); it != opts[modmapFlag].string_map.end(); ++it)
	{
		list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, it->second.value) ) );
	}

	for (IUnitSync::OptionMapFloatConstIter it = opts[modmapFlag].float_map.begin(); it != opts[modmapFlag].float_map.end(); ++it)
	{
		list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, f2s(it->second.value) ) ) );
	}

	for (IUnitSync::OptionMapListConstIter it = opts[modmapFlag].list_map.begin(); it != opts[modmapFlag].list_map.end(); ++it)
	{
		list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, it->second.value ) ) );
	}

	for (IUnitSync::OptionMapIntConstIter it = opts[modmapFlag].int_map.begin(); it != opts[modmapFlag].int_map.end(); ++it)
	{
		list.push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, i2s(it->second.value) ) ) );
	}
	return list;
}

std::map<wxString,wxString> OptionsWrapper::getOptionsMap( GameOption modmapFlag ) const
{
  std::map<wxString,wxString> map;
	for (IUnitSync::OptionMapBoolConstIter it = opts[modmapFlag].bool_map.begin(); it != opts[modmapFlag].bool_map.end(); ++it)
	{
		map[it->first] =  i2s(it->second.value);
	}

	for (IUnitSync::OptionMapStringConstIter it = opts[modmapFlag].string_map.begin(); it != opts[modmapFlag].string_map.end(); ++it)
	{
		map[it->first] = it->second.value;
	}

	for (IUnitSync::OptionMapFloatConstIter it = opts[modmapFlag].float_map.begin(); it != opts[modmapFlag].float_map.end(); ++it)
	{
		map[it->first] = f2s(it->second.value);
	}

	for (IUnitSync::OptionMapListConstIter it = opts[modmapFlag].list_map.begin(); it != opts[modmapFlag].list_map.end(); ++it)
	{
		map[it->first] = it->second.value;
	}

	for (IUnitSync::OptionMapIntConstIter it = opts[modmapFlag].int_map.begin(); it != opts[modmapFlag].int_map.end(); ++it)
	{
		map[it->first] =  i2s(it->second.value);
	}
	return map;
}

bool OptionsWrapper::setSingleOption(wxString key,wxString value,GameOption modmapFlag)
{
	OptionType optType = opt_undefined;
	keyExists( key, modmapFlag, false, optType );
	return setSingleOptionTypeSwitch(key,value,modmapFlag,optType);
}

bool OptionsWrapper::setSingleOption(wxString key,wxString value)
{
	OptionType optType = opt_undefined;
	if (keyExists(key,ModOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,ModOption,optType);
	else if (keyExists(key,MapOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,MapOption,optType);
	else
		return false;
}

wxString OptionsWrapper::getSingleValue(wxString key) const
{
	for ( int g = 0; g < optionCategoriesCount; g++ )
	{
		const wxString tmp = getSingleValue(key, (GameOption)g);
		if (tmp != wxEmptyString)
			return tmp;
	}
	return wxEmptyString;
}

wxString OptionsWrapper::getSingleValue(wxString key, GameOption modmapFlag) const
{
	OptionType optType = opt_undefined;

	if ( keyExists(key,modmapFlag,false,optType) )
	{
	    //purposefully create a copy, no better idea
	    GameOptions tempOpt = opts[modmapFlag];
		switch (optType)
		{
		case opt_float:
			return f2s( tempOpt.float_map[key].value );
		case opt_bool:
			return i2s( tempOpt.bool_map[key].value );
		case opt_int:
			return i2s( tempOpt.int_map[key].value );
		case opt_string:
			return  tempOpt.string_map[key].value ;
		case opt_list:
			return tempOpt.list_map[key].value;
        case opt_undefined:
        default:
            return wxEmptyString;
		}
	}

	return wxEmptyString;
}

bool  OptionsWrapper::setSingleOptionTypeSwitch(wxString key, wxString value, GameOption modmapFlag, OptionType optType)
{
	switch (optType)
	{
		case opt_float :
		{
			//test if min < val < max
			double d_val;
			bool d_conv_ok = value.ToDouble(&d_val);
			if( !d_conv_ok || d_val < (opts[modmapFlag].float_map)[key].min || d_val > (opts[modmapFlag].float_map)[key].max )
			{
				wxLogWarning(_T("recieved number option exceeds boundaries"));
				return false;
			}
			else
				(opts[modmapFlag].float_map)[key].value = d_val;
			break;
		}
		case opt_int :
		{
			//test if min < val < max
			long l_val = 0;
			bool l_conv_ok = value.ToLong(&l_val);
			if( !l_conv_ok || l_val < (opts[modmapFlag].int_map)[key].min || l_val > (opts[modmapFlag].int_map)[key].max )
			{
				wxLogWarning(_T("recieved number option exceeds boundaries"));
				return false;
			}
			else
				(opts[modmapFlag].int_map)[key].value = l_val;
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
				(opts[modmapFlag].bool_map)[key].value = bool(l_val);
			break;
		}
		case opt_string :
		{
			// test if maxlength isn't exceeded
			unsigned int max_lenght = (opts[modmapFlag].string_map)[key].max_len;
			if ( ( max_lenght != 0 ) && ( value.Len() > max_lenght )  )
			{
				wxLogWarning(_T("recieved string option exceeds max_len"));
				return false;
			}
			else
				(opts[modmapFlag].string_map)[key].value = value;
			break;
		}
		case opt_list :
		{
			// test if valid value, aka is in list
			int listitemcount = (opts[modmapFlag].list_map)[key].listitems.size();
			bool valid_string = false;
			int j = 0;
			for (; j < listitemcount; ++j)
			{
				if ( (opts[modmapFlag].list_map)[key].listitems[j].key == value)
				{
					valid_string = true;
					break;
				}
			}

			if (valid_string)
			{
			    //LOOKATME (koshi) if there's a problem with list modoption look here first
				(opts[modmapFlag].list_map)[key].value = (opts[modmapFlag].list_map)[key].listitems[j].key;
				(opts[modmapFlag].list_map)[key].cur_choice_index = j;
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

bool OptionsWrapper::reloadMapOptions(wxString mapname)
{
  GameOptions empty;
	opts[MapOption] = empty;

	return loadMapOptions(mapname);
}

wxString OptionsWrapper::GetNameListOptValue(wxString key, GameOption flag) const
{
	OptionType optType;
	if (flag < ModOption || flag > LastOption - 1)
		return wxEmptyString;
	else if ( keyExists(key,flag,false, optType) )
	{
		if ( optType == opt_list)
		{
		    GameOptions tempOpt = opts[flag];
			return ( (tempOpt.list_map)[key].cbx_choices[ (tempOpt.list_map)[key].cur_choice_index ] );
		}
	}

	// at this point retrieval failed
	return wxEmptyString;
}

wxString OptionsWrapper::GetNameListOptItemKey(wxString optkey, wxString itemname, GameOption flag) const
{
	OptionType optType;
	if (flag < ModOption || flag > LastOption - 1)
		return wxEmptyString;
	else if ( keyExists(optkey,flag,false, optType) )
	{
		if ( optType == opt_list)
		{
		    GameOptions tempOpt = opts[flag];
			for (ListItemVec::iterator it = (tempOpt.list_map)[optkey].listitems.begin(); it != (tempOpt.list_map)[optkey].listitems.end(); ++it)
			{
				if (it->name == itemname)
					return it->key;
			}
		}
	}

	// at this point retrieval failed
	return wxEmptyString;
}

void OptionsWrapper::ParseSectionMap( mmSectionTree& section_tree, const IUnitSync::OptionMapSection& section_map )
{
    for ( IUnitSync::OptionMapSectionConstIter it = section_map.begin(); it != section_map.end(); ++it )
    {
        section_tree.AddSection( it->second );
    }
}

mmSectionTree::mmSectionTree()
    : m_tree ( 0 )
{

    m_tree = new ConfigType(  _T("SL-temp"), wxEmptyString, _T("/tmp/sl_tree") );
}

mmSectionTree::~mmSectionTree()
{
    //! \todo wth does this segfault?
//    if ( m_tree ) {
//        delete m_tree;
//        m_tree = 0;
//    }
    #ifndef NDEBUG
        m_tree->Flush();
    #endif
}

void mmSectionTree::AddSection ( const wxString& parentpath, const mmOptionSection& section )
{
    wxString fullpath = parentpath + _T("/") + section.key + _T("/");
    m_tree->Write( fullpath + _T("key") ,section.key );
}

wxString mmSectionTree::FindParentpath ( const wxString& parent_key )
{
    long dummy;
    wxString current;
    bool cont = m_tree->GetFirstGroup( current, dummy );
    while ( cont )
    {
        //! \todo if possibly remove the .Lower()
        if ( current.EndsWith( parent_key.Lower() ) )
            return current;
        cont = m_tree->GetNextGroup( current, dummy );
    }
    return _T("");
}

void mmSectionTree::AddSection( const mmOptionSection section)
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

mmSectionTree::SectionVector mmSectionTree::GetSectionVector()
{

}

void mmSectionTree::Clear()
{
    m_section_map.clear();
    //m_tree->DeleteAll();
}
