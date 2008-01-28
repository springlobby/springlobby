#include "mmoptionswrapper.h"

#include "springunitsynclib.h"
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
	for (GameOption i = 0; i < optionCategoriesCount; ++i)
	{
		unLoadOptions( i );
	}
}

void mmOptionsWrapper::unLoadOptions(GameOption i)
{
	m_boolMaps[i] 	= new optionMapBool;
	m_floatMaps[i]	= new optionMapFloat;
	m_stringMaps[i] = new optionMapString;
	m_listMaps[i]	= new optionMapList;
	m_intMaps[i]	= new optionMapInt;

}

mmOptionsWrapper::~mmOptionsWrapper()
{
}

bool mmOptionsWrapper::loadMapOptions(wxString mapname)
{
	return loadOptions(MapOption,mapname);
}

OptionType mmOptionsWrapper::GetSingleOptionType (wxString key)
{
	OptionType* type = new OptionType(opt_undefined);
	for ( GameOption g = 0; g < optionCategoriesCount; g++ )
	{
		if (keyExists(key,g,false,type))
			return *type;
	}
	return opt_undefined;
}


bool mmOptionsWrapper::loadOptions(GameOption modmapFlag,wxString mapname)
{
	int count = 0;
	wxString singleError;
	unLoadOptions(modmapFlag);
	switch (modmapFlag)
	{
		case MapOption:
			try
			{
				count = susynclib()->GetMapOptionCount(mapname);

			}
			catch(...)
			{
				wxLogError(_T("Could not load map Options"));
				return false;
			}
			singleError = _T("Cannot load map-option #");
			break;
		case ModOption:
			try
			{
				count = susynclib()->GetModOptionCount();
			}
			catch(...)
			{
				wxLogError(_T("Could not load mod Options"));
				return false;
			}
			singleError = _T("Cannot load mod-option #");
			break;
    case EngineOption:
        (*m_boolMaps[modmapFlag])[_T("limitdgun")] = mmOptionBool(_("Limit D-Gun"),_T("limitdgun"),
        _("Disables commander's D-gun when being too far away from the starting point"),false);
        (*m_boolMaps[modmapFlag])[_T("ghostedbuildings")] = mmOptionBool(_("Ghosted Buildings"),_T("ghostedbuildings"),
        _("Enemy buildings will leave a ghost image on the map after losing LoS on them"),true);
        (*m_boolMaps[modmapFlag])[_T("diminishingmms")] = mmOptionBool(_("Diminishing MM"),_T("diminishingmms"),
        _("Efficiency of MetalMakers will progressively reduce as much as you build them"),false);
        (*m_intMaps[modmapFlag])[_T("startpostype")] = mmOptionInt( _("Start Position Type"),_T("startpostype"),
        _("How players will select where to be spawned in the map\n0: fixed map positions\n1: random map positions\n2: chose in game\n3: chose in the lobby before starting"),
        0, 1, 0, 3);
        (*m_intMaps[modmapFlag])[_T("gamemode")] = mmOptionInt( _("Game Ending condition"),_T("gamemode"),
        _("The condition that will end the game\n0: when all units will be destroyed\n1: when the commander will be destroyed\n2: lineage mode (see option 1, but given away units will still die"),
        0, 1, 0, 2);
        (*m_intMaps[modmapFlag])[_T("startmetal")] = mmOptionInt( _("Start Metal"),_T("startmetal"),
        _("Sets the amount of metal that players will start with"),
        1000, 1, 0, 10000);
        (*m_intMaps[modmapFlag])[_T("startenergy")] = mmOptionInt( _("Start Energy"),_T("startenergy"),
        _("Sets the amount of energy that players will start with"),
        1000, 1, 0, 10000);
        (*m_intMaps[modmapFlag])[_T("maxunits")] = mmOptionInt( _("Max Units Allowed"),_T("maxunits"),
        _("Sets the maximum amount of units that a player will be allowed to build"),
        500, 1, 0, 10000);
        return true;
	}

	mmOptionList* templist;
	for (int i = 0; i < count; ++i)
	{
		wxString key = susynclib()->GetOptionKey(i);
		OptionType* dummy = new OptionType(opt_undefined);
		if (keyExists(key,modmapFlag,true,dummy))
		{
			return false;
		}
		else
		{
			try
			{
				switch (susynclib()->GetOptionType(i))
				{
				case opt_float:
					(*m_floatMaps[modmapFlag])[key] = mmOptionFloat(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionNumberDef(i), susynclib()->GetOptionNumberStep(i),
							susynclib()->GetOptionNumberMin(i),susynclib()->GetOptionNumberMax(i));
					break;
				case opt_bool:
					(*m_boolMaps[modmapFlag])[key] = mmOptionBool(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionBoolDef(i));
					break;
				case opt_string:
					(*m_stringMaps[modmapFlag])[key] = mmOptionString(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionStringDef(i),susynclib()->GetOptionStringMaxLen(i));
					break;
				case opt_list:
					 templist = new mmOptionList(susynclib()->GetOptionName(i),key,
							susynclib()->GetOptionDesc(i),susynclib()->GetOptionListDef(i));
					 int listitemcount = susynclib()->GetOptionListCount(i);
					 for (int j = 0; j < listitemcount; ++j)
					 {
						 templist->addItem(susynclib()->GetOptionListItemKey(i,j),susynclib()->GetOptionListItemName(i,j),
								 								susynclib()->GetOptionListItemDesc(i,j));
					 }
					 (*m_listMaps[modmapFlag])[key] = (*templist);
				}

			}
			catch(...)
			{
				wxLogWarning( singleError << i );
				return false;
			}
		}
	}
	return true;
}

bool mmOptionsWrapper::keyExists(wxString key, GameOption modmapFlag, bool showError, OptionType* optType)
{
	wxString duplicateKeyError = _T("Please contact the mod's author and tell him\n"
										"to use unique keys in his ModOptions.lua");
	bool exists = false;
	*optType = opt_undefined;
	if ( modmapFlag < ModOption || modmapFlag > LastOption -1 )
		return false;//, wxString::Format(_T("%d"),modmapFlag));
	if ( m_listMaps[modmapFlag]->find(key) !=  m_listMaps[modmapFlag]->end())
	{
		*optType = opt_list;
		exists = true;
	}
	else if ( m_stringMaps[modmapFlag]->find(key) !=  m_stringMaps[modmapFlag]->end())
	{
		*optType = opt_string;
		exists = true;
	}
	else if ( m_boolMaps[modmapFlag]->find(key) !=  m_boolMaps[modmapFlag]->end())
	{
		*optType = opt_bool;
		exists = true;
	}
	else if ( m_floatMaps[modmapFlag]->find(key)!=  m_floatMaps[modmapFlag]->end())
	{
		*optType = opt_float;
		exists = true;
	}
	else if ( m_intMaps[modmapFlag]->find(key)!=  m_intMaps[modmapFlag]->end())
	{
		*optType = opt_int;
		exists = true;
	}
	if (exists && showError)
	{
		customMessageBox(SL_MAIN_ICON,duplicateKeyError,_T("Mod/map option error"),wxOK);
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
		OptionType* optType = new OptionType(opt_undefined);
		if(!keyExists(key,modmapFlag,false,optType))
			return false;
		else
		{
			if ( !setSingleOptionTypeSwitch( key, value, modmapFlag,  *optType) )
				return false;
		}
	}
	return true;
}

void  mmOptionsWrapper::getOptions(wxStringTripleVec* list, GameOption modmapFlag)
{
	for (optionMapBoolIter it = m_boolMaps[modmapFlag]->begin(); it != m_boolMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name , wxString::Format(_T("%d"),(*it).second.value) ) ) );
	}

	for (optionMapStringIter it = m_stringMaps[modmapFlag]->begin(); it != m_stringMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, (*it).second.value) ) );
	}

	for (optionMapFloatIter it = m_floatMaps[modmapFlag]->begin(); it != m_floatMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, wxString::Format(_T("%f"),(*it).second.value) ) ) );
	}

	for (optionMapListIter it = m_listMaps[modmapFlag]->begin(); it != m_listMaps[modmapFlag]->end(); ++it)
	{																					//TODO fixme
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, (*it).second.value) ) );
	}

	for (optionMapIntIter it = m_intMaps[modmapFlag]->begin(); it != m_intMaps[modmapFlag]->end(); ++it)
	{
		list->push_back( wxStringTriple( (*it).first, wxStringPair ( it->second.name, wxString::Format(_T("%ld"),(*it).second.value) ) ) );
	}
}

void mmOptionsWrapper::getOptionsMap(wxStringMap* map, GameOption modmapFlag)
{
	for (optionMapBoolIter it = m_boolMaps[modmapFlag]->begin(); it != m_boolMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] =  wxString::Format(_T("%d"),(*it).second.value);
	}

	for (optionMapStringIter it = m_stringMaps[modmapFlag]->begin(); it != m_stringMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] = (*it).second.value;
	}

	for (optionMapFloatIter it = m_floatMaps[modmapFlag]->begin(); it != m_floatMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] = wxString::Format(_T("%f"),(*it).second.value);
	}

	for (optionMapListIter it = m_listMaps[modmapFlag]->begin(); it != m_listMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] = (*it).second.value;
	}

	for (optionMapIntIter it = m_intMaps[modmapFlag]->begin(); it != m_intMaps[modmapFlag]->end(); ++it)
	{
		(*map)[(*it).first] =  wxString::Format(_T("%ld"),(*it).second.value);
	}
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value,GameOption modmapFlag)
{
	OptionType* optType = new OptionType(opt_undefined);
	keyExists( key, modmapFlag, false, optType );
	return setSingleOptionTypeSwitch(key,value,modmapFlag,*optType);
}

bool mmOptionsWrapper::setSingleOption(wxString key,wxString value)
{
	OptionType* optType = new OptionType(opt_undefined);
	if (keyExists(key,ModOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,ModOption,*optType);
	else if (keyExists(key,MapOption,false,optType))
		return setSingleOptionTypeSwitch(key,value,MapOption,*optType);
	else
		return false;
}

wxString mmOptionsWrapper::getSingleValue(wxString key)
{
	for ( GameOption g = 0; g < optionCategoriesCount; g++ )
	{
		wxString tmp = getSingleValue(key,g);
		if (tmp != wxEmptyString)
			return tmp;
	}
	return wxEmptyString;
}

wxString mmOptionsWrapper::getSingleValue(wxString key, GameOption modmapFlag)
{
	OptionType* optType = new OptionType(opt_undefined);

	if ( keyExists(key,modmapFlag,false,optType) )
	{
		switch (*optType)
		{
		case opt_float:
			return wxString::Format( _T("%f"),(*m_floatMaps[modmapFlag])[key].value );
		case opt_bool:
			return wxString::Format(_T("%d"), (*m_boolMaps[modmapFlag])[key].value );
		case opt_int:
			return wxString::Format(_T("%ld"), (*m_intMaps[modmapFlag])[key].value );
		case opt_string:
			return  (*m_stringMaps[modmapFlag])[key].value ;
		case opt_list:
			return (*m_listMaps[modmapFlag])[key].value;
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
			double* d_val = new double;
			bool d_conv_ok = value.ToDouble(d_val);
			float fl_value = float(*d_val);
			if( !d_conv_ok || fl_value < (*m_floatMaps[modmapFlag])[key].min || fl_value > (*m_floatMaps[modmapFlag])[key].max )
			{
				wxLogWarning(_T("recieved number option exceeds boundaries"));
				return false;
			}
			else
				(*m_floatMaps[modmapFlag])[key].value = fl_value;
			break;
		}
		case opt_int :
		{
			//test if min < val < max
			long* l_val = new long;
			bool l_conv_ok = value.ToLong(l_val);
			long int_value = long(*l_val);
			if( !l_conv_ok || int_value < (*m_intMaps[modmapFlag])[key].min || int_value > (*m_intMaps[modmapFlag])[key].max )
			{
				wxLogWarning(_T("recieved number option exceeds boundaries"));
				return false;
			}
			else
				(*m_intMaps[modmapFlag])[key].value = int_value;
			break;
		}
		case opt_bool :
		{
			long* l_val = new long;
			bool l_conv_ok = value.ToLong(l_val);
			if( !l_conv_ok || ( *l_val != 1 && *l_val != 0 ) )
			{
				wxLogWarning(_T("recieved bool option that is neither 0 or 1"));
				return false;
			}
			else
				(*m_boolMaps[modmapFlag])[key].value = bool(*l_val);
			break;
		}
		case opt_string :
		{
			// test if maxlength isn't exceeded
			if ( int(value.Len())> (*m_stringMaps[modmapFlag])[key].max_len )
			{
				wxLogWarning(_T("recieved string option exceeds max_len"));
				return false;
			}
			else
				(*m_stringMaps[modmapFlag])[key].value = value;
			break;
		}
		case opt_list :
		{
			// test if valid value, aka is in list
			int listitemcount = (*m_listMaps[modmapFlag])[key].listitems.size();
			bool valid_string = false;
			int j = 0;
			for (; j < listitemcount; ++j)
			{
				if ( (*m_listMaps[modmapFlag])[key].listitems[j].key == value)
				{
					valid_string = true;
					break;
				}
			}

			if (valid_string)
			{
				(*m_listMaps[modmapFlag])[key].value = (*m_listMaps[modmapFlag])[key].listitems[j].key;
				(*m_listMaps[modmapFlag])[key].cur_choice_index = j;
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
	m_boolMaps[MapOption] 	= new optionMapBool;
	m_floatMaps[MapOption]	= new optionMapFloat;
	m_stringMaps[MapOption] = new optionMapString;
	m_listMaps[MapOption]	= new optionMapList;
	m_intMaps[MapOption] 	= new optionMapInt;

	return loadMapOptions(mapname);
}

wxString mmOptionsWrapper::GetNameListOptValue(wxString key, GameOption flag)
{
	OptionType optType;
	if (flag < ModOption || flag > LastOption - 1)
		return wxEmptyString;
	else if ( keyExists(key,flag,false,&optType) )
	{
		if ( optType == opt_list)
		{
			return ( (*m_listMaps[flag])[key].cbx_choices[ (*m_listMaps[flag])[key].cur_choice_index ] );
		}
	}

	// at this point retrieval failed
	return wxEmptyString;
}

wxString mmOptionsWrapper::GetNameListOptItemKey(wxString optkey, wxString itemname, GameOption flag)
{
	OptionType optType;
	if (flag < ModOption || flag > LastOption - 1)
		return wxEmptyString;
	else if ( keyExists(optkey,flag,false,&optType) )
	{
		if ( optType == opt_list)
		{
			for (ListItemVec::iterator it = (*m_listMaps[flag])[optkey].listitems.begin(); it != (*m_listMaps[flag])[optkey].listitems.end(); ++it)
			{
				if (it->name == itemname)
					return it->key;
			}
		}
	}

	// at this point retrieval failed
	return wxEmptyString;
}
