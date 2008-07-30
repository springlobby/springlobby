#include "mmoptionswrapper.h"

#include "iunitsync.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"

#include <stdexcept>
#include <wx/intl.h>

mmOptionsWrapper::mmOptionsWrapper()
{
	unLoadOptions();
	loadOptions( EngineOption );
}

void mmOptionsWrapper::unLoadOptions()
{
	for (int i = 0; i < optionCategoriesCount; ++i)
	{
		unLoadOptions( (GameOption)i );
	}
}

void mmOptionsWrapper::unLoadOptions(GameOption i)
{
  GameOptions empty;
  opts[i] = empty;
}

mmOptionsWrapper::~mmOptionsWrapper()
{
}

bool mmOptionsWrapper::loadMapOptions(wxString mapname)
{
	return loadOptions(MapOption,mapname);
}

OptionType mmOptionsWrapper::GetSingleOptionType (wxString key) const
{
	OptionType type = opt_undefined;
	for ( int g = 0; g < optionCategoriesCount; g++ )
	{
		if (keyExists(key,(GameOption)g,false,type))
			return type;
	}
	return opt_undefined;
}


bool mmOptionsWrapper::loadOptions(GameOption modmapFlag, wxString name)
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
        opt = usync()->GetMapOptions(name);
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
        opt = usync()->GetModOptions(name);
			}
			catch(...)
			{
				wxLogError(_T("Could not load mod Options"));
				return false;
			}
			break;
    case EngineOption:
        opt.bool_map[_T("limitdgun")] = mmOptionBool(_("Limit D-Gun"),_T("limitdgun"),
        _("Disables commander's D-gun when being too far away from the starting point"),false);
        opt.bool_map[_T("ghostedbuildings")] = mmOptionBool(_("Ghosted Buildings"),_T("ghostedbuildings"),
        _("Enemy buildings will leave a ghost image on the map after losing LoS on them"),true);
        opt.bool_map[_T("diminishingmms")] = mmOptionBool(_("Diminishing MM"),_T("diminishingmms"),
        _("Efficiency of MetalMakers will progressively reduce as much as you build them"),false);
        opt.int_map[_T("startpostype")] = mmOptionInt( _("Start Position Type"),_T("startpostype"),
        _("How players will select where to be spawned in the map\n0: fixed map positions\n1: random map positions\n2: chose in game\n3: chose in the lobby before starting"),
        0, 1, 0, 3);
        opt.int_map[_T("gamemode")] = mmOptionInt( _("Game Ending condition"),_T("gamemode"),
        _("The condition that will end the game\n0: when all units will be destroyed\n1: when the commander will be destroyed\n2: lineage mode (see option 1, but given away units will still die"),
        0, 1, 0, 2);
        opt.int_map[_T("startmetal")] = mmOptionInt( _("Start Metal"),_T("startmetal"),
        _("Sets the amount of metal that players will start with"),
        1000, 1, 0, 10000);
        opt.int_map[_T("startenergy")] = mmOptionInt( _("Start Energy"),_T("startenergy"),
        _("Sets the amount of energy that players will start with"),
        1000, 1, 0, 10000);
        opt.int_map[_T("maxunits")] = mmOptionInt( _("Max Units Allowed"),_T("maxunits"),
        _("Sets the maximum amount of units that a player will be allowed to build"),
        500, 1, 0, 10000);
	}
	opts[modmapFlag] = opt;
	return true;
}

bool mmOptionsWrapper::keyExists(wxString key, GameOption modmapFlag, bool showError, OptionType& optType)
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

bool  mmOptionsWrapper::setOptions(wxStringPairVec* options, GameOption modmapFlag)
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

void  mmOptionsWrapper::getOptions(wxStringTripleVec* list, GameOption modmapFlag) const
{
	for (optionMapBoolConstIter it = opts[modmapFlag].bool_map.begin(); it != opts[modmapFlag].bool_map.end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name , i2s(it->second.value) ) ) );
	}

	for (optionMapStringConstIter it = opts[modmapFlag].string_map.begin(); it != opts[modmapFlag].string_map.end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, it->second.value) ) );
	}

	for (optionMapFloatConstIter it = opts[modmapFlag].float_map.begin(); it != opts[modmapFlag].float_map.end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, f2s(it->second.value) ) ) );
	}

	for (optionMapListConstIter it = opts[modmapFlag].list_map.begin(); it != opts[modmapFlag].list_map.end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, it->second.value ) ) );
	}

	for (optionMapIntConstIter it = opts[modmapFlag].int_map.begin(); it != opts[modmapFlag].int_map.end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, i2s(it->second.value) ) ) );
	}
}

void mmOptionsWrapper::getOptionsMap(wxStringMap* map, GameOption modmapFlag) const
{
	for (optionMapBoolConstIter it = opts[modmapFlag].bool_map.begin(); it != opts[modmapFlag].bool_map.end(); ++it)
	{
		(*map)[it->first] =  i2s(it->second.value);
	}

	for (optionMapStringConstIter it = opts[modmapFlag].string_map.begin(); it != opts[modmapFlag].string_map.end(); ++it)
	{
		(*map)[it->first] = it->second.value;
	}

	for (optionMapFloatConstIter it = opts[modmapFlag].float_map.begin(); it != opts[modmapFlag].float_map.end(); ++it)
	{
		(*map)[it->first] = f2s(it->second.value);
	}

	for (optionMapListConstIter it = opts[modmapFlag].list_map.begin(); it != opts[modmapFlag].list_map.end(); ++it)
	{
		(*map)[it->first] = it->second.value;
	}

	for (optionMapIntConstIter it = opts[modmapFlag].int_map.begin(); it != opts[modmapFlag].int_map.end(); ++it)
	{
		(*map)[it->first] =  i2s(it->second.value);
	}
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value,GameOption modmapFlag)
{
	OptionType optType = opt_undefined;
	keyExists( key, modmapFlag, false, optType );
	return setSingleOptionTypeSwitch(key,value,modmapFlag,optType);
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value)
{
	OptionType optType = opt_undefined;
	if (keyExists(key,ModOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,ModOption,optType);
	else if (keyExists(key,MapOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,MapOption,optType);
	else
		return false;
}

wxString mmOptionsWrapper::getSingleValue(wxString key) const
{
	for ( int g = 0; g < optionCategoriesCount; g++ )
	{
		const wxString tmp = getSingleValue(key, (GameOption)g);
		if (tmp != wxEmptyString)
			return tmp;
	}
	return wxEmptyString;
}

wxString mmOptionsWrapper::getSingleValue(wxString key, GameOption modmapFlag) const
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
            return wxEmptyString;
		}
	}

	return wxEmptyString;
}

bool  mmOptionsWrapper::setSingleOptionTypeSwitch(wxString key, wxString value, GameOption modmapFlag, OptionType optType)
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
			if ( int(value.Len())> (opts[modmapFlag].string_map)[key].max_len )
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

bool mmOptionsWrapper::reloadMapOptions(wxString mapname)
{
  GameOptions empty;
	opts[MapOption] = empty;

	return loadMapOptions(mapname);
}

wxString mmOptionsWrapper::GetNameListOptValue(wxString key, GameOption flag) const
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

wxString mmOptionsWrapper::GetNameListOptItemKey(wxString optkey, wxString itemname, GameOption flag) const
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
